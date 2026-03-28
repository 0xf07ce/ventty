#ifndef MDIR_APP_APPLICATION_H
#define MDIR_APP_APPLICATION_H

#include <ventty/Color.h>
#include <ventty/Screen.h>
#include <ventty/AnsiRenderer.h>
#include <ventty/TerminalControl.h>
#include <ventty/Input.h>

#include "../widget/Menu.h"
#include "../widget/Dialog.h"
#include "../widget/InputDialog.h"
#include "../view/FunctionBar.h"
#include "../view/PathBar.h"
#include "../view/FilePanel.h"
#include "../view/StatusBar.h"
#include "../view/InfoBar.h"
#include "../view/Theme.h"

#include <memory>

namespace mdir::app
{

class Application
{
public:
    Application();
    ~Application();

    int run();
    void quit();

private:
    void init();
    void cleanup();
    void resize();
    void draw();
    void handleInput();
    void updateUI();
    void executeFile(mdir::fs::FileInfo const & file);

    // Menu setup
    void setupMenus();

    // File operations
    void deleteSelected();
    void makeDirectory();
    void showAbout();

    // Dialog helpers
    widget::DialogResult showDialog(std::string const & title,
                                    std::string const & message,
                                    std::vector<std::pair<std::string, widget::DialogResult>> const & buttons);
    std::string showInputDialog(std::string const & title,
                                std::string const & prompt,
                                std::string const & defaultValue = "");

    bool _running = false;

    // ventty
    std::unique_ptr<ventty::TerminalControl> _terminal;
    std::unique_ptr<ventty::Screen> _screen;
    std::unique_ptr<ventty::AnsiRenderer> _ansiRenderer;
    std::unique_ptr<ventty::Input> _input;

    // Views
    std::unique_ptr<view::FunctionBar> _functionBar;
    std::unique_ptr<view::PathBar> _pathBar;
    std::unique_ptr<view::FilePanel> _filePanel;
    std::unique_ptr<view::StatusBar> _statusBar;
    std::unique_ptr<view::InfoBar> _infoBar;

    // Menu
    std::unique_ptr<widget::MenuBar> _menuBar;

    view::Theme _theme;
};

} // namespace mdir::app

#endif // MDIR_APP_APPLICATION_H
