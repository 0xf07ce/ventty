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
