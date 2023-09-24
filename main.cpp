#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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

int main(int argc, char** argv )
{
    // Read an image.
    Mat image = imread("../Neon_shapes.png");

    // Show the image in window.
    imshow("image", image);
    imshow("processed image", processingImage(image));
    waitKey(0);

    return 0;
}