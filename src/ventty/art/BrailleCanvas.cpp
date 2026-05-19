// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include <ventty/art/BrailleCanvas.h>

#include <ventty/core/Window.h>

#include <algorithm>
#include <cstdlib>

namespace ventty
{
namespace
{
// Sub-pixel (column, row) -> braille dot bit. Mirrors
// BraillePlot::setDot's OFFSETS so a BrailleCanvas cell and a
// BraillePlot character pack dots identically.
constexpr unsigned char kDotBit[2][4] = {
    {0x01, 0x02, 0x04, 0x40}, // left column,  rows 0..3
    {0x08, 0x10, 0x20, 0x80}, // right column, rows 0..3
};
} // namespace

BrailleCanvas::BrailleCanvas(int cols, int rows)
    : _cols(std::max(0, cols)),
      _rows(std::max(0, rows)),
      _cells(static_cast<std::size_t>(std::max(0, cols))
                 * static_cast<std::size_t>(std::max(0, rows)),
             0)
{
}

void BrailleCanvas::clear()
{
    std::fill(_cells.begin(), _cells.end(), static_cast<unsigned char>(0));
}

void BrailleCanvas::set(int sx, int sy)
{
    if (sx < 0 || sy < 0 || sx >= subWidth() || sy >= subHeight())
        return;
    _cells[static_cast<std::size_t>(sy / 4) * _cols + (sx / 2)] |=
        kDotBit[sx % 2][sy % 4];
}

void BrailleCanvas::unset(int sx, int sy)
{
    if (sx < 0 || sy < 0 || sx >= subWidth() || sy >= subHeight())
        return;
    _cells[static_cast<std::size_t>(sy / 4) * _cols + (sx / 2)] &=
        static_cast<unsigned char>(~kDotBit[sx % 2][sy % 4]);
}

bool BrailleCanvas::get(int sx, int sy) const
{
    if (sx < 0 || sy < 0 || sx >= subWidth() || sy >= subHeight())
        return false;
    return (_cells[static_cast<std::size_t>(sy / 4) * _cols + (sx / 2)]
            & kDotBit[sx % 2][sy % 4]) != 0;
}

void BrailleCanvas::line(int x0, int y0, int x1, int y1)
{
    // Standard integer Bresenham; set() clips per-point.
    int const dx = std::abs(x1 - x0);
    int const dy = -std::abs(y1 - y0);
    int const sx = x0 < x1 ? 1 : -1;
    int const sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;)
    {
        set(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        int const e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void BrailleCanvas::blit(Window & window, int x, int y, Style const & style,
                         bool drawEmpty) const
{
    for (int cy = 0; cy < _rows; ++cy)
        for (int cx = 0; cx < _cols; ++cx)
        {
            unsigned char const m =
                _cells[static_cast<std::size_t>(cy) * _cols + cx];
            if (m == 0 && !drawEmpty)
                continue;
            window.putChar(x + cx, y + cy,
                           static_cast<char32_t>(0x2800u + m), style);
        }
}

} // namespace ventty
