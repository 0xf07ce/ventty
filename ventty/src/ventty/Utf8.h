#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace ventty::utf8
{
char32_t decode(std::string_view str, size_t & pos);
void encode(char32_t cp, std::string & out);
std::vector<char32_t> toCodepoints(std::string_view str);
std::string fromCodepoints(std::vector<char32_t> const & cps);
bool isFullwidth(char32_t cp);

/// Return display width of a codepoint (1 or 2)
inline int displayWidth(char32_t cp)
{
    return isFullwidth(cp) ? 2 : 1;
}

/// Return total display width of a UTF-8 string
int stringWidth(std::string_view str);
} // namespace ventty::utf8
