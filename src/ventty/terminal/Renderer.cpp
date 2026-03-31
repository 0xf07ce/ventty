// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#include "Renderer.h"
#include <ventty/core/Utf8.h>

#include <cstdio>
#include <unistd.h>

namespace ventty
{
void Renderer::render(Window const & window)
{
    int width = window.width();
    int height = window.height();

    // Handle resize
    if (width != _prevWidth || height != _prevHeight)
    {
        _prevWidth = width;
        _prevHeight = height;
        _prev.resize(static_cast<size_t>(width * height));
        _fullRedraw = true;
    }

    Cell const * buffer = window.data();

    std::string output;
    output.reserve(width * height * 4);

    Style currentStyle;
    int lastX = -1;
    int lastY = -1;

    output += "\033[?25l";

    output += "\033[0m";

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Cell const & cell = buffer[y * width + x];
            Cell const & back = _prev[y * width + x];

            if (cell.width == 0)
            {
                continue;
            }

            if (!_fullRedraw && cell == back)
            {
                continue;
            }

            if (x != lastX + 1 || y != lastY)
            {
                char tmp[16];
                std::snprintf(tmp, sizeof(tmp), "\033[%d;%dH", y + 1, x + 1);
                output += tmp;
            }

            if (cell.style != currentStyle)
            {
                appendAnsiColor(output, cell.style, currentStyle);
                currentStyle = cell.style;
            }

            if (cell.ch == 0)
            {
                output += ' ';
            }
            else
            {
                encode(cell.ch, output);
            }

            lastX = x + cell.width - 1;
            lastY = y;
        }
    }

    output += "\033[0m";

    ::write(STDOUT_FILENO, output.data(), output.size());

    std::copy(buffer, buffer + width * height, _prev.begin());
    _fullRedraw = false;
}

void Renderer::invalidate()
{
    for (auto & cell : _prev)
    {
        cell.ch = 0xFFFFFFFF;
    }
    _fullRedraw = true;
}

void Renderer::appendAnsiColor(std::string & buf, Style const & style, Style const & prevStyle)
{
    bool needReset = false;

    if (style.attr != prevStyle.attr)
    {
        needReset = true;
    }

    if (needReset)
    {
        buf += "\033[0m";
    }

    if (hasAttr(style.attr, Attr::Bold))
    {
        buf += "\033[1m";
    }
    if (hasAttr(style.attr, Attr::Dim))
    {
        buf += "\033[2m";
    }
    if (hasAttr(style.attr, Attr::Italic))
    {
        buf += "\033[3m";
    }
    if (hasAttr(style.attr, Attr::Underline))
    {
        buf += "\033[4m";
    }
    if (hasAttr(style.attr, Attr::Blink))
    {
        buf += "\033[5m";
    }
    if (hasAttr(style.attr, Attr::Reverse))
    {
        buf += "\033[7m";
    }
    if (hasAttr(style.attr, Attr::Strike))
    {
        buf += "\033[9m";
    }

    if (needReset || style.fg != prevStyle.fg)
    {
        char tmp[24];
        std::snprintf(tmp, sizeof(tmp), "\033[38;2;%d;%d;%dm",
            style.fg.r, style.fg.g, style.fg.b);
        buf += tmp;
    }

    if (needReset || style.bg != prevStyle.bg)
    {
        char tmp[24];
        std::snprintf(tmp, sizeof(tmp), "\033[48;2;%d;%d;%dm",
            style.bg.r, style.bg.g, style.bg.b);
        buf += tmp;
    }
}
} // namespace ventty
