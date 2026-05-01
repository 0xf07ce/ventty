// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "Terminal.h"
#include <ventty/core/Window.h>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <exception>

namespace ventty
{
// -- ANSI escape helpers --

namespace ansi
{
static void write(std::string const & s)
{
    ::write(STDOUT_FILENO, s.data(), s.size());
}

static void moveTo(int x, int y)
{
    char buf[32];
    auto n = std::snprintf(buf, sizeof(buf), "\033[%d;%dH", y + 1, x + 1);
    ::write(STDOUT_FILENO, buf, static_cast<size_t>(n));
}

static void setFg(Color c)
{
    char buf[32];
    auto n = std::snprintf(buf, sizeof(buf), "\033[38;2;%u;%u;%um", c.r, c.g, c.b);
    ::write(STDOUT_FILENO, buf, static_cast<size_t>(n));
}

static void setBg(Color c)
{
    char buf[32];
    auto n = std::snprintf(buf, sizeof(buf), "\033[48;2;%u;%u;%um", c.r, c.g, c.b);
    ::write(STDOUT_FILENO, buf, static_cast<size_t>(n));
}

static void setAttr(Attr attr)
{
    if (hasAttr(attr, Attr::Bold)) ansi::write("\033[1m");
    if (hasAttr(attr, Attr::Dim)) ansi::write("\033[2m");
    if (hasAttr(attr, Attr::Italic)) ansi::write("\033[3m");
    if (hasAttr(attr, Attr::Underline)) ansi::write("\033[4m");
    if (hasAttr(attr, Attr::Blink)) ansi::write("\033[5m");
    if (hasAttr(attr, Attr::Reverse)) ansi::write("\033[7m");
    if (hasAttr(attr, Attr::Strike)) ansi::write("\033[9m");
}

static void reset()
{
    ansi::write("\033[0m");
}

static void hideCursor() { ansi::write("\033[?25l"); }
static void showCursor() { ansi::write("\033[?25h"); }
static void altScreen() { ansi::write("\033[?1049h"); }
static void mainScreen() { ansi::write("\033[?1049l"); }
static void enableMouse() { ansi::write("\033[?1003h\033[?1006h"); }

static void disableMouse()
{
    ansi::write("\033[?1003l"
        "\033[?1002l"
        "\033[?1000l"
        "\033[?1006l"
        "\033[?1015l");
}

// xterm modifyOtherKeys=1 — terminals that support it will report key
// combinations like Shift+Enter / Ctrl+Enter using the form
//   CSI 27 ; <modifier> ; <keycode> ~
// instead of collapsing them to a bare control byte. Mode 1 only activates
// for "non-trivial" combinations, so plain typing is unaffected.
static void enableModifyOtherKeys() { ansi::write("\033[>4;1m"); }
static void disableModifyOtherKeys() { ansi::write("\033[>4;0m"); }

static void clearScreen() { ansi::write("\033[2J\033[H"); }
} // namespace ansi

// -- Impl (termios state) --

struct Terminal::Impl
{
    struct termios origTermios {};
    bool rawMode = false;
};

static volatile sig_atomic_t gResizeFlag = 0;
static volatile sig_atomic_t gTermFlag = 0;

static Terminal * gActiveTerminal = nullptr;

static void emergencyRestore()
{
    char const MOUSE_OFF[] =
        "\033[?1003l"
        "\033[?1002l"
        "\033[?1000l"
        "\033[?1006l"
        "\033[?1015l";
    ::write(STDOUT_FILENO, MOUSE_OFF, sizeof(MOUSE_OFF) - 1);

    tcflush(STDIN_FILENO, TCIFLUSH);

    {
        unsigned char drain[256];
        for (int i = 0; i < 50; ++i)
        {
            auto r = ::read(STDIN_FILENO, drain, sizeof(drain));
            if (r <= 0)
                break;
        }
    }

    char const RESTORE[] =
        "\033[?25h"
        "\033[0m"
        "\033[?1049l"
        "\n";
    ::write(STDOUT_FILENO, RESTORE, sizeof(RESTORE) - 1);
}

static std::terminate_handler gPrevTerminate = nullptr;

static void terminateHandler()
{
    // Restore terminal state before aborting
    if (gActiveTerminal != nullptr)
        gActiveTerminal->shutdown();
    else
        emergencyRestore();

    if (gPrevTerminate)
        gPrevTerminate();
    else
        std::abort();
}

static void atexitHandler()
{
    if (gActiveTerminal != nullptr)
        gActiveTerminal->shutdown();
}

void signalHandler(int sig)
{
    if (sig == SIGWINCH)
    {
        gResizeFlag = 1;
        return;
    }

    gTermFlag = 1;

    if (gActiveTerminal != nullptr)
    {
        auto * impl = gActiveTerminal->_impl.get();
        if (impl != nullptr && impl->rawMode)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &impl->origTermios);
            impl->rawMode = false;
        }
        gActiveTerminal->_running = false;
    }
    emergencyRestore();

    struct sigaction sa {};
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, nullptr);
    raise(sig);
}

// -- AnsiTerminal --

Terminal::Terminal()
    : _impl(std::make_unique<Impl>())
{}

Terminal::~Terminal()
{
    if (_running)
        Terminal::shutdown();
}

bool Terminal::init()
{
    if (tcgetattr(STDIN_FILENO, &_impl->origTermios) < 0)
        return false;

    struct termios raw = _impl->origTermios;
    raw.c_iflag &= ~static_cast<tcflag_t>(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~static_cast<tcflag_t>(OPOST);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~static_cast<tcflag_t>(ECHO | ICANON | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0)
        return false;
    _impl->rawMode = true;

    gActiveTerminal = this;
    std::atexit(atexitHandler);
    gPrevTerminate = std::set_terminate(terminateHandler);

    struct sigaction sa {};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGWINCH, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGHUP, &sa, nullptr);

    ansi::altScreen();
    ansi::hideCursor();
    ansi::enableMouse();
    ansi::enableModifyOtherKeys();
    ansi::clearScreen();

    querySize();
    _root.resize(static_cast<size_t>(_cols * _rows));
    _screen.resize(static_cast<size_t>(_cols * _rows));
    _prev.resize(static_cast<size_t>(_cols * _rows));
    _fullRedraw = true;
    _running = true;

    return true;
}

void Terminal::shutdown()
{
    if (!_impl->rawMode)
        return;
    _running = false;

    ansi::disableModifyOtherKeys();
    ansi::disableMouse();
    tcflush(STDIN_FILENO, TCIFLUSH);

    {
        unsigned char drain[256];
        for (int i = 0; i < 50; ++i)
        {
            auto r = ::read(STDIN_FILENO, drain, sizeof(drain));
            if (r <= 0)
                break;
        }
    }

    ansi::showCursor();
    ansi::reset();
    ansi::mainScreen();

    if (_impl->rawMode)
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &_impl->origTermios);
        _impl->rawMode = false;
    }

    // Ensure cursor is on a fresh line so the shell prompt renders cleanly.
    ansi::write("\n");

    tcflush(STDIN_FILENO, TCIFLUSH);

    if (gActiveTerminal == this)
    {
        gActiveTerminal = nullptr;
        if (gPrevTerminate)
        {
            std::set_terminate(gPrevTerminate);
            gPrevTerminate = nullptr;
        }
    }
}

void Terminal::querySize()
{
    struct winsize ws {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
    {
        _cols = ws.ws_col;
        _rows = ws.ws_row;
    }
    else
    {
        _cols = 80;
        _rows = 24;
    }
}

void Terminal::handleResize()
{
    int oldCols = _cols;
    int oldRows = _rows;
    querySize();

    if (_cols != oldCols || _rows != oldRows)
    {
        _root.resize(static_cast<size_t>(_cols * _rows));
        _screen.resize(static_cast<size_t>(_cols * _rows));
        _prev.resize(static_cast<size_t>(_cols * _rows));
        _fullRedraw = true;

        if (_resizeCb)
            _resizeCb(ResizeEvent { _cols, _rows });
    }
}

// -- Drawing --

void Terminal::clear()
{
    for (auto & c : _root)
    {
        c = Cell {};
        c.style = _defaultStyle;
    }
}

void Terminal::clear(Color bg)
{
    for (auto & c : _root)
    {
        c = Cell {};
        c.style.bg = bg;
    }
}

void Terminal::setDefaultStyle(Style const & style)
{
    _defaultStyle = style;
}

void Terminal::putChar(int x, int y, char32_t cp)
{
    putChar(x, y, cp, _defaultStyle);
}

void Terminal::putChar(int x, int y, char32_t cp, Style const & style)
{
    if (x < 0 || x >= _cols || y < 0 || y >= _rows)
        return;

    auto & cell = _root[static_cast<size_t>(y * _cols + x)];
    cell.ch = cp;
    cell.style = style;
    cell.width = 1;

    if (isFullwidth(cp) && x + 1 < _cols)
    {
        cell.width = 2;
        auto & next = _root[static_cast<size_t>(y * _cols + x + 1)];
        next.ch = U'\0';
        next.style = style;
        next.width = 0;
    }
}

void Terminal::putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr)
{
    putChar(x, y, cp, Style { fg, bg, attr });
}

void Terminal::drawText(int x, int y, std::string_view text)
{
    drawText(x, y, text, _defaultStyle);
}

void Terminal::drawText(int x, int y, std::string_view text, Style const & style)
{
    if (y < 0 || y >= _rows)
        return;

    auto cps = toCodepoints(text);
    int cx = x;
    for (auto cp : cps)
    {
        if (cx >= _cols)
            break;
        if (cx >= 0)
            putChar(cx, y, cp, style);
        cx += displayWidth(cp);
    }
}

void Terminal::drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr)
{
    drawText(x, y, text, Style { fg, bg, attr });
}

void Terminal::fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg)
{
    fill(x, y, w, h, cp, Style { fg, bg });
}

void Terminal::fill(int x, int y, int w, int h, char32_t cp, Style const & style)
{
    int x0 = std::max(0, x);
    int y0 = std::max(0, y);
    int x1 = std::min(_cols, x + w);
    int y1 = std::min(_rows, y + h);

    for (int iy = y0; iy < y1; ++iy)
        for (int ix = x0; ix < x1; ++ix)
        {
            auto & cell = _root[static_cast<size_t>(iy * _cols + ix)];
            cell.ch = cp;
            cell.style = style;
            cell.width = 1;
        }
}

Cell & Terminal::cellAt(int x, int y)
{
    return _root[static_cast<size_t>(y * _cols + x)];
}

Cell const & Terminal::cellAt(int x, int y) const
{
    return _root[static_cast<size_t>(y * _cols + x)];
}

void Terminal::forceRedraw()
{
    _fullRedraw = true;
}

// -- Window management --

Window * Terminal::createWindow(int x, int y, int w, int h)
{
    _windows.push_back(std::make_unique<Window>(x, y, w, h));
    return _windows.back().get();
}

void Terminal::destroyWindow(Window * win)
{
    std::erase_if(_windows, [win](auto const & p) { return p.get() == win; });
}

int Terminal::cols() const { return _cols; }
int Terminal::rows() const { return _rows; }

// -- Composite --

void Terminal::composite()
{
    _screen = _root;

    std::ranges::sort(_windows, [](auto const & a, auto const & b)
    {
        return a->zOrder() < b->zOrder();
    });

    for (auto const & win : _windows)
    {
        if (!win->visible())
            continue;

        int wx = win->x();
        int wy = win->y();
        int ww = win->width();
        int wh = win->height();

        for (int ly = 0; ly < wh; ++ly)
        {
            int sy = wy + ly;
            if (sy < 0 || sy >= _rows)
                continue;

            for (int lx = 0; lx < ww; ++lx)
            {
                int sx = wx + lx;
                if (sx < 0 || sx >= _cols)
                    continue;

                _screen[static_cast<size_t>(sy * _cols + sx)] = win->cellAt(lx, ly);
            }
        }
    }
}

// -- Rendering (diff-based ANSI output) --

void Terminal::emitCell(int x, int y, Cell const & cell)
{
    ansi::moveTo(x, y);
    ansi::reset();

    if (cell.style.attr != Attr::None)
        ansi::setAttr(cell.style.attr);

    Color fg = cell.style.fg;
    Color bg = cell.style.bg;
    if (hasAttr(cell.style.attr, Attr::Reverse))
        std::swap(fg, bg);

    ansi::setFg(fg);
    ansi::setBg(bg);

    if (cell.ch == U'\0')
        return; // continuation cell for fullwidth

    std::string buf;
    encode(cell.ch, buf);
    ::write(STDOUT_FILENO, buf.data(), buf.size());
}

void Terminal::emitDiff()
{
    for (int y = 0; y < _rows; ++y)
    {
        for (int x = 0; x < _cols; ++x)
        {
            auto idx = static_cast<size_t>(y * _cols + x);
            if (_fullRedraw || _screen[idx] != _prev[idx])
                emitCell(x, y, _screen[idx]);
        }
    }

    _prev = _screen;
    _fullRedraw = false;
}

void Terminal::render()
{
    if (gResizeFlag)
    {
        gResizeFlag = 0;
        handleResize();
    }

    composite();
    emitDiff();
}

// -- Input Parsing --

bool Terminal::pollEvent()
{
    if (gTermFlag)
    {
        _running = false;
        return false;
    }

    if (gResizeFlag)
    {
        gResizeFlag = 0;
        handleResize();
    }

    unsigned char buf[32];
    auto n = ::read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0)
        return false;

    // Bare ESC: a lone 0x1b byte with no follow-up (n == 1).
    // Without this, the control-char handler below would misclassify it as Ctrl+{.
    if (buf[0] == 0x1b && n == 1)
    {
        KeyEvent ev;
        ev.key = KeyEvent::Key::Escape;
        if (_keyCb)
            _keyCb(ev);
        return true;
    }

    // ESC sequence?
    if (buf[0] == 0x1b && n > 1)
    {
        // CSI sequence: ESC [ ...
        if (buf[1] == '[')
        {
            // SGR mouse: ESC [ < Cb ; Cx ; Cy M/m
            if (n > 2 && buf[2] == '<')
            {
                int cb = 0, cx = 0, cy = 0;
                char trail = 0;
                int pos = 3;
                while (pos < n && buf[pos] != ';')
                    cb = cb * 10 + (buf[pos++] - '0');
                pos++;
                while (pos < n && buf[pos] != ';')
                    cx = cx * 10 + (buf[pos++] - '0');
                pos++;
                while (pos < n && buf[pos] != 'M' && buf[pos] != 'm')
                    cy = cy * 10 + (buf[pos++] - '0');
                if (pos < n)
                    trail = static_cast<char>(buf[pos]);

                MouseEvent ev;
                ev.x = cx - 1;
                ev.y = cy - 1;
                ev.action = (trail == 'm')
                    ? MouseEvent::Action::Release
                    : MouseEvent::Action::Press;

                int btn = cb & 0x03;
                bool motion = (cb & 0x20) != 0;
                bool scroll = (cb & 0x40) != 0;

                if (motion)
                    ev.action = MouseEvent::Action::Move;
                else if (scroll)
                    ev.button = (btn == 0)
                        ? MouseEvent::Button::ScrollUp
                        : MouseEvent::Button::ScrollDown;
                else if (btn == 0) ev.button = MouseEvent::Button::Left;
                else if (btn == 1) ev.button = MouseEvent::Button::Middle;
                else if (btn == 2) ev.button = MouseEvent::Button::Right;

                if (_mouseCb)
                    _mouseCb(ev);
                return true;
            }

            // Generic CSI: ESC [ <param> (; <param>)* <final>
            // Captures up to 4 numeric params; param[1] (when present) is the
            // standard xterm modifier byte (1 + bitmask of shift/alt/ctrl).
            int params[4] = { 0, 0, 0, 0 };
            int pCount = 0;
            int pos = 2;
            bool seenDigit = false;
            while (pos < n)
            {
                unsigned char b = buf[pos];
                if (b >= '0' && b <= '9')
                {
                    if (pCount < 4) params[pCount] = params[pCount] * 10 + (b - '0');
                    seenDigit = true;
                    ++pos;
                }
                else if (b == ';')
                {
                    if (pCount < 3) ++pCount;
                    seenDigit = false;
                    ++pos;
                }
                else
                {
                    break;
                }
            }
            if (seenDigit && pCount < 4) ++pCount;
            if (pos >= n) return true;
            char final = static_cast<char>(buf[pos]);

            KeyEvent ev;
            if (pCount >= 2)
            {
                int mod = params[1] - 1;
                if (mod >= 0)
                {
                    ev.shift = (mod & 1) != 0;
                    ev.alt   = (mod & 2) != 0;
                    ev.ctrl  = (mod & 4) != 0;
                }
            }

            switch (final)
            {
            case 'A': ev.key = KeyEvent::Key::Up;    break;
            case 'B': ev.key = KeyEvent::Key::Down;  break;
            case 'C': ev.key = KeyEvent::Key::Right; break;
            case 'D': ev.key = KeyEvent::Key::Left;  break;
            case 'H': ev.key = KeyEvent::Key::Home;  break;
            case 'F': ev.key = KeyEvent::Key::End;   break;
            case 'Z': ev.key = KeyEvent::Key::Tab;   ev.shift = true; break;
            case '~':
                // xterm modifyOtherKeys form: CSI 27 ; <mod> ; <key> ~
                if (params[0] == 27 && pCount >= 3)
                {
                    char32_t k = static_cast<char32_t>(params[2]);
                    switch (k)
                    {
                    case 13: ev.key = KeyEvent::Key::Enter;     break;
                    case 9:  ev.key = KeyEvent::Key::Tab;       break;
                    case 8:
                    case 127: ev.key = KeyEvent::Key::Backspace; break;
                    case 27: ev.key = KeyEvent::Key::Escape;    break;
                    default:
                        ev.key = KeyEvent::Key::Char;
                        ev.ch  = k;
                        break;
                    }
                }
                else
                {
                    switch (params[0])
                    {
                    case 2:  ev.key = KeyEvent::Key::Insert;   break;
                    case 3:  ev.key = KeyEvent::Key::Delete;   break;
                    case 5:  ev.key = KeyEvent::Key::PageUp;   break;
                    case 6:  ev.key = KeyEvent::Key::PageDown; break;
                    case 15: ev.key = KeyEvent::Key::F5;       break;
                    case 17: ev.key = KeyEvent::Key::F6;       break;
                    case 18: ev.key = KeyEvent::Key::F7;       break;
                    case 19: ev.key = KeyEvent::Key::F8;       break;
                    case 20: ev.key = KeyEvent::Key::F9;       break;
                    case 21: ev.key = KeyEvent::Key::F10;      break;
                    case 23: ev.key = KeyEvent::Key::F11;      break;
                    case 24: ev.key = KeyEvent::Key::F12;      break;
                    default: return true;
                    }
                }
                break;
            default:
                return true;
            }
            if (_keyCb)
                _keyCb(ev);
            return true;
        }

        // ESC O sequences (F1-F4)
        if (buf[1] == 'O' && n >= 3)
        {
            KeyEvent ev;
            switch (buf[2])
            {
            case 'P': ev.key = KeyEvent::Key::F1;
                break;
            case 'Q': ev.key = KeyEvent::Key::F2;
                break;
            case 'R': ev.key = KeyEvent::Key::F3;
                break;
            case 'S': ev.key = KeyEvent::Key::F4;
                break;
            default: return true;
            }
            if (_keyCb)
                _keyCb(ev);
            return true;
        }

        // Alt+key: ESC + printable
        if (n == 2 && buf[1] >= 0x20)
        {
            KeyEvent ev;
            ev.key = KeyEvent::Key::Char;
            ev.ch = buf[1];
            ev.alt = true;
            if (_keyCb)
                _keyCb(ev);
            return true;
        }

        // Bare ESC
        KeyEvent ev;
        ev.key = KeyEvent::Key::Escape;
        if (_keyCb)
            _keyCb(ev);
        return true;
    }

    // Control characters (0x7f is DEL — what most terminals send for Backspace)
    if (buf[0] < 0x20 || buf[0] == 0x7f)
    {
        KeyEvent ev;
        switch (buf[0])
        {
        case 0x0d: ev.key = KeyEvent::Key::Enter;
            break;
        case 0x09: ev.key = KeyEvent::Key::Tab;
            break;
        case 0x08:
        case 0x7f: ev.key = KeyEvent::Key::Backspace;
            break;
        default:
            ev.key = KeyEvent::Key::Char;
            ev.ch = static_cast<char32_t>(buf[0] + 0x60);
            ev.ctrl = true;
            break;
        }
        if (_keyCb)
            _keyCb(ev);
        return true;
    }

    // UTF-8 character(s)
    std::string_view sv(reinterpret_cast<char const *>(buf), static_cast<size_t>(n));
    size_t pos = 0;
    while (pos < sv.size())
    {
        auto cp = decode(sv, pos);
        if (cp == U'\0')
            break;
        KeyEvent ev;
        ev.key = KeyEvent::Key::Char;
        ev.ch = cp;
        if (_keyCb)
            _keyCb(ev);
    }
    return true;
}

void Terminal::onKey(KeyCallback cb) { _keyCb = std::move(cb); }
void Terminal::onMouse(MouseCallback cb) { _mouseCb = std::move(cb); }
void Terminal::onResize(ResizeCallback cb) { _resizeCb = std::move(cb); }
} // namespace ventty
