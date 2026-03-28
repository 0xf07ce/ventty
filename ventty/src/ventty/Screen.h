#pragma once

#include "Cell.h"

#include <string>
#include <string_view>
#include <vector>

namespace ventty
{

class Screen
{
public:
    Screen();
    ~Screen();

    void resize(int width, int height);
    int width() const { return _width; }
    int height() const { return _height; }

    void clear();
    void clear(Style const & style);

    void setCursor(int x, int y);
    int cursorX() const { return _cursorX; }
    int cursorY() const { return _cursorY; }

    void setStyle(Style const & style);
    Style const & style() const { return _currentStyle; }

    void putChar(char32_t ch);
    void putString(std::string_view str);
    void putStringAt(int x, int y, std::string_view str);

    // Box drawing
    void drawBox(int x, int y, int w, int h);
    void drawBoxDouble(int x, int y, int w, int h);
    void drawHLine(int x, int y, int length, char32_t ch = U'─');
    void drawVLine(int x, int y, int length, char32_t ch = U'│');

    // Fill region
    void fillRect(int x, int y, int w, int h, char32_t ch = U' ');

    // Access cells
    Cell & at(int x, int y);
    Cell const & at(int x, int y) const;

    // Direct buffer access for renderers
    Cell const * data() const { return _buffer.data(); }
    int size() const { return _width * _height; }

private:
    int _width = 0;
    int _height = 0;
    int _cursorX = 0;
    int _cursorY = 0;
    Style _currentStyle;

    std::vector<Cell> _buffer;

    // Box drawing characters (single line)
    static constexpr char32_t BOX_H = U'─';
    static constexpr char32_t BOX_V = U'│';
    static constexpr char32_t BOX_TL = U'┌';
    static constexpr char32_t BOX_TR = U'┐';
    static constexpr char32_t BOX_BL = U'└';
    static constexpr char32_t BOX_BR = U'┘';
    static constexpr char32_t BOX_LT = U'├';
    static constexpr char32_t BOX_RT = U'┤';
    static constexpr char32_t BOX_TT = U'┬';
    static constexpr char32_t BOX_BT = U'┴';
    static constexpr char32_t BOX_X = U'┼';

    // Box drawing characters (double line)
    static constexpr char32_t BOX2_H = U'═';
    static constexpr char32_t BOX2_V = U'║';
    static constexpr char32_t BOX2_TL = U'╔';
    static constexpr char32_t BOX2_TR = U'╗';
    static constexpr char32_t BOX2_BL = U'╚';
    static constexpr char32_t BOX2_BR = U'╝';
};

} // namespace ventty
