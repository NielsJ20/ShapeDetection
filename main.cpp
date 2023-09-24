#include <iostream>
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

void getContours(Mat &img_processed, Mat &image)
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

        drawContours(image, conPoly, i, Scalar(0, 0, 0), 2); 
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

    Mat img_processed = processingImage(image);

    getContours(img_processed, image);

    // Show the image in window.
    imshow("image", image);
    imshow("processed image", img_processed);
    waitKey(0);

    return 0;
}