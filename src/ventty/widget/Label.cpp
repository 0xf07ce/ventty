// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#include "Label.h"

#include <algorithm>

namespace ventty
{
Label::Label(std::string text)
    : _text(std::move(text))
{}

std::string const & Label::text() const
{
    return _text;
}

void Label::setText(std::string text)
{
    _text = std::move(text);
}

Style const & Label::style() const
{
    return _style;
}

void Label::setStyle(Style const & s)
{
    _style = s;
}

Align Label::align() const
{
    return _align;
}

void Label::setAlign(Align a)
{
    _align = a;
}

void Label::draw(Window & window)
{
    if (!_visible || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    // Calculate text position based on alignment
    int textLen = 0;
    for (size_t i = 0; i < _text.size();)
    {
        unsigned char c = _text[i];
        if ((c & 0x80) == 0)
        {
            ++textLen;
            ++i;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            ++textLen;
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            textLen += 2; // CJK characters are typically double-width
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            textLen += 2;
            i += 4;
        }
        else
        {
            ++textLen;
            ++i;
        }
    }

    int startX = _rect.x;
    switch (_align)
    {
    case Align::Left:
        startX = _rect.x;
        break;
    case Align::Center:
        startX = _rect.x + (_rect.width - textLen) / 2;
        break;
    case Align::Right:
        startX = _rect.x + _rect.width - textLen;
        break;
    }

    startX = std::max(startX, _rect.x);

    // Clear the line first
    for (int i = 0; i < _rect.width; ++i)
        window.putChar(_rect.x + i, _rect.y, U' ', _style);

    // Draw text
    window.drawText(startX, _rect.y, _text, _style);
}
} // namespace ventty
