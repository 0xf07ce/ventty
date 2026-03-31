#pragma once

#include <ventty/core/Color.h>

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
// -- Box Drawing --

/// Box drawing character set
struct BoxChars
{
    char32_t tl, tr, bl, br; ///< Corners (top-left, top-right, bottom-left, bottom-right)
    char32_t h, v;           ///< Horizontal line, vertical line
    char32_t lt, rt, tt, bt; ///< T-junctions (left, right, top, bottom)
    char32_t cross;          ///< Cross intersection
};

/// Single-line box characters
constexpr BoxChars SINGLE_BOX = {
    U'\u250C', U'\u2510', U'\u2514', U'\u2518',
    U'\u2500', U'\u2502',
    U'\u251C', U'\u2524', U'\u252C', U'\u2534',
    U'\u253C'
};

/// Double-line box characters
constexpr BoxChars DOUBLE_BOX = {
    U'\u2554', U'\u2557', U'\u255A', U'\u255D',
    U'\u2550', U'\u2551',
    U'\u2560', U'\u2563', U'\u2566', U'\u2569',
    U'\u256C'
};

/// Rounded corner box characters
constexpr BoxChars ROUND_BOX = {
    U'\u256D', U'\u256E', U'\u2570', U'\u256F',
    U'\u2500', U'\u2502',
    U'\u251C', U'\u2524', U'\u252C', U'\u2534',
    U'\u253C'
};

/// Heavy-line box characters
constexpr BoxChars HEAVY_BOX = {
    U'\u250F', U'\u2513', U'\u2517', U'\u251B',
    U'\u2501', U'\u2503',
    U'\u2523', U'\u252B', U'\u2533', U'\u253B',
    U'\u254B'
};

// -- Spinner Frames --

/// Braille pattern spinner frames
constexpr std::array<std::string_view, 10> SPINNER_BRAILLE = {
    "\u280B", "\u2819", "\u2839", "\u2838",
    "\u283C", "\u2834", "\u2826", "\u2827",
    "\u2807", "\u280F"
};

/// Line spinner frames
constexpr std::array<std::string_view, 4> SPINNER_LINE = {
    "|", "/", "-", "\\"
};

/// Block spinner frames
constexpr std::array<std::string_view, 4> SPINNER_BLOCK = {
    "\u2596", "\u2598", "\u259D", "\u2597"
};

/// Moon phase spinner frames
constexpr std::array<std::string_view, 8> SPINNER_MOON = {
    "\U0001F311", "\U0001F312", "\U0001F313", "\U0001F314",
    "\U0001F315", "\U0001F316", "\U0001F317", "\U0001F318"
};

/// Dot spinner frames
constexpr std::array<std::string_view, 4> SPINNER_DOTS = {
    "   ", ".  ", ".. ", "..."
};

// -- Progress Bar --

/// Progress bar style
struct ProgressStyle
{
    char32_t filled = U'\u2588';            ///< Filled portion character
    char32_t partial = U'\u2593';           ///< Partial fill character
    char32_t empty = U'\u2591';             ///< Empty portion character
    char32_t leftCap = U'[';           ///< Left cap character
    char32_t rightCap = U']';          ///< Right cap character
    Color filledFg = Colors::GREEN;    ///< Filled portion foreground color
    Color emptyFg = Colors::DARK_GRAY; ///< Empty portion foreground color
    Color capFg = Colors::LIGHT_GRAY;  ///< Cap foreground color
};

/// Block style progress bar
constexpr ProgressStyle PROGRESS_BLOCK = {};

/// Smooth line style progress bar
constexpr ProgressStyle PROGRESS_SMOOTH = {
    U'\u2501', U'\u2578', U'\u2500',
    U'\u257A', U'\u2578',
    Colors::CYAN, Colors::DARK_GRAY, Colors::LIGHT_GRAY
};

/// Hash style progress bar
constexpr ProgressStyle PROGRESS_HASH = {
    U'#', U'>', U'-',
    U'[', U']',
    Colors::GREEN, Colors::DARK_GRAY, Colors::LIGHT_GRAY
};

/// Generate a progress bar string
std::string progressBar(int width, float ratio, ProgressStyle const & style = PROGRESS_BLOCK);

// -- Horizontal Rule --

/// Thin horizontal rule
constexpr char32_t HR_THIN = U'\u2500';
/// Thick horizontal rule
constexpr char32_t HR_THICK = U'\u2501';
/// Double horizontal rule
constexpr char32_t HR_DOUBLE = U'\u2550';
/// Dotted horizontal rule
constexpr char32_t HR_DOTTED = U'\u2504';
/// Dashed horizontal rule
constexpr char32_t HR_DASHED = U'\u2505';

// -- Block Shade Characters --

/// Light shade block
constexpr char32_t SHADE_LIGHT = U'\u2591';
/// Medium shade block
constexpr char32_t SHADE_MEDIUM = U'\u2592';
/// Dark shade block
constexpr char32_t SHADE_DARK = U'\u2593';
/// Full block
constexpr char32_t SHADE_FULL = U'\u2588';

/// Vertical bar graph characters (1/8 increments)
constexpr std::array<char32_t, 9> VBAR = {
    U' ', U'\u2581', U'\u2582', U'\u2583', U'\u2584', U'\u2585', U'\u2586', U'\u2587', U'\u2588'
};

/// Horizontal bar graph characters (1/8 increments)
constexpr std::array<char32_t, 9> HBAR = {
    U' ', U'\u258F', U'\u258E', U'\u258D', U'\u258C', U'\u258B', U'\u258A', U'\u2589', U'\u2588'
};

// -- Braille Plotting --

/// Braille pattern-based plot utility
struct BraillePlot
{
    /// Generate braille character from 2x4 dot array
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

    /// Add a specific dot to a braille character
    static constexpr char32_t setDot(char32_t cp, int dx, int dy)
    {
        constexpr uint8_t OFFSETS[2][4] = {
            { 0x01, 0x02, 0x04, 0x40 },
            { 0x08, 0x10, 0x20, 0x80 }
        };
        if (dx >= 0 && dx < 2 && dy >= 0 && dy < 4)
            cp |= OFFSETS[dx][dy];
        return cp;
    }
};
} // namespace ventty
