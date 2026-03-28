cmake_minimum_required(VERSION 4.0)
project(venttyx)

set(CMAKE_CXX_STANDARD 20)

include(local.cmake OPTIONAL)

add_subdirectory(ventty)
add_subdirectory(ventty_gfx)
add_subdirectory(mdir)

