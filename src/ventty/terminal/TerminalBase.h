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
    /// Key type
    enum class Key : int
    {
        None = 0,  ///< No key
        Up,        ///< Up arrow
        Down,      ///< Down arrow
        Left,      ///< Left arrow
        Right,     ///< Right arrow
        Home,      ///< Home key
        End,       ///< End key
        PageUp,    ///< Page Up key
        PageDown,  ///< Page Down key
        Insert,    ///< Insert key
        Delete,    ///< Delete key
        Backspace, ///< Backspace key
        Tab,       ///< Tab key
        Enter,     ///< Enter key
        Escape,    ///< Escape key
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Char,      ///< Printable character — see `ch`
    };

    Key key = Key::None; ///< Key type pressed
    char32_t ch = 0;     ///< Unicode code point (when key == Key::Char)
    bool shift = false;  ///< Whether Shift is held
    bool ctrl = false;   ///< Whether Ctrl is held
    bool alt = false;    ///< Whether Alt is held
};

/// Mouse button event
struct MouseEvent
{
    /// Mouse button type
    enum class Button : uint8_t
    {
        None = 0,   ///< No button
        Left,       ///< Left button
        Middle,     ///< Middle button
        Right,      ///< Right button
        ScrollUp,   ///< Scroll up
        ScrollDown, ///< Scroll down
    };

    /// Mouse action type
    enum class Action : uint8_t
    {
        Press,   ///< Press
        Release, ///< Release
        Move,    ///< Move
    };

    Button button = Button::None;  ///< Mouse button pressed
    Action action = Action::Press; ///< Mouse action
    int x = 0;                     ///< Column coordinate (0-based)
    int y = 0;                     ///< Row coordinate (0-based)
};

/// Terminal resize event
struct ResizeEvent
{
    int cols = 0; ///< New column count
    int rows = 0; ///< New row count
};

/// Key event callback type
using KeyCallback = std::function<void(KeyEvent const &)>;

/// Mouse event callback type
using MouseCallback = std::function<void(MouseEvent const &)>;

/// Resize event callback type
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

    /// Shut down the terminal and release resources
    virtual void shutdown() = 0;

    /// Poll for events (returns true if an event was processed)
    virtual bool pollEvent() = 0;

    /// Render the screen
    virtual void render() = 0;

    /// Force a full screen redraw
    virtual void forceRedraw() = 0;

    // -- drawing on root screen --

    /// Clear the root screen with default style
    virtual void clear() = 0;

    /// Clear the root screen with the specified background color
    virtual void clear(Color bg) = 0;

    /// Put a character with default style
    virtual void putChar(int x, int y, char32_t cp) = 0;

    /// Put a character with the given style
    virtual void putChar(int x, int y, char32_t cp, Style const & style) = 0;

    /// Put a character with foreground, background, and attributes
    virtual void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None) = 0;

    /// Draw text with default style
    virtual void drawText(int x, int y, std::string_view text) = 0;

    /// Draw text with the given style
    virtual void drawText(int x, int y, std::string_view text, Style const & style) = 0;

    /// Draw text with foreground, background, and attributes
    virtual void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None) = 0;

    /// Fill a region with foreground and background colors
    virtual void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) = 0;

    /// Fill a region with the given style
    virtual void fill(int x, int y, int w, int h, char32_t cp, Style const & style) = 0;

    /// Set the default style
    virtual void setDefaultStyle(Style const & style) = 0;

    // -- window management --

    /// Create a window at the given position and size
    virtual Window * createWindow(int x, int y, int w, int h) = 0;

    /// Destroy a window
    virtual void destroyWindow(Window * win) = 0;

    // -- queries --

    /// Return the number of terminal columns
    virtual int cols() const = 0;

    /// Return the number of terminal rows
    virtual int rows() const = 0;

    /// Check whether the terminal is running
    bool isRunning() const { return _running; }

    /// Request termination
    void quit() { _running = false; }

    // -- callbacks --

    /// Register a key event callback
    virtual void onKey(KeyCallback cb) = 0;

    /// Register a mouse event callback
    virtual void onMouse(MouseCallback cb) = 0;

    /// Register a resize event callback
    virtual void onResize(ResizeCallback cb) = 0;

    // -- direct cell access --

    /// Return a mutable reference to the cell at the given coordinates
    virtual Cell & cellAt(int x, int y) = 0;

    /// Return a const reference to the cell at the given coordinates
    virtual Cell const & cellAt(int x, int y) const = 0;

protected:
    TerminalBase() = default;
    bool _running = false; ///< Running state flag
};
} // namespace ventty
