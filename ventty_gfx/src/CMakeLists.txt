add_library(ventty_gfx STATIC
    ventty_gfx/Font.cpp
    ventty_gfx/GfxRenderer.cpp
    ventty_gfx/GfxTerminal.cpp
    ventty_gfx/Input.cpp
)

target_include_directories(ventty_gfx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ventty_gfx PUBLIC
    ventty
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image)
