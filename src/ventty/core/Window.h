// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/core/Cell.hpp>
#include <ventty/core/Utf8.h>

#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
/// Virtual window — a rectangular region of cells that can be
/// positioned anywhere on the terminal screen.
class Window
{
public:
    /// Construct a window with position and size
    Window(int x, int y, int w, int h);

    // -- drawing --

    /// Clear entire window with default style
    void clear();

    /// Clear entire window with specified background color
    void clear(Color bg);

    /// Clear entire window with specified style
    void clear(Style const & style);

    /// Draw a character at the given position with style
    void putChar(int x, int y, char32_t cp, Style const & style);

    /// Draw a character at the given position with fg/bg/attr
    void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None);

    /// Draw text at the given position with style
    void drawText(int x, int y, std::string_view text, Style const & style);

    /// Draw text at the given position with fg/bg/attr
    void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None);

    /// Fill a region with a character and style
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style);

    /// Fill a region with a character and fg/bg colors
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg);

    /// Print text with automatic line wrapping + scrollback
    void print(std::string_view text, Style const & style);
    void print(std::string_view text, Color fg, Color bg);

    /// Draw a box border around the window (single or double line)
    void drawBox(Style const & style, bool doubleLine = false);

    /// Draw a box border around the window with fg/bg colors
    void drawBox(Color fg, Color bg, bool doubleLine = false);

    /// Draw a box at arbitrary position (single or double line)
    void drawBox(int x, int y, int w, int h, Style const & style, bool doubleLine = false);

    /// Draw a horizontal line
    void drawHLine(int x, int y, int length, char32_t ch, Style const & style);

    /// Draw a vertical line
    void drawVLine(int x, int y, int length, char32_t ch, Style const & style);

    // -- scrollback --

    /// Enable scrollback buffer with max line count
    void enableScroll(int scrollbackLines);

    /// Scroll by the given number of lines (positive = up)
    void scroll(int lines);

    /// Return current scroll offset
    int scrollOffset() const;

    /// Return number of lines stored in scrollback buffer
    int scrollbackSize() const;

    // -- position / visibility --

    /// Return X coordinate (column)
    int x() const;

    /// Return Y coordinate (row)
    int y() const;

    /// Return window width
    int width() const;

    /// Return window height
    int height() const;

    /// Set window position
    void setPosition(int x, int y);

    /// Resize the window
    void resize(int w, int h);

    /// Return Z order (higher values render in front)
    int zOrder() const;

    /// Set Z order
    void setZOrder(int z);

    /// Return visibility state
    bool visible() const;

    /// Set visibility
    void setVisible(bool v);

    /// Check if there are pending changes
    bool isDirty() const;

    /// Mark as dirty
    void markDirty();

    /// Clear dirty flag
    void clearDirty();

    // -- cell access --

    /// Return reference to cell at given coordinates
    Cell & cellAt(int x, int y);

    /// Return const reference to cell at given coordinates
    Cell const & cellAt(int x, int y) const;

    /// Direct buffer access pointer for renderer
    Cell const * data() const;

    /// Return buffer size (cell count)
    int bufferSize() const;

    // -- cursor for print() --

    /// Return X coordinate of print() cursor
    int cursorX() const;

    /// Return Y coordinate of print() cursor
    int cursorY() const;

    /// Set print() cursor position
    void setCursor(int x, int y);

private:
    int _x, _y;           ///< Window position
    int _width, _height;  ///< Window size
    int _zOrder = 0;      ///< Z order
    bool _visible = true; ///< Visibility flag
    bool _dirty = true;   ///< Dirty flag

    std::vector<Cell> _buffer; ///< Cell buffer

    // scrollback
    std::vector<std::vector<Cell>> _scrollback; ///< Scrollback buffer
    int _scrollOffset = 0;                      ///< Current scroll offset
    int _maxScrollback = 0;                     ///< Max scrollback line count

    // cursor for print()
    int _cursorX = 0; ///< print() cursor X coordinate
    int _cursorY = 0; ///< print() cursor Y coordinate

    /// Scroll buffer up by one line
    void scrollBufferUp();
};
} // namespace ventty
