// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "Utf8.h"

namespace ventty
{
char32_t decode(std::string_view str, size_t & pos)
{
    if (pos >= str.size())
        return U'\0';

    auto byte = static_cast<uint8_t>(str[pos]);
    char32_t cp;
    int extra;

    if (byte < 0x80)
    {
        cp = byte;
        extra = 0;
    }
    else if ((byte & 0xE0) == 0xC0)
    {
        cp = byte & 0x1F;
        extra = 1;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        cp = byte & 0x0F;
        extra = 2;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        cp = byte & 0x07;
        extra = 3;
    }
    else
    {
        ++pos;
        return U'\uFFFD';
    }

    if (pos + extra >= str.size() + 1)
    {
        pos = str.size();
        return U'\uFFFD';
    }

    for (int i = 0; i < extra; ++i)
    {
        auto cont = static_cast<uint8_t>(str[pos + 1 + i]);
        if ((cont & 0xC0) != 0x80)
        {
            pos += 1 + i;
            return U'\uFFFD';
        }
        cp = (cp << 6) | (cont & 0x3F);
    }

    pos += 1 + extra;
    return cp;
}

void encode(char32_t cp, std::string & out)
{
    if (cp < 0x80)
        out += static_cast<char>(cp);
    else if (cp < 0x800)
    {
        out += static_cast<char>(0xC0 | (cp >> 6));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp < 0x10000)
    {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp < 0x110000)
    {
        out += static_cast<char>(0xF0 | (cp >> 18));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

std::vector<char32_t> toCodepoints(std::string_view str)
{
    std::vector<char32_t> result;
    size_t pos = 0;
    while (pos < str.size())
        result.push_back(decode(str, pos));
    return result;
}

std::string fromCodepoints(std::vector<char32_t> const & cps)
{
    std::string result;
    for (auto const & cp : cps)
        encode(cp, result);
    return result;
}

bool isFullwidth(char32_t cp)
{
    // East Asian Wide / Fullwidth per Unicode UAX #11. The previous version
    // omitted Hiragana, Katakana, and several other blocks, which made e.g.
    // U+306E (の) render as width 1 — adjacent glyphs then overwrote it on
    // the terminal grid, so the kana appeared "missing" between kanji.
    return (cp >= 0x1100 && cp <= 0x115F) ||    // Hangul Jamo (initials)
        (cp >= 0x2E80 && cp <= 0x303E) ||       // CJK Radicals / Kangxi / CJK Symbols & Punctuation
        (cp >= 0x3041 && cp <= 0x33FF) ||       // Hiragana, Katakana, Bopomofo, Hangul Compat Jamo, Kanbun, CJK Strokes, Enclosed CJK, CJK Compatibility
        (cp >= 0x3400 && cp <= 0x4DBF) ||       // CJK Extension A
        (cp >= 0x4E00 && cp <= 0x9FFF) ||       // CJK Unified Ideographs
        (cp >= 0xA000 && cp <= 0xA4CF) ||       // Yi Syllables / Yi Radicals
        (cp >= 0xA960 && cp <= 0xA97F) ||       // Hangul Jamo Extended-A
        (cp >= 0xAC00 && cp <= 0xD7A3) ||       // Hangul Syllables
        (cp >= 0xD7B0 && cp <= 0xD7FF) ||       // Hangul Jamo Extended-B
        (cp >= 0xF900 && cp <= 0xFAFF) ||       // CJK Compatibility Ideographs
        (cp >= 0xFE10 && cp <= 0xFE1F) ||       // Vertical Forms
        (cp >= 0xFE30 && cp <= 0xFE6F) ||       // CJK Compatibility Forms / Small Form Variants
        (cp >= 0xFF00 && cp <= 0xFF60) ||       // Fullwidth ASCII (U+FF61–U+FFDC is halfwidth katakana — width 1)
        (cp >= 0xFFE0 && cp <= 0xFFE6) ||       // Fullwidth signs
        (cp >= 0x1B000 && cp <= 0x1B16F) ||     // Kana Supplement / Kana Ext-A / Small Kana Ext
        (cp >= 0x1F200 && cp <= 0x1F2FF) ||     // Enclosed Ideographic Supplement
        (cp >= 0x20000 && cp <= 0x2FFFD) ||     // CJK Ext B–F / Compat Ideographs Supplement
        (cp >= 0x30000 && cp <= 0x3FFFD);       // CJK Ext G+
}

int stringWidth(std::string_view str)
{
    int w = 0;
    size_t pos = 0;
    while (pos < str.size())
    {
        auto cp = decode(str, pos);
        w += displayWidth(cp);
    }
    return w;
}
} // namespace ventty
