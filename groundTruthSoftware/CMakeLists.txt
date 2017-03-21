cmake_minimum_required(VERSION 2.8)
project( GroundTruth )
find_package( OpenCV REQUIRED )
set(CMAKE_BUILD_TYPE "None")
set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -pedantic -g")
include_directories(SYSTEM
   ${OPENCV_INCLUDE_DIR}
)
include_directories(src)
add_executable( GroundTruth src/main.cpp src/groundTruth.cpp src/groundTruth.h)
target_link_libraries( GroundTruth ${OpenCV_LIBS} )
