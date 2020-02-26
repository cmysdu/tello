cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)

set(libh264decoder "${CMAKE_SOURCE_DIR}/src/lib/libh264decoder.so")

find_package(OpenCV REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/src/include")
include_directories(${OpenCV_INCLUDE_DIRS})

aux_source_directory("${CMAKE_SOURCE_DIR}/src/cpp/RemoteController/" remote_controller)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/cpp/CascadeDetectorAdapter/" cascade_detector_adapter)

message(MSG ${remote_controller})
message(MSG ${CMAKE_SOURCE_DIR})

add_executable(h264 main.cpp ${remote_controller} ${cascade_detector_adapter})

target_link_libraries(h264 ${OpenCV_LIBS} ${libh264decoder} pthread)
