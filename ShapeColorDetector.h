#pragma once
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

/**
 * @brief Represents a combination of shape and color.
 * 
 */
struct ShapeColorCombination
{
    std::string shape;
    std::string color;
};

class ShapeColorDetector
{
public:
    /**
     * @brief Constructor for the ShapeColorDetector class.
     */
    ShapeColorDetector();

    /**
     * @brief Initialize  all the shape-color combinations from a batchfile.
     * @param inputFilename 
     */
    void initializeCombinationsFromFile(const std::string &inputFilename);

    /**
     * @brief Initialize shape-color combinations interactively from user input.
     */
    void initializeCombinationsFromInteractiveInput();

    /**
     * @brief Process an image frame to detect specified shapes of certain colors.
     * @param frame An input image frame from the video capture.
     */
    void processFrame(cv::Mat &frame);

private:
    std::vector<ShapeColorCombination> combinations;

    /**
     * @brief Filter the image to retain only a specific color range.
     * @param image The input image.
     * @param color The color to be retained.
     * @return Filtered image.
     */
    cv::Mat filterColor(const cv::Mat &image, const std::string &color);

    /**
     * @brief Process the image before shape detection.
     * @param image The input image.
     * @return Processed image.
     */
    cv::Mat processingImage(const cv::Mat &image);

        /**
     * @brief Find and classify shapes within the processed image.
     * @param img_processed The processed image.
     * @param image The original image for visualization.
     * @param shape The specified shape for detection.
     */
    void getContours(cv::Mat &img_processed, cv::Mat &image, const std::string &shape);

    /**
     * @brief Check if a given combination of shape and color is valid.
     * @param shape The shape to check.
     * @param color The color to check.
     * @return True if the combination is valid, otherwise false.
     */
    bool checkShapeColor(const std::string &shape, const std::string &color);

    /**
     * @brief Check if a shape string is valid.
     * @param shape The shape to check.
     * @return True if the shape is valid, otherwise false.
     */
    bool isValidShape(const std::string &shape);

    /**
     * @brief Check if a color string is valid.
     * @param color The color to check.
     * @return True if the color is valid, otherwise false.
     */
    bool isValidColor(const std::string &color);
};
