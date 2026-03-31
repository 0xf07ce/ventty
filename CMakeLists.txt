cmake_minimum_required(VERSION 3.20)
project(ventty VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# local.cmake is only loaded when ventty is the top-level project.
# When consumed via FetchContent, the parent controls options via cache variables.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(local.cmake OPTIONAL)
endif ()

option(VENTTY_BUILD_GFX      "Build with Graphics Support"    OFF)
option(VENTTY_BUILD_TESTS    "Build Tests"                    ON)
option(VENTTY_BUILD_EXAMPLES "Build Examples"                 ON)

# --- SDL3 dependency resolution ---
# When VENTTY_BUILD_GFX is OFF, SDL3 is not required at all.
# When VENTTY_BUILD_GFX is ON:
#   1. Try find_package() to use a locally installed SDL3.
#   2. If not found, fetch from GitHub via FetchContent.
if (VENTTY_BUILD_GFX)
    find_package(SDL3 QUIET)
    if (NOT SDL3_FOUND)
        include(FetchContent)
        message(STATUS "SDL3 not found locally, fetching from GitHub...")
        FetchContent_Declare(SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG        main
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(SDL3)
    endif ()
endif ()

add_subdirectory(src)

if (VENTTY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (VENTTY_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
