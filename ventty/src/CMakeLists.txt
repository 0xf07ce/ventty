add_library(ventty STATIC
    ventty/Color.cpp
    ventty/Utf8.cpp
    ventty/AnsiTerminal.cpp
    ventty/TerminalControl.cpp
    ventty/Input.cpp
    ventty/Screen.cpp
    ventty/AnsiRenderer.cpp
    ventty/Window.cpp
    ventty/AsciiArt.cpp
)

target_include_directories(ventty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
