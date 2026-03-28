# Unit tests (no terminal required — headless)
add_executable(test_utf8 test_utf8.cpp)
target_link_libraries(test_utf8 PRIVATE ventty)

add_executable(test_color test_color.cpp)
target_link_libraries(test_color PRIVATE ventty)

add_executable(test_cell test_cell.cpp)
target_link_libraries(test_cell PRIVATE ventty)

add_executable(test_window test_window.cpp)
target_link_libraries(test_window PRIVATE ventty)

add_executable(test_ascii_art test_ascii_art.cpp)
target_link_libraries(test_ascii_art PRIVATE ventty)

add_executable(test_screen test_screen.cpp)
target_link_libraries(test_screen PRIVATE ventty)

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
