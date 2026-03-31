// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/core/Color.h>

namespace ventty
{
/// A cell representing one position on screen
struct Cell
{
    char32_t ch = U' '; ///< Unicode character (codepoint)
    Style style;        ///< Cell style (foreground, background, attributes)
    uint8_t width = 1;  ///< Display width (1: normal, 2: CJK full-width)

    /// Check equality of two cells
    bool operator==(Cell const & other) const
    {
        return ch == other.ch && style == other.style && width == other.width;
    }

    /// Check inequality of two cells
    bool operator!=(Cell const & other) const
    {
        return !(*this == other);
    }
};
} // namespace ventty
