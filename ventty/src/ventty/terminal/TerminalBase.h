#pragma once

#include <ventty/core/Cell.hpp>
#include <ventty/core/Color.h>

#include <functional>
#include <string_view>

namespace ventty
{
class Window;

/// Key event from terminal input
struct KeyEvent
{
    enum class Key : int
    {
        None = 0,
        Up, Down, Left, Right,
        Home, End, PageUp, PageDown,
        Insert, Delete, Backspace, Tab,
        Enter, Escape,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Char, ///< Regular character — see `ch`
    };

    Key key = Key::None;
    char32_t ch = 0; ///< Unicode codepoint when key == Key::Char
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
};

/// Mouse button event
struct MouseEvent
{
    enum class Button : uint8_t { None = 0, Left, Middle, Right, ScrollUp, ScrollDown };

    enum class Action : uint8_t { Press, Release, Move };

    Button button = Button::None;
    Action action = Action::Press;
    int x = 0; ///< Column (0-based)
    int y = 0; ///< Row (0-based)
};

/// Terminal resize event
struct ResizeEvent
{
    int cols = 0;
    int rows = 0;
};

using KeyCallback = std::function<void(KeyEvent const &)>;
using MouseCallback = std::function<void(MouseEvent const &)>;
using ResizeCallback = std::function<void(ResizeEvent const &)>;

/// Abstract terminal interface — backend-agnostic.
/// Concrete implementations: AnsiTerminal (ANSI/VT), gfx::GfxTerminal (SDL3).
class TerminalBase
{
public:
    virtual ~TerminalBase() = default;

    TerminalBase(TerminalBase const &) = delete;
    TerminalBase & operator=(TerminalBase const &) = delete;

    // -- lifecycle --

    virtual void shutdown() = 0;
    virtual bool pollEvent() = 0;
    virtual void render() = 0;
    virtual void forceRedraw() = 0;

    // -- drawing on root screen --

    virtual void clear() = 0;
    virtual void clear(Color bg) = 0;

    virtual void putChar(int x, int y, char32_t cp) = 0;
    virtual void putChar(int x, int y, char32_t cp, Style const & style) = 0;
    virtual void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None) = 0;

    virtual void drawText(int x, int y, std::string_view text) = 0;
    virtual void drawText(int x, int y, std::string_view text, Style const & style) = 0;
    virtual void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None) = 0;

    virtual void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) = 0;
    virtual void fill(int x, int y, int w, int h, char32_t cp, Style const & style) = 0;

    virtual void setDefaultStyle(Style const & style) = 0;

    // -- window management --

    virtual Window * createWindow(int x, int y, int w, int h) = 0;
    virtual void destroyWindow(Window * win) = 0;

    // -- queries --

    virtual int cols() const = 0;
    virtual int rows() const = 0;
    bool isRunning() const { return _running; }
    void quit() { _running = false; }

    // -- callbacks --

    virtual void onKey(KeyCallback cb) = 0;
    virtual void onMouse(MouseCallback cb) = 0;
    virtual void onResize(ResizeCallback cb) = 0;

    // -- direct cell access --

    virtual Cell & cellAt(int x, int y) = 0;
    virtual Cell const & cellAt(int x, int y) const = 0;

protected:
    TerminalBase() = default;
    bool _running = false;
};

} // namespace ventty
