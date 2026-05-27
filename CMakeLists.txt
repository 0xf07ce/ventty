cmake_minimum_required(VERSION 3.20)
project(ventty VERSION 0.3.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# local.cmake is only loaded when ventty is the top-level project.
# When consumed via FetchContent, the parent controls options via cache variables.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(local.cmake OPTIONAL)
endif ()

option(VENTTY_BUILD_TESTS    "Build Tests"                    ON)
option(VENTTY_BUILD_EXAMPLES "Build Examples"                 ON)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_ventty_default_install ON)
else ()
    set(_ventty_default_install OFF)
endif ()
option(VENTTY_INSTALL        "Generate install rules" ${_ventty_default_install})

add_subdirectory(src)

if (VENTTY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (VENTTY_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# --- Install / export rules ---
if (VENTTY_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS ventty
        EXPORT venttyTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(DIRECTORY src/ventty/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ventty
        FILES_MATCHING
            PATTERN "*.h"
            PATTERN "*.hpp")

    install(EXPORT venttyTargets
        FILE venttyTargets.cmake
        NAMESPACE ventty::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ventty)

    configure_package_config_file(
        cmake/venttyConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/venttyConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ventty)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/venttyConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/venttyConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/venttyConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ventty)
endif ()
