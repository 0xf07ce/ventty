#pragma once

#include <ventty/core/Color.h>

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
// -- Box Drawing --

/// 박스 그리기 문자 세트
struct BoxChars
{
    char32_t tl, tr, bl, br; ///< 모서리 (좌상, 우상, 좌하, 우하)
    char32_t h, v;           ///< 수평선, 수직선
    char32_t lt, rt, tt, bt; ///< T자 연결 (좌, 우, 상, 하)
    char32_t cross;          ///< 십자 교차점
};

/// 단선 박스 문자
constexpr BoxChars SINGLE_BOX = {
    U'┌', U'┐', U'└', U'┘',
    U'─', U'│',
    U'├', U'┤', U'┬', U'┴',
    U'┼'
};

/// 이중선 박스 문자
constexpr BoxChars DOUBLE_BOX = {
    U'╔', U'╗', U'╚', U'╝',
    U'═', U'║',
    U'╠', U'╣', U'╦', U'╩',
    U'╬'
};

/// 둥근 모서리 박스 문자
constexpr BoxChars ROUND_BOX = {
    U'╭', U'╮', U'╰', U'╯',
    U'─', U'│',
    U'├', U'┤', U'┬', U'┴',
    U'┼'
};

/// 굵은선 박스 문자
constexpr BoxChars HEAVY_BOX = {
    U'┏', U'┓', U'┗', U'┛',
    U'━', U'┃',
    U'┣', U'┫', U'┳', U'┻',
    U'╋'
};

// -- Spinner Frames --

/// 점자 패턴 스피너 프레임
constexpr std::array<std::string_view, 10> SPINNER_BRAILLE = {
    "\u280B", "\u2819", "\u2839", "\u2838",
    "\u283C", "\u2834", "\u2826", "\u2827",
    "\u2807", "\u280F"
};

/// 선 스피너 프레임
constexpr std::array<std::string_view, 4> SPINNER_LINE = {
    "|", "/", "-", "\\"
};

/// 블록 스피너 프레임
constexpr std::array<std::string_view, 4> SPINNER_BLOCK = {
    "\u2596", "\u2598", "\u259D", "\u2597"
};

/// 달 위상 스피너 프레임
constexpr std::array<std::string_view, 8> SPINNER_MOON = {
    "\U0001F311", "\U0001F312", "\U0001F313", "\U0001F314",
    "\U0001F315", "\U0001F316", "\U0001F317", "\U0001F318"
};

/// 점 스피너 프레임
constexpr std::array<std::string_view, 4> SPINNER_DOTS = {
    "   ", ".  ", ".. ", "..."
};

// -- Progress Bar --

/// 진행률 표시줄 스타일
struct ProgressStyle
{
    char32_t filled = U'█';            ///< 채워진 부분 문자
    char32_t partial = U'▓';           ///< 부분 채움 문자
    char32_t empty = U'░';             ///< 빈 부분 문자
    char32_t leftCap = U'[';           ///< 왼쪽 캡 문자
    char32_t rightCap = U']';          ///< 오른쪽 캡 문자
    Color filledFg = Colors::GREEN;    ///< 채워진 부분 전경색
    Color emptyFg = Colors::DARK_GRAY; ///< 빈 부분 전경색
    Color capFg = Colors::LIGHT_GRAY;  ///< 캡 전경색
};

/// 블록 스타일 진행률 표시줄
constexpr ProgressStyle PROGRESS_BLOCK = {};

/// 부드러운 선 스타일 진행률 표시줄
constexpr ProgressStyle PROGRESS_SMOOTH = {
    U'━', U'╸', U'─',
    U'╺', U'╸',
    Colors::CYAN, Colors::DARK_GRAY, Colors::LIGHT_GRAY
};

/// 해시 스타일 진행률 표시줄
constexpr ProgressStyle PROGRESS_HASH = {
    U'#', U'>', U'-',
    U'[', U']',
    Colors::GREEN, Colors::DARK_GRAY, Colors::LIGHT_GRAY
};

/// 진행률 표시줄 문자열 생성
std::string progressBar(int width, float ratio, ProgressStyle const & style = PROGRESS_BLOCK);

// -- Horizontal Rule --

/// 가는 수평선
constexpr char32_t HR_THIN = U'─';
/// 굵은 수평선
constexpr char32_t HR_THICK = U'━';
/// 이중 수평선
constexpr char32_t HR_DOUBLE = U'═';
/// 점선 수평선
constexpr char32_t HR_DOTTED = U'┄';
/// 파선 수평선
constexpr char32_t HR_DASHED = U'┅';

// -- Block Shade Characters --

/// 밝은 음영 블록
constexpr char32_t SHADE_LIGHT = U'░';
/// 중간 음영 블록
constexpr char32_t SHADE_MEDIUM = U'▒';
/// 어두운 음영 블록
constexpr char32_t SHADE_DARK = U'▓';
/// 전체 채움 블록
constexpr char32_t SHADE_FULL = U'█';

/// Vertical bar graph characters (1/8 increments)
constexpr std::array<char32_t, 9> VBAR = {
    U' ', U'▁', U'▂', U'▃', U'▄', U'▅', U'▆', U'▇', U'█'
};

/// Horizontal bar graph characters (1/8 increments)
constexpr std::array<char32_t, 9> HBAR = {
    U' ', U'▏', U'▎', U'▍', U'▌', U'▋', U'▊', U'▉', U'█'
};

// -- Braille Plotting --

/// 점자 패턴 기반 플롯 유틸리티
struct BraillePlot
{
    /// 2x4 점 배열에서 점자 문자 생성
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

    /// 점자 문자에 특정 점 추가
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
