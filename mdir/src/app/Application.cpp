#include "Application.h"

#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

namespace mdir::app
{

using Key = ventty::KeyEvent::Key;

Application::Application()
{
    _theme = view::Theme::mdir();
}

Application::~Application()
{
    cleanup();
}

int Application::run()
{
    init();
    _running = true;

    while (_running)
    {
        updateUI();
        draw();
        handleInput();
    }

    cleanup();
    return 0;
}

void Application::quit()
{
    _running = false;
}

void Application::init()
{
    // Create terminal
    _terminal = std::make_unique<ventty::TerminalControl>();
    _terminal->enterRawMode();
    _terminal->enableAlternateScreen();
    _terminal->hideCursor();

    // Create screen and renderer
    _screen = std::make_unique<ventty::Screen>();
    _ansiRenderer = std::make_unique<ventty::AnsiRenderer>();
    auto const size = _terminal->size();
    _screen->resize(size.width, size.height);

    // Create input
    _input = std::make_unique<ventty::Input>();

    // Create menu bar
    _menuBar = std::make_unique<widget::MenuBar>();
    setupMenus();

    // Create views
    _functionBar = std::make_unique<view::FunctionBar>();
    _functionBar->setTheme(_theme);

    _pathBar = std::make_unique<view::PathBar>();
    _pathBar->setTheme(_theme);

    _filePanel = std::make_unique<view::FilePanel>();
    _filePanel->setTheme(_theme);
    _filePanel->setFocused(true);
    _filePanel->setOnExecute([this](mdir::fs::FileInfo const & file)
    {
        executeFile(file);
    });

    _statusBar = std::make_unique<view::StatusBar>();
    _statusBar->setTheme(_theme);

    _infoBar = std::make_unique<view::InfoBar>();
    _infoBar->setTheme(_theme);

    // Handle resize
    _terminal->setResizeHandler([this](ventty::Size newSize)
    {
        _screen->resize(newSize.width, newSize.height);
        resize();
        _ansiRenderer->invalidate();
    });

    resize();
}

void Application::setupMenus()
{
    // Mdir menu
    auto mdirMenu = std::make_shared<widget::Menu>();
    mdirMenu->addItem({"About", "", Key::None, false, true, [this]() { showAbout(); }});
    mdirMenu->addItem({"Help", "F1", Key::F1, false, true, nullptr});
    mdirMenu->addSeparator();
    mdirMenu->addItem({"Refresh", "F5", Key::F5, false, true, [this]() { _filePanel->refresh(); }});
    mdirMenu->addItem({"Toggle hidden", "Ctrl-H", Key::None, false, true, [this]() {
        _filePanel->fileSystem().setShowHidden(!_filePanel->fileSystem().showHidden());
    }});
    mdirMenu->addSeparator();
    mdirMenu->addItem({"Quit", "Alt-X", Key::None, false, true, [this]() { quit(); }});

    // File menu
    auto fileMenu = std::make_shared<widget::Menu>();
    fileMenu->addItem({"View", "F3", Key::F3, false, false, nullptr});
    fileMenu->addItem({"Edit", "F4", Key::F4, false, false, nullptr});
    fileMenu->addSeparator();
    fileMenu->addItem({"Copy", "F5", Key::F5, false, false, nullptr});
    fileMenu->addItem({"Move/Rename", "F6", Key::F6, false, false, nullptr});
    fileMenu->addSeparator();
    fileMenu->addItem({"Make Directory", "F7", Key::F7, false, true, [this]() { makeDirectory(); }});
    fileMenu->addItem({"Delete", "F8", Key::F8, false, true, [this]() { deleteSelected(); }});

    // Direct menu
    auto directMenu = std::make_shared<widget::Menu>();
    directMenu->addItem({"MCD (Tree)", "F10", Key::F10, false, false, nullptr});
    directMenu->addSeparator();
    directMenu->addItem({"Go to parent", "Backspace", Key::Backspace, false, true, [this]() { _filePanel->goUp(); }});
    directMenu->addItem({"Go to root", "/", Key::None, false, true, [this]() {
        _filePanel->fileSystem().changeDirectory("/");
    }});
    directMenu->addItem({"Go to home", "~", Key::None, false, true, [this]() {
        char const * home = std::getenv("HOME");
        if (home) _filePanel->fileSystem().changeDirectory(home);
    }});

    _menuBar->addMenu("Mdir", mdirMenu);
    _menuBar->addMenu("File", fileMenu);
    _menuBar->addMenu("Direct", directMenu);
}

void Application::cleanup()
{
    if (_terminal)
    {
        _terminal->showCursor();
        _terminal->disableAlternateScreen();
        _terminal->exitRawMode();
    }
}

void Application::resize()
{
    int const w = _screen->width();
    int const h = _screen->height();

    // Function bar at top (line 0)
    _functionBar->setRect(0, 0, w, 1);

    // Path bar (line 1)
    _pathBar->setRect(0, 1, w, 1);

    // File panel (line 2 to h-3, includes === top/bottom)
    int const panelY = 2;
    int const panelH = h - 4;
    _filePanel->setRect(0, panelY, w, panelH);

    // Status bar
    _statusBar->setRect(0, h - 2, w, 1);

    // Info bar at bottom
    _infoBar->setRect(0, h - 1, w, 1);

    // Menu bar overlays line 0 when active
    _menuBar->setRect(0, 0, w, 1);
}

void Application::updateUI()
{
    auto const & fs = _filePanel->fileSystem();

    _pathBar->setPath(fs.currentPath().string());

    std::string vol = fs.currentPath().filename().string();
    if (vol.empty())
    {
        vol = "ROOT";
    }
    _pathBar->setVolume(vol);

    _statusBar->setFileCount(fs.fileCount());
    _statusBar->setDirCount(fs.dirCount());
    _statusBar->setTotalSize(fs.totalSize());
    _statusBar->setFreeSpace(fs.freeSpace());
    _statusBar->setTotalSpace(fs.totalSpace());

    auto const * selected = _filePanel->selectedFile();
    if (selected)
    {
        _infoBar->setCurrentFile(selected->name);
    }
}

void Application::draw()
{
    _screen->setStyle({_theme.foreground, _theme.background});
    _screen->clear();

    _functionBar->draw(*_screen);
    _pathBar->draw(*_screen);
    _filePanel->draw(*_screen);
    _statusBar->draw(*_screen);
    _infoBar->draw(*_screen);

    if (_menuBar->isActive())
    {
        _menuBar->draw(*_screen);
    }

    _ansiRenderer->render(*_screen);
}

void Application::handleInput()
{
    auto event = _input->read(100);

    if (event.key == Key::None)
    {
        return;
    }

    // Menu bar handles input first when active
    if (_menuBar->isActive())
    {
        if (_menuBar->handleKey(event))
        {
            return;
        }
    }

    if (event.key == Key::F12)
    {
        _menuBar->activate();
        return;
    }

    if (event.key == Key::Char && (event.ch == 'q' || event.ch == 'Q') && !event.alt && !event.ctrl)
    {
        quit();
        return;
    }

    if (event.key == Key::Char && event.alt && (event.ch == 'x' || event.ch == 'X'))
    {
        quit();
        return;
    }

    if (event.key == Key::F7)
    {
        makeDirectory();
        return;
    }

    if (event.key == Key::F8)
    {
        deleteSelected();
        return;
    }

    if (event.key == Key::F5 ||
        (event.key == Key::Char && event.ctrl && (event.ch == 'r' || event.ch == 'R')))
    {
        _filePanel->refresh();
        _ansiRenderer->invalidate();
        return;
    }

    _filePanel->handleKey(event);
}

void Application::executeFile(mdir::fs::FileInfo const & file)
{
    if (!file.isExecutable && file.type != mdir::fs::FileType::Executable)
    {
        return;
    }

    _terminal->showCursor();
    _terminal->disableAlternateScreen();
    _terminal->exitRawMode();

    std::printf("\033[2J\033[H");
    std::fflush(stdout);

    pid_t pid = fork();
    if (pid == 0)
    {
        std::string const pathStr = file.path.string();
        execl(pathStr.c_str(), pathStr.c_str(), nullptr);
        std::exit(127);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }

    std::printf("\n\033[7m Press any key to continue... \033[0m");
    std::fflush(stdout);

    _terminal->enterRawMode();
    _input->read(-1);
    _terminal->exitRawMode();

    _terminal->enterRawMode();
    _terminal->enableAlternateScreen();
    _terminal->hideCursor();
    _ansiRenderer->invalidate();

    _filePanel->refresh();
}

widget::DialogResult Application::showDialog(
    std::string const & title,
    std::string const & message,
    std::vector<std::pair<std::string, widget::DialogResult>> const & buttons)
{
    widget::Dialog dialog;
    dialog.setTitle(title);
    dialog.setMessage(message);
    for (auto const & [label, result] : buttons)
    {
        dialog.addButton(label, result);
    }
    dialog.autoSize(_screen->width(), _screen->height());

    widget::DialogResult result = widget::DialogResult::None;
    while (result == widget::DialogResult::None && _running)
    {
        draw();
        dialog.draw(*_screen);
        _ansiRenderer->render(*_screen);

        auto event = _input->read(-1);
        dialog.handleKey(event);
        result = dialog.result();
    }

    _ansiRenderer->invalidate();
    return result;
}

std::string Application::showInputDialog(
    std::string const & title,
    std::string const & prompt,
    std::string const & defaultValue)
{
    widget::InputDialog dialog;
    dialog.setTitle(title);
    dialog.setPrompt(prompt);
    dialog.setValue(defaultValue);
    dialog.autoSize(_screen->width(), _screen->height());

    widget::DialogResult result = widget::DialogResult::None;
    while (result == widget::DialogResult::None && _running)
    {
        draw();
        dialog.draw(*_screen);
        _ansiRenderer->render(*_screen);

        auto event = _input->read(-1);
        dialog.handleKey(event);
        result = dialog.result();
    }

    _ansiRenderer->invalidate();

    if (result == widget::DialogResult::Ok)
    {
        return dialog.value();
    }
    return "";
}

void Application::deleteSelected()
{
    auto const * file = _filePanel->selectedFile();
    if (!file || file->isParentDir())
    {
        return;
    }

    std::string message = "Delete '" + file->name + "'?";

    auto result = showDialog("Delete", message, {
        {"Yes", widget::DialogResult::Yes},
        {"No", widget::DialogResult::No}
    });

    if (result == widget::DialogResult::Yes)
    {
        std::error_code ec;
        std::filesystem::remove_all(file->path, ec);
        _filePanel->refresh();
    }
}

void Application::makeDirectory()
{
    std::string name = showInputDialog("Make Directory", "Directory name:");

    if (!name.empty())
    {
        auto path = _filePanel->fileSystem().currentPath() / name;
        std::error_code ec;
        std::filesystem::create_directory(path, ec);
        if (ec)
        {
            showDialog("Error", "Failed to create directory: " + ec.message(), {
                {"OK", widget::DialogResult::Ok}
            });
        }
        _filePanel->refresh();
    }
}

void Application::showAbout()
{
    showDialog("About MDIR", "MDIR v0.2 - MDIR Clone for Linux", {
        {"OK", widget::DialogResult::Ok}
    });
}

} // namespace mdir::app
