// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#pragma once

namespace ventty
{
/// Size representing width and height
struct Size
{
    int width = 0;  ///< Width
    int height = 0; ///< Height
};

/// Rectangular region with position and size
struct Rect
{
    int x = 0;      ///< X coordinate (column)
    int y = 0;      ///< Y coordinate (row)
    int width = 0;  ///< Width
    int height = 0; ///< Height

    /// Check whether the given point is inside this rect
    bool contains(int px, int py) const;
};
} // namespace ventty
