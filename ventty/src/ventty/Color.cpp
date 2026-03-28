#include "Color.h"

#include <charconv>
#include <cstdio>

namespace ventty
{

Color Color::fromHex(std::string_view hex)
{
    if (hex.empty())
    {
        return Colors::BLACK;
    }

    if (hex[0] == '#')
    {
        hex = hex.substr(1);
    }

    if (hex.size() != 6)
    {
        return Colors::BLACK;
    }

    auto parseHexByte = [](std::string_view s) -> uint8_t
    {
        uint8_t value = 0;
        auto result = std::from_chars(s.data(), s.data() + s.size(), value, 16);
        if (result.ec != std::errc{})
        {
            return 0;
        }
        return value;
    };

    return Color{
        parseHexByte(hex.substr(0, 2)),
        parseHexByte(hex.substr(2, 2)),
        parseHexByte(hex.substr(4, 2))
    };
}

std::string Color::toHex() const
{
    char buf[8];
    std::snprintf(buf, sizeof(buf), "#%02X%02X%02X", r, g, b);
    return std::string(buf);
}

} // namespace ventty
