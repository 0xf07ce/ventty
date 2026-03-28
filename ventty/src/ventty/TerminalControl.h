#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

struct termios;

namespace ventty
{

struct Size
{
    int width = 0;
    int height = 0;
};

/// Low-level terminal control for widget-based applications.
/// Manages raw mode, alternate screen, cursor, mouse, and resize signals.
/// Use this when you need manual control over the terminal rather than
/// the integrated Terminal class.
class TerminalControl
{
public:
    TerminalControl();
    ~TerminalControl();

    TerminalControl(TerminalControl const &) = delete;
    TerminalControl & operator=(TerminalControl const &) = delete;

    void enterRawMode();
    void exitRawMode();
    bool isRawMode() const;

    void enableAlternateScreen();
    void disableAlternateScreen();

    void enableMouse();
    void disableMouse();

    void showCursor();
    void hideCursor();

    Size size() const;

    void write(std::string_view data);
    void flush();

    // Signal handling
    using ResizeHandler = std::function<void(Size)>;
    void setResizeHandler(ResizeHandler handler);

    static TerminalControl & instance();

private:
    void setupSignalHandlers();
    static void handleSigwinch(int sig);
    static void handleTermSignal(int sig);

    struct termios * _originalTermios = nullptr;
    bool _rawMode = false;
    bool _alternateScreen = false;
    bool _mouseEnabled = false;
    bool _cursorVisible = true;
    ResizeHandler _resizeHandler;

    static TerminalControl * _instance;
};

} // namespace ventty
