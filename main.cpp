#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

bool isValidShape(const string& shape) {
    return shape == "circle" || shape == "half-circle" || shape == "rectangle" || shape == "square" || shape == "triangle";
}

bool isValidColor(const string& color) {
    return color == "green" || color == "pink" || color == "orange" || color == "yellow";
}

struct ShapeColorCombination {
    string shape;
    string color;
};

Mat filterColor(Mat image, string color)
{
    Mat img_hsv, img_mask;
    Scalar lower, upper;

    // Convert the image to HSV format.
    cvtColor(image, img_hsv, COLOR_BGR2HSV);

    if (color == "green") {
        lower = Scalar(38, 105, 0);
        upper = Scalar(75, 255, 188);
    } else if (color == "pink") {
        lower = Scalar(80, 170, 0);
        upper = Scalar(179, 255, 255);
    } else if (color == "orange") {
        lower = Scalar(0, 170, 0);
        upper = Scalar(22, 255, 255);
    } else if (color == "yellow") {
        lower = Scalar(22, 170, 0);
        upper = Scalar(38, 255, 255);
    } else {
        cerr << "Invalid color specified." << endl;
        return Mat(); // Return an empty Mat in case of invalid color
    }

    // Create a color mask
    inRange(img_hsv, lower, upper, img_mask);

    return img_mask;
}

// Function to process the image before use.
Mat processingImage(Mat image) 
{
    Mat img_blurred, img_canny, img_dilated;
    // Apply gaussian blur.
    GaussianBlur(image, img_blurred, Size(3, 3), 3, 0);

    // Apply Canny.
    Canny(img_blurred, img_canny, 25, 75);

    // Apply dilation
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(img_canny, img_dilated, kernel);

    return img_dilated;
}

void getContours(Mat &img_processed, Mat &image, string shape)
{
    string objectType;

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img_processed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {
        // Approximate the contour to reduce the number of vertices
        float epsilon = 0.03 * arcLength(contours[i], true);
        approxPolyDP(contours[i], conPoly[i], epsilon, true);
        
        // Calculate the aspect ratio of the bounding rectangle
        boundRect[i] = boundingRect(contours[i]);
        float aspectRatio = (float)boundRect[i].width / (float)boundRect[i].height;

        // Determine the middle of the contour
        Moments M = moments(contours[i]);
        int posX = M.m10 / M.m00;
        int posY = M.m01 / M.m00; 

        // Determine the number of corners (vertices)
        int numCorners = static_cast<int>(conPoly[i].size());

        // Based on the characteristics, classify the shape
        if (numCorners == 3) {
            objectType = "triangle";
        } else if (numCorners == 4) {
            if (aspectRatio >= 0.95 && aspectRatio <= 1.05) {
                objectType = "square";
            } else {
                objectType = "rectangle";
            }
        } else if (numCorners > 4 && numCorners < 6) {
            objectType = "half-circle";
        } else if (numCorners >= 6) {
            objectType = "circle";
        } 

        if (objectType == shape) {
            // Draw the contour
            drawContours(image, conPoly, i, Scalar(0, 0, 0), 2); 

            // Draw a circle at the middle of the shape
            circle(image, Point(posX, posY), 5, Scalar(0, 0, 0), -1); // -1 for a filled circle

            // Display the X and Y coordinates at the circle
            stringstream ss;
            ss << "X: " << posX << ", Y: " << posY;
            cout << ss.str() << endl;
            putText(image, ss.str(), Point(posX - 50, posY - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 1);

            // Calculate the surface area of each contour and display it
            double area = contourArea(contours[i]);
            cout << "Contour " << shape << " Area: " << area << endl;
        }
    }
}

int main(int argc, char** argv )
{
    // Read an image.
    Mat image = imread("../Neon_shapes.png");

    // Check for no data
    if (! image.data ) 
    {
        cout << "Could not open or find the image.\n";
        return -1; // unsuccessful
    }

    // Vector to store the saved shape and color combinations
    vector<ShapeColorCombination> combinations;

    // Check if a filename is provided as a command-line argument
    if (argc > 1) {
        const char* filename = argv[1];
        ifstream file(filename);
        
        if (!file) {
            cerr << "Error: Unable to open file." << endl;
            return 1;
        }

        string line;
        while (getline(file, line)) {
            // Check if the line is empty or starts with a #
            if (line.empty() || line[0] == '#') {
                continue; // Skip empty lines and comments
            }
            
            stringstream ss(line);
            string shape, color;

            if (ss >> shape >> color) {
                // Validate the "shape" component
                if (isValidShape(shape)) {
                    // Validate the "color" component
                    if (isValidColor(color)) {
                        // Save the combination
                        ShapeColorCombination combination;
                        combination.shape = shape;
                        combination.color = color;
                        combinations.push_back(combination);
                        std::cout << "Shape: " << shape << ", Color: " << color << std::endl;
                    } else {
                        std::cerr << "Invalid color. Supported colors: green, pink, orange, yellow." << std::endl;
                    }
                } else {
                    std::cerr << "Invalid shape. Supported shapes: circle, half-circle, rectangle, square, triangle." << std::endl;
                }
            } else {
                std::cerr << "Invalid message format. Please use '[shape] [color]' format." << std::endl;
            }
        }

        file.close();
    } else {
        // Interactive mode    
        string input;
        while (true)
        {
            cout << "Enter a message ([shape] [color], or 'exit' to quit): ";
            getline(cin, input);
            
            if (input == "exit") {
                cout << "Exiting the program." << endl;
                break;
            } else {
                stringstream ss(input);
                string shape, color;

                if (ss >> shape >> color) {
                    // Validate the "shape" component
                    if (isValidShape(shape)) {
                        // Validate the "color" component
                        if (isValidColor(color)) {
                            // Save the combination
                            ShapeColorCombination combination;
                            combination.shape = shape;
                            combination.color = color;
                            combinations.push_back(combination);
                            std::cout << "Shape: " << shape << ", Color: " << color << std::endl;
                        } else {
                            std::cerr << "Invalid color. Supported colors: green, pink, orange, yellow." << std::endl;
                        }
                    } else {
                        std::cerr << "Invalid shape. Supported shapes: circle, half-circle, rectangle, square, triangle." << std::endl;
                    }
                } else {
                    std::cerr << "Invalid message format. Please use '[shape] [color]' format." << std::endl;
                }
            }
            // Clear the input buffer
            cin.clear();
        }
    }

    Mat img_colorfiltered, img_processed;

    for (const auto& ShapeColorCombination : combinations) {
        clock_t start = clock(); // Start measuring time
        std::cout << "Desired Shape: " << ShapeColorCombination.shape << ", Desired Color: " << ShapeColorCombination.color << std::endl;
        img_colorfiltered = filterColor(image, ShapeColorCombination.color);
        img_processed = processingImage(img_colorfiltered);
        getContours(img_processed, image, ShapeColorCombination.shape);
        clock_t end = clock(); // Stop measuring time

        // Calculate and print the execution time in seconds
        double elapsed_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        cout << "Execution Time: " << elapsed_time << " seconds" << endl;
    }

    // Show the image in window.
    imshow("image", image);
    waitKey(0);

    return 0;
}