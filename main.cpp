#include <iostream>
#include <opencv2/opencv.hpp>
#include "ShapeColorDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    ShapeColorDetector detector;

    if (argc > 1)
    {
        const char *filename = argv[1];
        detector.initializeCombinationsFromFile(filename);
    }
    else
    {
        detector.initializeCombinationsFromInteractiveInput();
    }

    VideoCapture cap(0); // Declaring an object to capture a stream of frames from the default camera

    if (!cap.isOpened())
    {
        cout << "No video stream detected" << endl;
        system("pause");
        return -1;
    }

    while (true)
    {
        Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            break;
        }

        detector.processFrame(frame);

        imshow("Video Player", frame);

        char c = (char)waitKey(500); // Allowing 25 milliseconds frame processing time and initiating a break condition
        if (c == 27)
        { // If 'Esc' is entered, break the loop
            break;
        }
    }
    cap.release(); // Releasing the buffer memory

    return 0;
}
