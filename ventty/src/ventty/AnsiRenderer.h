#pragma once

#include "Cell.h"
#include "Screen.h"

#include <string>
#include <vector>

namespace ventty
{

/// Renders a Screen to the terminal using diff-based ANSI escape sequences.
class AnsiRenderer
{
public:
    AnsiRenderer() = default;
    ~AnsiRenderer() = default;

    /// Render the screen contents to stdout (diff-based).
    void render(Screen const & screen);

    /// Force full redraw on next render().
    void invalidate();

private:
    void appendAnsiColor(std::string & buf, Style const & style, Style const & prevStyle);

    std::vector<Cell> _prev;
    int _prevWidth = 0;
    int _prevHeight = 0;
    bool _fullRedraw = true;
};

} // namespace ventty
