# Interactive demos (require a real terminal)
add_executable(demo_terminal demo_terminal.cpp)
target_link_libraries(demo_terminal PRIVATE ventty)

add_executable(demo_color demo_color.cpp)
target_link_libraries(demo_color PRIVATE ventty)

add_executable(demo_hangul demo_hangul.cpp)
target_link_libraries(demo_hangul PRIVATE ventty)

add_executable(demo_window demo_window.cpp)
target_link_libraries(demo_window PRIVATE ventty)

add_executable(demo_ascii_art demo_ascii_art.cpp)
target_link_libraries(demo_ascii_art PRIVATE ventty)

# Widget demos
add_executable(demo_widget_label demo_widget_label.cpp)
target_link_libraries(demo_widget_label PRIVATE ventty)

add_executable(demo_widget_listview demo_widget_listview.cpp)
target_link_libraries(demo_widget_listview PRIVATE ventty)

add_executable(demo_widget_dialog demo_widget_dialog.cpp)
target_link_libraries(demo_widget_dialog PRIVATE ventty)

add_executable(demo_widget_menu demo_widget_menu.cpp)
target_link_libraries(demo_widget_menu PRIVATE ventty)

# SDL3 graphical demo
if (VENTTY_BUILD_GFX)
    add_executable(demo_gfx_terminal demo_gfx_terminal.cpp)
    target_link_libraries(demo_gfx_terminal PRIVATE ventty_gfx)
endif ()
