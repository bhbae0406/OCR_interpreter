cmake_minimum_required(VERSION 2.8)
project(SegmentPage )
find_package( OpenCV REQUIRED )
set(CMAKE_BUILD_TYPE "None")
set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -pedantic -g")
include_directories(SYSTEM
   ${OPENCV_INCLUDE_DIR}
)

include_directories(src)

add_executable( SegmentPage src/main.cpp src/segment.cpp src/segment.h src/textLine.h src/textLine.cpp src/block.h src/block.cpp)
target_link_libraries( SegmentPage ${OpenCV_LIBS} )
