cmake_minimum_required(VERSION 4.0)
project(ventty)

set(CMAKE_CXX_STANDARD 20)

include(local.cmake OPTIONAL)

option(VENTTY_BUILD_GFX     "Build with Graphics Support"    ON)
option(VENTTY_BUILD_TESTS   "Build Tests"                    ON)

add_subdirectory(src)

if (VENTTY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
