#pragma once

#include "Color.h"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace ventty::ascii
{

// -- Box Drawing --

struct BoxChars
{
    char32_t tl, tr, bl, br;   // corners
    char32_t h, v;             // horizontal, vertical
    char32_t lt, rt, tt, bt;   // T-junctions (left, right, top, bottom)
    char32_t cross;            // cross junction
};

constexpr BoxChars SINGLE_BOX = {
    U'┌', U'┐', U'└', U'┘',
    U'─', U'│',
    U'├', U'┤', U'┬', U'┴',
    U'┼'
};

constexpr BoxChars DOUBLE_BOX = {
    U'╔', U'╗', U'╚', U'╝',
    U'═', U'║',
    U'╠', U'╣', U'╦', U'╩',
    U'╬'
};

constexpr BoxChars ROUND_BOX = {
    U'╭', U'╮', U'╰', U'╯',
    U'─', U'│',
    U'├', U'┤', U'┬', U'┴',
    U'┼'
};

constexpr BoxChars HEAVY_BOX = {
    U'┏', U'┓', U'┗', U'┛',
    U'━', U'┃',
    U'┣', U'┫', U'┳', U'┻',
    U'╋'
};

// -- Spinner Frames --

constexpr std::array<std::string_view, 10> SPINNER_BRAILLE = {
    "\u280B", "\u2819", "\u2839", "\u2838",
    "\u283C", "\u2834", "\u2826", "\u2827",
    "\u2807", "\u280F"
};

constexpr std::array<std::string_view, 4> SPINNER_LINE = {
    "|", "/", "-", "\\"
};

constexpr std::array<std::string_view, 4> SPINNER_BLOCK = {
    "\u2596", "\u2598", "\u259D", "\u2597"
};

constexpr std::array<std::string_view, 8> SPINNER_MOON = {
    "\U0001F311", "\U0001F312", "\U0001F313", "\U0001F314",
    "\U0001F315", "\U0001F316", "\U0001F317", "\U0001F318"
};

constexpr std::array<std::string_view, 4> SPINNER_DOTS = {
    "   ", ".  ", ".. ", "..."
};

// -- Progress Bar --

struct ProgressStyle
{
    char32_t filled    = U'█';
    char32_t partial   = U'▓';
    char32_t empty     = U'░';
    char32_t leftCap   = U'[';
    char32_t rightCap  = U']';
    Color filledFg  = Colors::GREEN;
    Color emptyFg   = Colors::DARK_GRAY;
    Color capFg     = Colors::LIGHT_GRAY;
};

constexpr ProgressStyle PROGRESS_BLOCK = {};

constexpr ProgressStyle PROGRESS_SMOOTH = {
    U'━', U'╸', U'─',
    U'╺', U'╸',
    Colors::CYAN, Colors::DARK_GRAY, Colors::LIGHT_GRAY
};

constexpr ProgressStyle PROGRESS_HASH = {
    U'#', U'>', U'-',
    U'[', U']',
    Colors::GREEN, Colors::DARK_GRAY, Colors::LIGHT_GRAY
};

std::string progressBar(int width, float ratio, ProgressStyle const & style = PROGRESS_BLOCK);

// -- Horizontal Rule --

constexpr char32_t HR_THIN   = U'─';
constexpr char32_t HR_THICK  = U'━';
constexpr char32_t HR_DOUBLE = U'═';
constexpr char32_t HR_DOTTED = U'┄';
constexpr char32_t HR_DASHED = U'┅';

// -- Block Shade Characters --

constexpr char32_t SHADE_LIGHT  = U'░';
constexpr char32_t SHADE_MEDIUM = U'▒';
constexpr char32_t SHADE_DARK   = U'▓';
constexpr char32_t SHADE_FULL   = U'█';

/// Vertical bar graph characters (1/8 increments)
constexpr std::array<char32_t, 9> VBAR = {
    U' ', U'▁', U'▂', U'▃', U'▄', U'▅', U'▆', U'▇', U'█'
};

/// Horizontal bar graph characters (1/8 increments)
constexpr std::array<char32_t, 9> HBAR = {
    U' ', U'▏', U'▎', U'▍', U'▌', U'▋', U'▊', U'▉', U'█'
};

// -- Braille Plotting --

struct BraillePlot
{
    static constexpr char32_t fromDots(bool d[2][4])
    {
        char32_t cp = 0x2800;
        if (d[0][0]) cp |= 0x01;
        if (d[0][1]) cp |= 0x02;
        if (d[0][2]) cp |= 0x04;
        if (d[1][0]) cp |= 0x08;
        if (d[1][1]) cp |= 0x10;
        if (d[1][2]) cp |= 0x20;
        if (d[0][3]) cp |= 0x40;
        if (d[1][3]) cp |= 0x80;
        return cp;
    }

    static constexpr char32_t setDot(char32_t cp, int dx, int dy)
    {
        constexpr uint8_t OFFSETS[2][4] = {
            {0x01, 0x02, 0x04, 0x40},
            {0x08, 0x10, 0x20, 0x80}
        };
        if (dx >= 0 && dx < 2 && dy >= 0 && dy < 4)
            cp |= OFFSETS[dx][dy];
        return cp;
    }
};

} // namespace ventty::ascii
