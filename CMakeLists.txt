cmake_minimum_required(VERSION 3.15)

project(tello)

set(CMAKE_CXX_STANDARD 11)

option(GEN_DECODER "generate h264 decoder" OFF)
if (GEN_DECODER)
    add_subdirectory(h264decoder)
endif()

add_subdirectory(src)
