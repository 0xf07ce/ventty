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
    ventty/art/BrailleCanvas.h
    ventty/art/BrailleCanvas.cpp

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

target_include_directories(ventty PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)