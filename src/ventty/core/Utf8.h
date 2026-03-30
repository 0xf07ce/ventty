#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
/// UTF-8 문자열에서 하나의 코드포인트를 디코딩
char32_t decode(std::string_view str, size_t & pos);

/// 코드포인트를 UTF-8로 인코딩하여 문자열에 추가
void encode(char32_t cp, std::string & out);

/// UTF-8 문자열을 코드포인트 벡터로 변환
std::vector<char32_t> toCodepoints(std::string_view str);

/// 코드포인트 벡터를 UTF-8 문자열로 변환
std::string fromCodepoints(std::vector<char32_t> const & cps);

/// 코드포인트가 전각(CJK) 문자인지 판별
bool isFullwidth(char32_t cp);

/// Return display width of a codepoint (1 or 2)
inline int displayWidth(char32_t cp)
{
    return isFullwidth(cp) ? 2 : 1;
}

/// Return total display width of a UTF-8 string
int stringWidth(std::string_view str);
} // namespace ventty
