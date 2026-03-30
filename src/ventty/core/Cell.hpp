#pragma once

#include <ventty/core/Color.h>

namespace ventty
{
/// 화면의 한 칸을 나타내는 셀
struct Cell
{
    char32_t ch = U' '; ///< 유니코드 문자 (코드포인트)
    Style style;        ///< 셀의 스타일 (전경색, 배경색, 속성)
    uint8_t width = 1;  ///< 표시 너비 (1: 일반, 2: CJK 전각 문자)

    /// 두 셀이 같은지 비교
    bool operator==(Cell const & other) const
    {
        return ch == other.ch && style == other.style && width == other.width;
    }

    /// 두 셀이 다른지 비교
    bool operator!=(Cell const & other) const
    {
        return !(*this == other);
    }
};
} // namespace ventty
