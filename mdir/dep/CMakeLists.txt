add_library(dep INTERFACE)
target_include_directories(dep INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_link_directories(dep INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
