#pragma once

#include "Color.h"

namespace ventty
{
struct Cell
{
    char32_t ch = U' ';
    Style style;
    uint8_t width = 1; // 1 for normal, 2 for wide chars (CJK)

    bool operator==(Cell const & other) const
    {
        return ch == other.ch && style == other.style && width == other.width;
    }

    bool operator!=(Cell const & other) const
    {
        return !(*this == other);
    }
};
} // namespace ventty
