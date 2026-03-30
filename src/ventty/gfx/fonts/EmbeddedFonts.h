#pragma once

#include <cstdint>

namespace ventty::fonts
{
extern unsigned char const CP437_8X16_PNG[];
extern unsigned int const CP437_8X16_PNG_SIZE;
constexpr int CP437_8X16_GLYPH_WIDTH = 8;
constexpr int CP437_8X16_GLYPH_HEIGHT = 16;
constexpr int CP437_8X16_ATLAS_COLS = 32;
constexpr int CP437_8X16_ADVANCE = 1;
constexpr unsigned int CP437_8X16_COUNT = 256;
extern char32_t const CP437_8X16_CODEPAGE[];

extern unsigned char const HANGUL_16X16_PNG[];
extern unsigned int const HANGUL_16X16_PNG_SIZE;
constexpr int HANGUL_16X16_GLYPH_WIDTH = 16;
constexpr int HANGUL_16X16_GLYPH_HEIGHT = 16;
constexpr int HANGUL_16X16_ATLAS_COLS = 128;
constexpr int HANGUL_16X16_ADVANCE = 2;
constexpr unsigned int HANGUL_16X16_COUNT = 11184;
constexpr char32_t HANGUL_16X16_START = 44032;

} // namespace ventty::fonts
