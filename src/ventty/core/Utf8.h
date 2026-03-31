#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
/// Decode a single codepoint from a UTF-8 string
char32_t decode(std::string_view str, size_t & pos);

/// Encode a codepoint as UTF-8 and append to the string
void encode(char32_t cp, std::string & out);

/// Convert a UTF-8 string to a vector of codepoints
std::vector<char32_t> toCodepoints(std::string_view str);

/// Convert a vector of codepoints to a UTF-8 string
std::string fromCodepoints(std::vector<char32_t> const & cps);

/// Check whether a codepoint is full-width (CJK)
bool isFullwidth(char32_t cp);

/// Return display width of a codepoint (1 or 2)
inline int displayWidth(char32_t cp)
{
    return isFullwidth(cp) ? 2 : 1;
}

/// Return total display width of a UTF-8 string
int stringWidth(std::string_view str);
} // namespace ventty
