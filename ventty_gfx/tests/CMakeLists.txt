# Interactive demo (requires a real terminal + SDL3)
add_executable(gfx_demo_terminal demo_terminal.cpp)
target_link_libraries(gfx_demo_terminal PRIVATE ventty_gfx)

# Copy assets next to executable at build time
add_custom_command(TARGET gfx_demo_terminal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
        "$<TARGET_FILE_DIR:gfx_demo_terminal>/assets"
)
