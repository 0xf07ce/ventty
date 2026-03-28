cmake_minimum_required(VERSION 4.0)
project(ventty)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

option(VENTTY_BUILD_TESTS   "Build Tests"    ON)

add_subdirectory(src)

if (VENTTY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
