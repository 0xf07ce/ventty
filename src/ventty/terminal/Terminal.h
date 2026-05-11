// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/terminal/TerminalBase.h>
#include <ventty/core/Utf8.h>

#include <memory>
#include <string>
#include <vector>

namespace ventty
{
/// Real terminal backend — termios raw mode + ANSI escape sequences.
class Terminal : public TerminalBase
{
public:
    /// Constructor
    Terminal();

    /// Destructor (restores raw mode)
    ~Terminal() override;

    /// Enter raw mode, switch to alternate screen, enable mouse
    bool init();

    /// Shut down the terminal and release resources
    void shutdown() override;

    /// Poll for events
    bool pollEvent() override;

    /// Render the screen
    void render() override;

    /// Force a full screen redraw
    void forceRedraw() override;

    // -- drawing on the root screen --

    /// Clear the root screen with default style
    void clear() override;

    /// Clear the root screen with the specified background color
    void clear(Color bg) override;

    /// Put a character with default style
    void putChar(int x, int y, char32_t cp) override;

    /// Put a character with the given style
    void putChar(int x, int y, char32_t cp, Style const & style) override;

    /// Put a character with foreground, background, and attributes
    void putChar(int x, int y, char32_t cp, Color fg, Color bg,
        Attr attr = Attr::None) override;

    /// Draw text with default style
    void drawText(int x, int y, std::string_view text) override;

    /// Draw text with the given style
    void drawText(int x, int y, std::string_view text, Style const & style) override;

    /// Draw text with foreground, background, and attributes
    void drawText(int x, int y, std::string_view text, Color fg, Color bg,
        Attr attr = Attr::None) override;

    /// Fill a region with foreground and background colors
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) override;

    /// Fill a region with the given style
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style) override;

    /// Set the default style
    void setDefaultStyle(Style const & style) override;

    // -- window management --

    /// Create a window
    Window * createWindow(int x, int y, int w, int h) override;

    /// Destroy a window
    void destroyWindow(Window * win) override;

    // -- queries --

    /// Return the number of terminal columns
    int cols() const override;

    /// Return the number of terminal rows
    int rows() const override;

    // -- callbacks --

    /// Register a key event callback
    void onKey(KeyCallback cb) override;

    /// Register a mouse event callback
    void onMouse(MouseCallback cb) override;

    /// Switch between button-event-only (?1002h) and any-event (?1003h) reporting.
    void setMouseMode(MouseMode mode) override;

    /// Register a resize event callback
    void onResize(ResizeCallback cb) override;

    // -- direct cell access --

    /// Return a mutable reference to the cell at the given coordinates
    Cell & cellAt(int x, int y) override;

    /// Return a const reference to the cell at the given coordinates
    Cell const & cellAt(int x, int y) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl; ///< Implementation details (PIMPL)

    int _cols = 0; ///< Terminal column count
    int _rows = 0; ///< Terminal row count

    /// Friend declaration for emergency restoration by signal handler
    friend void signalHandler(int);

    Style _defaultStyle; ///< Default style

    std::vector<Cell> _root;   ///< Root (background) layer
    std::vector<Cell> _screen; ///< Composited frame
    std::vector<Cell> _prev;   ///< Previous frame (for diff comparison)
    bool _fullRedraw = true;   ///< Full redraw flag

    std::vector<std::unique_ptr<Window>> _windows; ///< Managed window list

    KeyCallback _keyCb;       ///< Key event callback
    MouseCallback _mouseCb;   ///< Mouse event callback
    ResizeCallback _resizeCb; ///< Resize callback

    /// Query terminal size
    void querySize();

    /// Composite windows onto _screen
    void composite();

    /// Emit ANSI escapes for changed cells
    void emitDiff();

    /// Emit a single cell as ANSI escape sequences
    void emitCell(int x, int y, Cell const & cell);

    /// Handle terminal resize
    void handleResize();

    /// Parse one input event from the front of `buf`. Returns the number of
    /// bytes consumed, or 0 if `buf` holds an incomplete sequence that needs
    /// more bytes. Dispatches via the registered key/mouse callbacks.
    size_t parseEventFromBuffer(unsigned char const * buf, size_t n);
};
} // namespace ventty
