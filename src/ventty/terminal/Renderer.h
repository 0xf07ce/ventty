// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/core/Cell.hpp>
#include <ventty/core/Window.h>

#include <string>
#include <vector>

namespace ventty
{
/// Renders a Screen to the terminal using diff-based ANSI escape sequences.
class Renderer
{
public:
    /// Default constructor
    Renderer() = default;

    /// Destructor
    ~Renderer() = default;

    /// Render screen contents to stdout (diff-based)
    void render(Window const & window);

    /// Force a full redraw on the next render() call
    void invalidate();

private:
    /// Append ANSI color escape sequences to the buffer
    void appendAnsiColor(std::string & buf, Style const & style, Style const & prevStyle);

    std::vector<Cell> _prev; ///< Previous frame buffer
    int _prevWidth = 0;      ///< Previous frame width
    int _prevHeight = 0;     ///< Previous frame height
    bool _fullRedraw = true; ///< Full redraw flag
};
} // namespace ventty
