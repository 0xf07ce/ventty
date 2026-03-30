#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace ventty
{
struct Color
{
    uint8_t r = 0; ///< 빨간색 채널 (0-255)
    uint8_t g = 0; ///< 초록색 채널 (0-255)
    uint8_t b = 0; ///< 파란색 채널 (0-255)

    /// 기본 생성자 (검정색)
    constexpr Color() = default;

    /// RGB 값으로 Color 생성
    constexpr Color(uint8_t red, uint8_t green, uint8_t blue)
        : r(red), g(green), b(blue)
    {}

    /// 두 색상이 같은지 비교
    constexpr bool operator==(Color const & other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    /// 두 색상이 다른지 비교
    constexpr bool operator!=(Color const & other) const
    {
        return !(*this == other);
    }

    /// 16진수 문자열에서 Color 생성 (예: "#FF0000")
    static Color fromHex(std::string_view hex);

    /// 16진수 문자열로 변환
    std::string toHex() const;

    /// DOS 16색 팔레트 인덱스에서 Color 변환
    static constexpr Color fromDosIndex(int index);
};

// DOS 16-color palette (True Color values)
namespace Colors
{
/// 검정색
inline constexpr Color BLACK { 0x00, 0x00, 0x00 };

/// 파란색
inline constexpr Color BLUE { 0x00, 0x00, 0xAA };

/// 초록색
inline constexpr Color GREEN { 0x00, 0xAA, 0x00 };

/// 청록색
inline constexpr Color CYAN { 0x00, 0xAA, 0xAA };

/// 빨간색
inline constexpr Color RED { 0xAA, 0x00, 0x00 };

/// 자홍색
inline constexpr Color MAGENTA { 0xAA, 0x00, 0xAA };

/// 갈색
inline constexpr Color BROWN { 0xAA, 0x55, 0x00 };

/// 밝은 회색
inline constexpr Color LIGHT_GRAY { 0xAA, 0xAA, 0xAA };

/// 어두운 회색
inline constexpr Color DARK_GRAY { 0x55, 0x55, 0x55 };

/// 밝은 파란색
inline constexpr Color LIGHT_BLUE { 0x55, 0x55, 0xFF };

/// 밝은 초록색
inline constexpr Color LIGHT_GREEN { 0x55, 0xFF, 0x55 };

/// 밝은 청록색
inline constexpr Color LIGHT_CYAN { 0x55, 0xFF, 0xFF };

/// 밝은 빨간색
inline constexpr Color LIGHT_RED { 0xFF, 0x55, 0x55 };

/// 밝은 자홍색
inline constexpr Color LIGHT_MAGENTA { 0xFF, 0x55, 0xFF };

/// 노란색
inline constexpr Color YELLOW { 0xFF, 0xFF, 0x55 };

/// 흰색
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
/// CGA 16색 팔레트 배열
inline constexpr Color CGA_16[16] = {
    Colors::BLACK, Colors::BLUE, Colors::GREEN, Colors::CYAN,
    Colors::RED, Colors::MAGENTA, Colors::BROWN, Colors::LIGHT_GRAY,
    Colors::DARK_GRAY, Colors::LIGHT_BLUE, Colors::LIGHT_GREEN, Colors::LIGHT_CYAN,
    Colors::LIGHT_RED, Colors::LIGHT_MAGENTA, Colors::YELLOW, Colors::WHITE
};

/// xterm 256색 팔레트에서 Color 변환
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

/// 텍스트 속성 비트 플래그
enum class Attr : uint8_t
{
    None      = 0,      ///< 속성 없음
    Bold      = 1 << 0, ///< 굵게
    Dim       = 1 << 1, ///< 흐리게
    Italic    = 1 << 2, ///< 기울임
    Underline = 1 << 3, ///< 밑줄
    Blink     = 1 << 4, ///< 깜빡임
    Reverse   = 1 << 5, ///< 반전
    Strike    = 1 << 6, ///< 취소선
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

/// 셀의 전경색, 배경색, 텍스트 속성을 포함하는 스타일
struct Style
{
    Color fg = Colors::LIGHT_GRAY; ///< 전경색 (글자 색상)
    Color bg = Colors::BLACK;      ///< 배경색
    Attr attr = Attr::None;        ///< 텍스트 속성 (굵게, 기울임 등)

    /// 기본 생성자 (밝은 회색 전경, 검정 배경)
    constexpr Style() = default;

    /// 전경색, 배경색, 속성으로 Style 생성
    constexpr Style(Color foreground, Color background, Attr attributes = Attr::None)
        : fg(foreground), bg(background), attr(attributes)
    {}

    /// 두 스타일이 같은지 비교
    constexpr bool operator==(Style const & other) const
    {
        return fg == other.fg && bg == other.bg && attr == other.attr;
    }

    /// 두 스타일이 다른지 비교
    constexpr bool operator!=(Style const & other) const
    {
        return !(*this == other);
    }
};
} // namespace ventty
