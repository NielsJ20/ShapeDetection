#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv )
{
    // Read an image.
    Mat image = imread("../Neon_shapes.png");

    // Show the image in window.
    imshow("image", image);
    waitKey(0);

    return 0;
}