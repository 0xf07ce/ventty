#pragma once

#include <ventty/Utf8.h>

#include <cstdint>
#include <string>
#include <unordered_map>

struct SDL_Renderer;
struct SDL_Texture;
typedef struct TTF_Font TTF_Font;

namespace ventty::gfx
{
struct GlyphEntry
{
    SDL_Texture * texture = nullptr;
    int width = 0;
    int height = 0;
    int advance = 1; // 1 = halfwidth, 2 = fullwidth
};

class Font
{
public:
    Font() = default;
    ~Font();

    Font(Font const &) = delete;
    Font & operator=(Font const &) = delete;

    /// Open a TTF font and compute cell metrics (no renderer needed)
    bool open(std::string const & ttfPath, float ptSize);

    /// Set the renderer for lazy glyph texture creation
    void setRenderer(SDL_Renderer * renderer);

    /// Release all cached textures and close the font
    void shutdown();

    /// Get or lazily render a glyph for the given codepoint
    GlyphEntry const * getGlyph(char32_t codepoint);

    int cellWidth() const { return _cellWidth; }
    int cellHeight() const { return _cellHeight; }

private:
    GlyphEntry const * renderGlyph(char32_t codepoint);

    SDL_Renderer * _renderer = nullptr;
    TTF_Font * _font = nullptr;
    int _cellWidth = 8;
    int _cellHeight = 16;

    std::unordered_map<char32_t, GlyphEntry> _glyphCache;
};
} // namespace ventty::gfx
