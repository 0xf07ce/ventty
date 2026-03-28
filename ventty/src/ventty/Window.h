#pragma once

#include "Cell.h"
#include "Utf8.h"

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
    Window(int x, int y, int w, int h);

    // -- drawing --

    void clear();
    void clear(Color bg);
    void putChar(int x, int y, char32_t cp, Style const & style);
    void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None);
    void drawText(int x, int y, std::string_view text, Style const & style);
    void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None);
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style);
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg);

    /// Print text with automatic line wrapping + scrollback
    void print(std::string_view text, Style const & style);
    void print(std::string_view text, Color fg, Color bg);

    /// Draw a box border (single or double line)
    void drawBox(Style const & style, bool doubleLine = false);
    void drawBox(Color fg, Color bg, bool doubleLine = false);

    // -- scrollback --

    void enableScroll(int scrollbackLines);
    void scroll(int lines); ///< positive = up
    int scrollOffset() const { return _scrollOffset; }
    int scrollbackSize() const { return static_cast<int>(_scrollback.size()); }

    // -- position / visibility --

    int x() const { return _x; }
    int y() const { return _y; }
    int width() const { return _width; }
    int height() const { return _height; }

    void setPosition(int x, int y)
    {
        _x = x;
        _y = y;
        _dirty = true;
    }

    void resize(int w, int h);

    int zOrder() const { return _zOrder; }
    void setZOrder(int z) { _zOrder = z; }

    bool visible() const { return _visible; }

    void setVisible(bool v)
    {
        _visible = v;
        _dirty = true;
    }

    bool isDirty() const { return _dirty; }
    void markDirty() { _dirty = true; }
    void clearDirty() { _dirty = false; }

    // -- cell access --

    Cell & cellAt(int x, int y);
    Cell const & cellAt(int x, int y) const;

    // -- cursor for print() --

    int cursorX() const { return _cursorX; }
    int cursorY() const { return _cursorY; }

    void setCursor(int x, int y)
    {
        _cursorX = x;
        _cursorY = y;
    }

private:
    int _x, _y;
    int _width, _height;
    int _zOrder = 0;
    bool _visible = true;
    bool _dirty = true;

    std::vector<Cell> _buffer;

    // scrollback
    std::vector<std::vector<Cell>> _scrollback;
    int _scrollOffset = 0;
    int _maxScrollback = 0;

    // cursor for print()
    int _cursorX = 0;
    int _cursorY = 0;

    void scrollBufferUp();
};
} // namespace ventty
