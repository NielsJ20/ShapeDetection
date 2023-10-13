#pragma once
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

struct ShapeColorCombination
{
    std::string shape;
    std::string color;
};

class ShapeColorDetector
{
public:
    ShapeColorDetector();
    void initializeCombinationsFromFile(const std::string &inputFilename);
    void initializeCombinationsFromInteractiveInput();
    void processFrame(cv::Mat &frame);

private:
    std::vector<ShapeColorCombination> combinations;
    cv::Mat filterColor(const cv::Mat &image, const std::string &color);
    cv::Mat processingImage(const cv::Mat &image);
    void getContours(cv::Mat &imgProcessed, cv::Mat &image, const std::string &shape);
    bool checkShapeColor(const std::string &shape, const std::string &color);
    bool isValidShape(const std::string &shape);
    bool isValidColor(const std::string &color);
};
