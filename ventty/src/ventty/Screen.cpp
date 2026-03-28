#include "Screen.h"
#include "Utf8.h"

#include <algorithm>

namespace ventty
{

Screen::Screen()
{
}

Screen::~Screen()
{
}

void Screen::resize(int width, int height)
{
    if (width == _width && height == _height)
    {
        return;
    }

    _width = width;
    _height = height;
    _buffer.resize(width * height);

    clear();
}

void Screen::clear()
{
    clear(_currentStyle);
}

void Screen::clear(Style const & style)
{
    Cell cell;
    cell.ch = U' ';
    cell.style = style;
    cell.width = 1;

    std::fill(_buffer.begin(), _buffer.end(), cell);
    _cursorX = 0;
    _cursorY = 0;
}

void Screen::setCursor(int x, int y)
{
    _cursorX = std::clamp(x, 0, _width - 1);
    _cursorY = std::clamp(y, 0, _height - 1);
}

void Screen::setStyle(Style const & style)
{
    _currentStyle = style;
}

void Screen::putChar(char32_t ch)
{
    if (_cursorX >= _width || _cursorY >= _height)
    {
        return;
    }

    int w = utf8::isFullwidth(ch) ? 2 : 1;

    if (w == 2 && _cursorX + 1 >= _width)
    {
        return;
    }

    Cell & cell = at(_cursorX, _cursorY);
    cell.ch = ch;
    cell.style = _currentStyle;
    cell.width = w;

    if (w == 2 && _cursorX + 1 < _width)
    {
        Cell & next = at(_cursorX + 1, _cursorY);
        next.ch = 0;
        next.style = _currentStyle;
        next.width = 0;
    }

    _cursorX += w;
}

void Screen::putString(std::string_view str)
{
    size_t pos = 0;
    while (pos < str.size())
    {
        auto ch = utf8::decode(str, pos);
        if (ch == U'\0')
            break;
        putChar(ch);
    }
}

void Screen::putStringAt(int x, int y, std::string_view str)
{
    setCursor(x, y);
    putString(str);
}

void Screen::drawBox(int x, int y, int w, int h)
{
    if (w < 2 || h < 2)
    {
        return;
    }

    setCursor(x, y);
    putChar(BOX_TL);
    setCursor(x + w - 1, y);
    putChar(BOX_TR);
    setCursor(x, y + h - 1);
    putChar(BOX_BL);
    setCursor(x + w - 1, y + h - 1);
    putChar(BOX_BR);

    drawHLine(x + 1, y, w - 2, BOX_H);
    drawHLine(x + 1, y + h - 1, w - 2, BOX_H);

    drawVLine(x, y + 1, h - 2, BOX_V);
    drawVLine(x + w - 1, y + 1, h - 2, BOX_V);
}

void Screen::drawBoxDouble(int x, int y, int w, int h)
{
    if (w < 2 || h < 2)
    {
        return;
    }

    setCursor(x, y);
    putChar(BOX2_TL);
    setCursor(x + w - 1, y);
    putChar(BOX2_TR);
    setCursor(x, y + h - 1);
    putChar(BOX2_BL);
    setCursor(x + w - 1, y + h - 1);
    putChar(BOX2_BR);

    drawHLine(x + 1, y, w - 2, BOX2_H);
    drawHLine(x + 1, y + h - 1, w - 2, BOX2_H);

    drawVLine(x, y + 1, h - 2, BOX2_V);
    drawVLine(x + w - 1, y + 1, h - 2, BOX2_V);
}

void Screen::drawHLine(int x, int y, int length, char32_t ch)
{
    setCursor(x, y);
    for (int i = 0; i < length; ++i)
    {
        putChar(ch);
    }
}

void Screen::drawVLine(int x, int y, int length, char32_t ch)
{
    for (int i = 0; i < length; ++i)
    {
        setCursor(x, y + i);
        putChar(ch);
    }
}

void Screen::fillRect(int x, int y, int w, int h, char32_t ch)
{
    for (int row = y; row < y + h && row < _height; ++row)
    {
        setCursor(x, row);
        for (int col = x; col < x + w && col < _width; ++col)
        {
            putChar(ch);
        }
    }
}

Cell & Screen::at(int x, int y)
{
    static Cell empty;
    if (x < 0 || x >= _width || y < 0 || y >= _height)
    {
        return empty;
    }
    return _buffer[y * _width + x];
}

Cell const & Screen::at(int x, int y) const
{
    static Cell const empty;
    if (x < 0 || x >= _width || y < 0 || y >= _height)
    {
        return empty;
    }
    return _buffer[y * _width + x];
}

} // namespace ventty
