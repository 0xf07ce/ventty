// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;

namespace ventty
{
/// Glyph metadata in atlas coordinates
struct GlyphInfo
{
    int atlasX = 0;    ///< Glyph top-left X in texture atlas (pixels)
    int atlasY = 0;    ///< Glyph top-left Y in texture atlas (pixels)
    int width = 0;     ///< Glyph width (pixels)
    int height = 0;    ///< Glyph height (pixels)
    int advance = 1;   ///< Logical cell advance: 1 half-width, 2 full-width
    int fontIndex = 0; ///< Loaded texture array index
};

/// Multi-atlas bitmap font manager
class GfxFont
{
public:
    GfxFont() = default;
    ~GfxFont();

    GfxFont(GfxFont const &) = delete;
    GfxFont & operator=(GfxFont const &) = delete;
    GfxFont(GfxFont &&) noexcept;
    GfxFont & operator=(GfxFont &&) noexcept;

    /// Load atlas from files (PNG + .cfnt metadata)
    bool load(SDL_Renderer * renderer, std::string const & pngPath, std::string const & cfntPath);

    /// Load built-in font (CP437 + Hangul)
    bool loadBuiltin(SDL_Renderer * renderer);

    /// Release cached textures
    void shutdown();

    /// Look up glyph metadata by codepoint
    GlyphInfo const * getGlyph(char32_t codepoint) const;

    /// Return atlas texture by loaded font index
    SDL_Texture * getTexture(int fontIndex) const;

    /// Return default cell width (pixels, based on first loaded font)
    int cellWidth() const { return _cellWidth; }

    /// Return default cell height (pixels, based on first loaded font)
    int cellHeight() const { return _cellHeight; }

private:
    bool parseCfnt(std::istream & stream, int fontIndex);

    void registerGlyphs(char32_t startCodepoint, unsigned int count,
        int cols, int glyphW, int glyphH, int advance, int fontIndex);
    void registerCodepage(char32_t const * codepage, unsigned int count,
        int cols, int glyphW, int glyphH, int advance, int fontIndex);

    std::vector<SDL_Texture *> _textures;
    std::unordered_map<char32_t, GlyphInfo> _glyphs;
    int _cellWidth = 8;
    int _cellHeight = 16;
};
} // namespace ventty
