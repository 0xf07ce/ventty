#include "TerminalControl.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

namespace ventty
{

TerminalControl * TerminalControl::_instance = nullptr;

TerminalControl::TerminalControl()
{
    _instance = this;
    _originalTermios = new struct termios;
}

TerminalControl::~TerminalControl()
{
    if (_mouseEnabled)
    {
        disableMouse();
    }
    if (!_cursorVisible)
    {
        showCursor();
    }
    if (_alternateScreen)
    {
        disableAlternateScreen();
    }
    if (_rawMode)
    {
        exitRawMode();
    }
    delete _originalTermios;
    _instance = nullptr;
}

TerminalControl & TerminalControl::instance()
{
    if (!_instance)
    {
        throw std::runtime_error("TerminalControl not initialized");
    }
    return *_instance;
}

void TerminalControl::enterRawMode()
{
    if (_rawMode)
    {
        return;
    }

    if (tcgetattr(STDIN_FILENO, _originalTermios) == -1)
    {
        throw std::runtime_error("Failed to get terminal attributes");
    }

    struct termios raw = *_originalTermios;

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        throw std::runtime_error("Failed to set terminal attributes");
    }

    _rawMode = true;
    setupSignalHandlers();
}

void TerminalControl::exitRawMode()
{
    if (!_rawMode)
    {
        return;
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, _originalTermios);
    _rawMode = false;
}

bool TerminalControl::isRawMode() const
{
    return _rawMode;
}

void TerminalControl::enableAlternateScreen()
{
    if (_alternateScreen)
    {
        return;
    }
    write("\033[?1049h");
    flush();
    _alternateScreen = true;
}

void TerminalControl::disableAlternateScreen()
{
    if (!_alternateScreen)
    {
        return;
    }
    write("\033[?1049l");
    flush();
    _alternateScreen = false;
}

void TerminalControl::enableMouse()
{
    if (_mouseEnabled)
    {
        return;
    }
    write("\033[?1000h\033[?1006h");
    flush();
    _mouseEnabled = true;
}

void TerminalControl::disableMouse()
{
    if (!_mouseEnabled)
    {
        return;
    }
    write("\033[?1006l\033[?1000l");
    flush();
    _mouseEnabled = false;
}

void TerminalControl::showCursor()
{
    write("\033[?25h");
    flush();
    _cursorVisible = true;
}

void TerminalControl::hideCursor()
{
    write("\033[?25l");
    flush();
    _cursorVisible = false;
}

Size TerminalControl::size() const
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        return {80, 24};
    }
    return {ws.ws_col, ws.ws_row};
}

void TerminalControl::write(std::string_view data)
{
    ::write(STDOUT_FILENO, data.data(), data.size());
}

void TerminalControl::flush()
{
    std::fflush(stdout);
}

void TerminalControl::setResizeHandler(ResizeHandler handler)
{
    _resizeHandler = std::move(handler);
}

void TerminalControl::setupSignalHandlers()
{
    struct sigaction sa;
    sa.sa_handler = handleSigwinch;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGWINCH, &sa, nullptr);

    struct sigaction cleanup;
    cleanup.sa_handler = handleTermSignal;
    cleanup.sa_flags = 0;
    sigemptyset(&cleanup.sa_mask);
    sigaction(SIGINT, &cleanup, nullptr);
    sigaction(SIGTERM, &cleanup, nullptr);
    sigaction(SIGHUP, &cleanup, nullptr);
}

void TerminalControl::handleSigwinch(int)
{
    if (_instance && _instance->_resizeHandler)
    {
        _instance->_resizeHandler(_instance->size());
    }
}

void TerminalControl::handleTermSignal(int sig)
{
    if (_instance)
    {
        ::write(STDOUT_FILENO, "\033[?25h", 6);
        ::write(STDOUT_FILENO, "\033[?1049l", 8);

        if (_instance->_rawMode)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, _instance->_originalTermios);
        }
    }

    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, nullptr);
    raise(sig);
}

} // namespace ventty
