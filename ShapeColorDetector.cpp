#include "ShapeColorDetector.h"

using namespace std;
using namespace cv;

ShapeColorDetector::ShapeColorDetector()
{
}

cv::Mat ShapeColorDetector::filterColor(const cv::Mat &image, const std::string &color)
{
    Mat img_hsv, img_mask;
    Scalar lower, upper;

    // Convert the image to HSV format.
    cvtColor(image, img_hsv, COLOR_BGR2HSV);

    if (color == "groen")
    {
        lower = Scalar(38, 41, 50);
        upper = Scalar(90, 255, 255);
    }
    else if (color == "roze")
    {
        lower = Scalar(155, 50, 126);
        upper = Scalar(179, 255, 255);
    }
    else if (color == "oranje")
    {
        lower = Scalar(0, 80, 130);
        upper = Scalar(18, 255, 255);
    }
    else if (color == "geel")
    {
        lower = Scalar(19, 41, 124);
        upper = Scalar(38, 255, 255);
    }
    else
    {
        cerr << "Invalid color specified." << endl;
        return Mat(); // Return an empty Mat in case of invalid color
    }

    // Create a color mask
    inRange(img_hsv, lower, upper, img_mask);

    return img_mask;
}

cv::Mat ShapeColorDetector::processingImage(const cv::Mat &image)
{
    Mat img_blurred, img_canny, img_dilated, img_erode;
    // Apply gaussian blur.
    GaussianBlur(image, img_blurred, Size(3, 3), 3, 0);

    // Apply erode
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(img_blurred, img_erode, kernel);

    // Apply dilation
    dilate(img_erode, img_dilated, kernel);

    return img_dilated;
}

void ShapeColorDetector::getContours(cv::Mat &img_processed, cv::Mat &image, const std::string &shape)
{
    string objectType;

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img_processed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {
        // Calculate the surface area
        double area = contourArea(contours[i]);

        if (area > 1000)
        {
            // Approximate the contour to reduce the number of vertices
            double epsilon = 0.03 * arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], epsilon, true);

            // Calculate the aspect ratio of the bounding rectangle
            boundRect[i] = boundingRect(contours[i]);
            float aspectRatio = (float)boundRect[i].width / (float)boundRect[i].height;

            // Determine the middle of the contour
            Moments M = moments(contours[i]);
            int posX = static_cast<int>(M.m10 / M.m00);
            int posY = static_cast<int>(M.m01 / M.m00);

            // Determine the number of corners (vertices)
            int numCorners = static_cast<int>(conPoly[i].size());

            // Based on the characteristics, classify the shape
            if (numCorners == 3)
            {
                objectType = "driehoek";
            }
            else if (numCorners == 4)
            {
                if (aspectRatio >= 0.95 && aspectRatio <= 1.05)
                {
                    objectType = "vierkant";
                }
                else
                {
                    objectType = "rechthoek";
                }
            }
            else if (numCorners > 4 && numCorners < 6)
            {
                objectType = "halve-cirkel";
            }
            else if (numCorners >= 6)
            {
                objectType = "cirkel";
            }

            if (objectType == shape)
            {
                // Draw the contour
                drawContours(image, conPoly, static_cast<int>(i), Scalar(0, 0, 0), 2);

                // Draw a circle at the middle of the shape
                circle(image, Point(posX, posY), 5, Scalar(0, 0, 0), -1); // -1 for a filled circle

                // Display the X and Y coordinates of the middle
                stringstream ss;
                ss << "X: " << posX << ", Y: " << posY;
                cout << ss.str() << endl;
                putText(image, ss.str(), Point(posX - 50, posY - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 1);

                // Display the surface area
                cout << "Contour " << shape << " Area: " << area << endl;
            }
        }
    }
}

bool ShapeColorDetector::checkShapeColor(const std::string &shape, const std::string &color)
{
    // Validate the "shape" component
    if (isValidShape(shape))
    {
        // Validate the "color" component
        if (isValidColor(color))
        {
            return true;
        }
        else
        {
            std::cerr << "Invalid color. Supported colors: groen, roze, oranje, geel." << std::endl;
        }
    }
    else
    {
        std::cerr << "Invalid shape. Supported shapes: cirkel, halve-cirkel, rechthoek, vierkant, driehoek." << std::endl;
    }
    return false;
}

bool ShapeColorDetector::isValidShape(const std::string &shape)
{
    return shape == "cirkel" || shape == "halve-cirkel" || shape == "rechthoek" || shape == "vierkant" || shape == "driehoek";
}

bool ShapeColorDetector::isValidColor(const std::string &color)
{
    return color == "groen" || color == "roze" || color == "oranje" || color == "geel";
}

void ShapeColorDetector::processFrame(Mat &frame)
{

    // Process each ShapeColorCombination
    for (const auto &combination : combinations)
    {
        Mat img_processed, img_colorfiltered;
        img_colorfiltered = filterColor(frame, combination.color);
        img_processed = processingImage(img_colorfiltered);

        // Detect the specified shape and add information to the frame
        getContours(img_processed, frame, combination.shape);
    }
}

void ShapeColorDetector::initializeCombinationsFromFile(const std::string &inputFilename)
{
    if (inputFilename.empty())
    {
        std::cerr << "No input file specified." << std::endl;
        return;
    }

    std::ifstream file(inputFilename);
    if (!file)
    {
        std::cerr << "Error: Unable to open file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::stringstream ss(line);
        std::string shape, color;

        if (ss >> shape >> color)
        {
            if (checkShapeColor(shape, color))
            {
                ShapeColorCombination combination;
                combination.shape = shape;
                combination.color = color;
                combinations.push_back(combination);
                std::cout << "Shape: " << shape << ", Color: " << color << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid message format in the file. Please use '[shape] [color]' format." << std::endl;
        }
    }

    file.close();
}

void ShapeColorDetector::initializeCombinationsFromInteractiveInput()
{
    std::string input;
    while (true)
    {
        std::cout << "Enter a message ([shape] [color], or 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit")
        {
            std::cout << "Exiting the program." << std::endl;
            break;
        }
        else
        {
            std::stringstream ss(input);
            std::string shape, color;

            if (ss >> shape >> color)
            {
                if (checkShapeColor(shape, color))
                {
                    ShapeColorCombination combination;
                    combination.shape = shape;
                    combination.color = color;
                    combinations.push_back(combination);
                    std::cout << "Shape: " << shape << ", Color: " << color << std::endl;
                }
            }
            else
            {
                std::cerr << "Invalid message format. Please use '[shape] [color]' format." << std::endl;
            }
        }
        // Clear the input buffer
        std::cin.clear();
    }
}
