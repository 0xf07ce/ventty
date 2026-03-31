cmake_minimum_required(VERSION 4.0)
project(ventty VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include(local.cmake OPTIONAL)

option(VENTTY_BUILD_GFX      "Build with Graphics Support"    ON)
option(VENTTY_BUILD_TESTS    "Build Tests"                    ON)
option(VENTTY_BUILD_EXAMPLES "Build Examples"                 ON)

add_subdirectory(src)

if (VENTTY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (VENTTY_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
