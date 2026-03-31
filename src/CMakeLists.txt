add_library(ventty STATIC
    # core
    ventty/core/Color.h
    ventty/core/Color.cpp
    ventty/core/Cell.hpp
    ventty/core/Rect.h
    ventty/core/Rect.cpp
    ventty/core/Utf8.h
    ventty/core/Utf8.cpp
    ventty/core/Window.h
    ventty/core/Window.cpp

    # terminal
    ventty/terminal/TerminalBase.h
    ventty/terminal/Terminal.h
    ventty/terminal/Terminal.cpp
    ventty/terminal/Renderer.h
    ventty/terminal/Renderer.cpp
    ventty/terminal/Input.h
    ventty/terminal/Input.cpp

    # art
    ventty/art/AsciiArt.h
    ventty/art/AsciiArt.cpp

    # widget
    ventty/widget/Widget.h
    ventty/widget/Widget.cpp
    ventty/widget/Label.h
    ventty/widget/Label.cpp
    ventty/widget/ListView.h
    ventty/widget/ListView.cpp
    ventty/widget/Dialog.h
    ventty/widget/Dialog.cpp
    ventty/widget/InputDialog.h
    ventty/widget/InputDialog.cpp
    ventty/widget/Menu.h
    ventty/widget/Menu.cpp

    # umbrella
    ventty/ventty.h
)

target_include_directories(ventty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if (VENTTY_BUILD_GFX)

    add_library(ventty_gfx STATIC
        ventty/gfx/GfxFont.h
        ventty/gfx/GfxFont.cpp
        ventty/gfx/GfxRenderer.h
        ventty/gfx/GfxRenderer.cpp
        ventty/gfx/GfxInput.h
        ventty/gfx/GfxInput.cpp
        ventty/gfx/GfxTerminal.h
        ventty/gfx/GfxTerminal.cpp

        # embedded fonts
        ventty/gfx/fonts/EmbeddedFonts.h
        ventty/gfx/fonts/cp437_8x16.cpp
        ventty/gfx/fonts/hangul_16x16.cpp
        ventty/gfx/fonts/jamo_16x16.cpp

        # umbrella
        ventty/ventty_gfx.h
    )
    target_include_directories(ventty_gfx PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/dep/include)
    target_link_libraries(ventty_gfx PUBLIC
        ventty
        SDL3::SDL3)

endif ()