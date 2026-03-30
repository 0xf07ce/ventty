#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace ventty
{
struct Color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    constexpr Color() = default;

    constexpr Color(uint8_t red, uint8_t green, uint8_t blue)
        : r(red), g(green), b(blue)
    {}

    constexpr bool operator==(Color const & other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    constexpr bool operator!=(Color const & other) const
    {
        return !(*this == other);
    }

    static Color fromHex(std::string_view hex);
    std::string toHex() const;

    static constexpr Color fromDosIndex(int index);
};

// DOS 16-color palette (True Color values)
namespace Colors
{
inline constexpr Color BLACK { 0x00, 0x00, 0x00 };
inline constexpr Color BLUE { 0x00, 0x00, 0xAA };
inline constexpr Color GREEN { 0x00, 0xAA, 0x00 };
inline constexpr Color CYAN { 0x00, 0xAA, 0xAA };
inline constexpr Color RED { 0xAA, 0x00, 0x00 };
inline constexpr Color MAGENTA { 0xAA, 0x00, 0xAA };
inline constexpr Color BROWN { 0xAA, 0x55, 0x00 };
inline constexpr Color LIGHT_GRAY { 0xAA, 0xAA, 0xAA };
inline constexpr Color DARK_GRAY { 0x55, 0x55, 0x55 };
inline constexpr Color LIGHT_BLUE { 0x55, 0x55, 0xFF };
inline constexpr Color LIGHT_GREEN { 0x55, 0xFF, 0x55 };
inline constexpr Color LIGHT_CYAN { 0x55, 0xFF, 0xFF };
inline constexpr Color LIGHT_RED { 0xFF, 0x55, 0x55 };
inline constexpr Color LIGHT_MAGENTA { 0xFF, 0x55, 0xFF };
inline constexpr Color YELLOW { 0xFF, 0xFF, 0x55 };
inline constexpr Color WHITE { 0xFF, 0xFF, 0xFF };
}

constexpr Color Color::fromDosIndex(int index)
{
    switch (index)
    {
    case 0: return Colors::BLACK;
    case 1: return Colors::BLUE;
    case 2: return Colors::GREEN;
    case 3: return Colors::CYAN;
    case 4: return Colors::RED;
    case 5: return Colors::MAGENTA;
    case 6: return Colors::BROWN;
    case 7: return Colors::LIGHT_GRAY;
    case 8: return Colors::DARK_GRAY;
    case 9: return Colors::LIGHT_BLUE;
    case 10: return Colors::LIGHT_GREEN;
    case 11: return Colors::LIGHT_CYAN;
    case 12: return Colors::LIGHT_RED;
    case 13: return Colors::LIGHT_MAGENTA;
    case 14: return Colors::YELLOW;
    case 15: return Colors::WHITE;
    default: return Colors::BLACK;
    }
}

/// Linearly interpolate between two colors
constexpr Color lerpColor(Color a, Color b, float t)
{
    if (t <= 0.0f) return a;
    if (t >= 1.0f) return b;
    auto mix = [t](uint8_t x, uint8_t y) -> uint8_t
    {
        return static_cast<uint8_t>(x + (y - x) * t);
    };
    return Color { mix(a.r, b.r), mix(a.g, b.g), mix(a.b, b.b) };
}

namespace Palette
{
inline constexpr Color CGA_16[16] = {
    Colors::BLACK, Colors::BLUE, Colors::GREEN, Colors::CYAN,
    Colors::RED, Colors::MAGENTA, Colors::BROWN, Colors::LIGHT_GRAY,
    Colors::DARK_GRAY, Colors::LIGHT_BLUE, Colors::LIGHT_GREEN, Colors::LIGHT_CYAN,
    Colors::LIGHT_RED, Colors::LIGHT_MAGENTA, Colors::YELLOW, Colors::WHITE
};

constexpr Color xterm256(uint8_t index)
{
    if (index < 16)
        return CGA_16[index];
    if (index < 232)
    {
        int i = index - 16;
        auto r = static_cast<uint8_t>((i / 36) * 51);
        auto g = static_cast<uint8_t>(((i / 6) % 6) * 51);
        auto b = static_cast<uint8_t>((i % 6) * 51);
        return Color { r, g, b };
    }
    auto v = static_cast<uint8_t>(8 + (index - 232) * 10);
    return Color { v, v, v };
}
} // namespace Palette

enum class Attr : uint8_t
{
    None      = 0,
    Bold      = 1 << 0,
    Dim       = 1 << 1,
    Italic    = 1 << 2,
    Underline = 1 << 3,
    Blink     = 1 << 4,
    Reverse   = 1 << 5,
    Strike    = 1 << 6,
};

constexpr Attr operator|(Attr a, Attr b)
{
    return static_cast<Attr>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr Attr operator&(Attr a, Attr b)
{
    return static_cast<Attr>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

constexpr bool hasAttr(Attr attrs, Attr flag)
{
    return (static_cast<uint8_t>(attrs) & static_cast<uint8_t>(flag)) != 0;
}

struct Style
{
    Color fg = Colors::LIGHT_GRAY;
    Color bg = Colors::BLACK;
    Attr attr = Attr::None;

    constexpr Style() = default;

    constexpr Style(Color foreground, Color background, Attr attributes = Attr::None)
        : fg(foreground), bg(background), attr(attributes)
    {}

    constexpr bool operator==(Style const & other) const
    {
        return fg == other.fg && bg == other.bg && attr == other.attr;
    }

    constexpr bool operator!=(Style const & other) const
    {
        return !(*this == other);
    }
};
} // namespace ventty
