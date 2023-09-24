# cmake needs this line
cmake_minimum_required(VERSION 3.5)

# Define project name
project( ImageDetection )

# Find OpenCV, you may need to set OpenCV_DIR variable to the absolute path to the directory 
# containing OpenCVConfig.cmake file via the command line or GUI
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Declare the executable target built from your sources
add_executable( OpenCVImageDetection main.cpp )

# Link your application with OpenCV libraries
target_link_libraries( OpenCVImageDetection ${OpenCV_LIBS} )
