#include "Window.h"

#include <algorithm>

namespace ventty
{
Window::Window(int x, int y, int w, int h)
    : _x(x), _y(y), _width(w), _height(h)
{
    _buffer.resize(static_cast<size_t>(_width * _height));
}

void Window::clear()
{
    for (auto & c : _buffer)
        c = Cell {};
    _dirty = true;
    _cursorX = 0;
    _cursorY = 0;
}

void Window::clear(Color bg)
{
    for (auto & c : _buffer)
    {
        c = Cell {};
        c.style.bg = bg;
    }
    _dirty = true;
    _cursorX = 0;
    _cursorY = 0;
}

void Window::putChar(int x, int y, char32_t cp, Style const & style)
{
    if (x < 0 || x >= _width || y < 0 || y >= _height)
        return;

    auto & cell = _buffer[static_cast<size_t>(y * _width + x)];
    cell.ch = cp;
    cell.style = style;
    cell.width = 1;

    if (isFullwidth(cp) && x + 1 < _width)
    {
        cell.width = 2;
        auto & next = _buffer[static_cast<size_t>(y * _width + x + 1)];
        next.ch = U'\0';
        next.style = style;
        next.width = 0;
    }

    _dirty = true;
}

void Window::putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr)
{
    putChar(x, y, cp, Style { fg, bg, attr });
}

void Window::drawText(int x, int y, std::string_view text, Style const & style)
{
    if (y < 0 || y >= _height)
        return;

    auto cps = toCodepoints(text);
    int cx = x;
    for (auto cp : cps)
    {
        if (cx >= _width)
            break;
        if (cx >= 0)
            putChar(cx, y, cp, style);
        cx += displayWidth(cp);
    }
}

void Window::drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr)
{
    drawText(x, y, text, Style { fg, bg, attr });
}

void Window::fill(int x, int y, int w, int h, char32_t cp, Style const & style)
{
    int x0 = std::max(0, x);
    int y0 = std::max(0, y);
    int x1 = std::min(_width, x + w);
    int y1 = std::min(_height, y + h);

    for (int iy = y0; iy < y1; ++iy)
        for (int ix = x0; ix < x1; ++ix)
        {
            auto & cell = _buffer[static_cast<size_t>(iy * _width + ix)];
            cell.ch = cp;
            cell.style = style;
            cell.width = 1;
        }
    _dirty = true;
}

void Window::fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg)
{
    fill(x, y, w, h, cp, Style { fg, bg });
}

void Window::print(std::string_view text, Style const & style)
{
    auto cps = toCodepoints(text);
    for (auto cp : cps)
    {
        if (cp == U'\n')
        {
            _cursorX = 0;
            _cursorY++;
            if (_cursorY >= _height)
            {
                scrollBufferUp();
                _cursorY = _height - 1;
            }
            continue;
        }

        int w = displayWidth(cp);
        if (_cursorX + w > _width)
        {
            _cursorX = 0;
            _cursorY++;
            if (_cursorY >= _height)
            {
                scrollBufferUp();
                _cursorY = _height - 1;
            }
        }

        putChar(_cursorX, _cursorY, cp, style);
        _cursorX += w;
    }
}

void Window::print(std::string_view text, Color fg, Color bg)
{
    print(text, Style { fg, bg });
}

void Window::drawBox(Style const & style, bool doubleLine)
{
    char32_t tl, tr, bl, br, h, v;
    if (doubleLine)
    {
        tl = U'╔';
        tr = U'╗';
        bl = U'╚';
        br = U'╝';
        h = U'═';
        v = U'║';
    }
    else
    {
        tl = U'┌';
        tr = U'┐';
        bl = U'└';
        br = U'┘';
        h = U'─';
        v = U'│';
    }

    putChar(0, 0, tl, style);
    putChar(_width - 1, 0, tr, style);
    putChar(0, _height - 1, bl, style);
    putChar(_width - 1, _height - 1, br, style);

    for (int x = 1; x < _width - 1; ++x)
    {
        putChar(x, 0, h, style);
        putChar(x, _height - 1, h, style);
    }
    for (int y = 1; y < _height - 1; ++y)
    {
        putChar(0, y, v, style);
        putChar(_width - 1, y, v, style);
    }
}

void Window::drawBox(Color fg, Color bg, bool doubleLine)
{
    drawBox(Style { fg, bg }, doubleLine);
}

void Window::enableScroll(int scrollbackLines)
{
    _maxScrollback = scrollbackLines;
    _scrollback.clear();
    _scrollOffset = 0;
}

void Window::scroll(int lines)
{
    int maxOffset = static_cast<int>(_scrollback.size());
    _scrollOffset = std::clamp(_scrollOffset + lines, 0, maxOffset);
    _dirty = true;
}

void Window::resize(int w, int h)
{
    _width = w;
    _height = h;
    _buffer.resize(static_cast<size_t>(w * h));
    clear();
}

Cell & Window::cellAt(int x, int y)
{
    return _buffer[static_cast<size_t>(y * _width + x)];
}

Cell const & Window::cellAt(int x, int y) const
{
    return _buffer[static_cast<size_t>(y * _width + x)];
}

void Window::scrollBufferUp()
{
    if (_maxScrollback > 0)
    {
        std::vector<Cell> row(_buffer.begin(),
            _buffer.begin() + _width);
        _scrollback.push_back(std::move(row));
        if (static_cast<int>(_scrollback.size()) > _maxScrollback)
            _scrollback.erase(_scrollback.begin());
    }

    for (int y = 0; y < _height - 1; ++y)
    {
        auto dst = static_cast<size_t>(y * _width);
        auto src = static_cast<size_t>((y + 1) * _width);
        std::copy(_buffer.begin() + static_cast<long>(src),
            _buffer.begin() + static_cast<long>(src + _width),
            _buffer.begin() + static_cast<long>(dst));
    }

    auto last = static_cast<size_t>((_height - 1) * _width);
    for (int x = 0; x < _width; ++x)
        _buffer[last + static_cast<size_t>(x)] = Cell {};

    _dirty = true;
}

void Window::drawBox(int x, int y, int w, int h, Style const & style, bool doubleLine)
{
    if (w < 2 || h < 2)
        return;

    char32_t tl, tr, bl, br, hz, vt;
    if (doubleLine)
    {
        tl = U'╔';
        tr = U'╗';
        bl = U'╚';
        br = U'╝';
        hz = U'═';
        vt = U'║';
    }
    else
    {
        tl = U'┌';
        tr = U'┐';
        bl = U'└';
        br = U'┘';
        hz = U'─';
        vt = U'│';
    }

    putChar(x, y, tl, style);
    putChar(x + w - 1, y, tr, style);
    putChar(x, y + h - 1, bl, style);
    putChar(x + w - 1, y + h - 1, br, style);

    drawHLine(x + 1, y, w - 2, hz, style);
    drawHLine(x + 1, y + h - 1, w - 2, hz, style);
    drawVLine(x, y + 1, h - 2, vt, style);
    drawVLine(x + w - 1, y + 1, h - 2, vt, style);
}

void Window::drawHLine(int x, int y, int length, char32_t ch, Style const & style)
{
    for (int i = 0; i < length; ++i)
    {
        putChar(x + i, y, ch, style);
    }
}

void Window::drawVLine(int x, int y, int length, char32_t ch, Style const & style)
{
    for (int i = 0; i < length; ++i)
    {
        putChar(x, y + i, ch, style);
    }
}

void Window::clear(Style const & style)
{
    for (auto & c : _buffer)
    {
        c = Cell {};
        c.style = style;
    }
    _dirty = true;
    _cursorX = 0;
    _cursorY = 0;
}

Cell const * Window::data() const { return _buffer.data(); }
int Window::bufferSize() const { return _width * _height; }

int Window::scrollOffset() const { return _scrollOffset; }
int Window::scrollbackSize() const { return static_cast<int>(_scrollback.size()); }
int Window::x() const { return _x; }
int Window::y() const { return _y; }
int Window::width() const { return _width; }
int Window::height() const { return _height; }

void Window::setPosition(int x, int y)
{
    _x = x;
    _y = y;
    _dirty = true;
}

int Window::zOrder() const { return _zOrder; }
void Window::setZOrder(int z) { _zOrder = z; }
bool Window::visible() const { return _visible; }

void Window::setVisible(bool v)
{
    _visible = v;
    _dirty = true;
}

bool Window::isDirty() const { return _dirty; }
void Window::markDirty() { _dirty = true; }
void Window::clearDirty() { _dirty = false; }
int Window::cursorX() const { return _cursorX; }
int Window::cursorY() const { return _cursorY; }

void Window::setCursor(int x, int y)
{
    _cursorX = x;
    _cursorY = y;
}
} // namespace ventty
