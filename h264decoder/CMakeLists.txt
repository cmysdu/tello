cmake_minimum_required(VERSION 3.15)
project(h264decoder)

set(CMAKE_CXX_STANDARD 11)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/src/lib)

add_library(h264decoder SHARED h264decoder.cpp)

target_link_libraries(h264decoder avcodec swscale avutil)

install(TARGETS h264decoder LIBRARY DESTINATION "${CMAKE_SOURCE_DIR}/src/lib")