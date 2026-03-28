# fs library
add_library(fs STATIC
    fs/FileInfo.cpp
    fs/FileSystem.cpp
)
target_include_directories(fs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# widget library
add_library(widget STATIC
    widget/Label.cpp
    widget/ListView.cpp
    widget/Dialog.cpp
    widget/InputDialog.cpp
    widget/Menu.cpp
)
target_include_directories(widget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(widget PUBLIC ventty)

# view library
add_library(view STATIC
    view/FunctionBar.cpp
    view/PathBar.cpp
    view/FilePanel.cpp
    view/StatusBar.cpp
    view/InfoBar.cpp
)
target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(view PUBLIC widget fs)

# app library
add_library(app STATIC
    app/Application.cpp
)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PUBLIC view)

# Main executable
add_executable(mdir main.cpp)
target_link_libraries(mdir app dep)
