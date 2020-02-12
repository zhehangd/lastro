cmake_minimum_required(VERSION 3.1)

project(lastro VERSION 1.0.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING
            "No build type selected. Use Release.\n"
            "To use debug mode, use -DCMAKE_BUILD_TYPE=Debug.")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wall -pedantic -Wextra)
  add_compile_options(-pthread)
else()
  message(SEND_ERROR "Unspported platform")
endif()

find_package(Threads REQUIRED)
find_package(glog REQUIRED)
find_package(OpenCV REQUIRED)
find_package(wavelib REQUIRED)
find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)

add_subdirectory(src)

