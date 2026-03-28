#include "Font.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace ventty::gfx
{

Font::~Font()
{
    shutdown();
}

void Font::shutdown()
{
    for (auto & [cp, entry] : _glyphCache)
    {
        if (entry.texture)
            SDL_DestroyTexture(entry.texture);
    }
    _glyphCache.clear();
    if (_font)
    {
        TTF_CloseFont(_font);
        _font = nullptr;
    }
    _renderer = nullptr;
}

bool Font::open(std::string const & ttfPath, float ptSize)
{
    _font = TTF_OpenFont(ttfPath.c_str(), ptSize);
    if (!_font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to open font: %s (%s)", ttfPath.c_str(), SDL_GetError());
        return false;
    }

    _cellHeight = TTF_GetFontHeight(_font);

    // Determine cell width from advance of 'M' (monospace reference)
    int advance = 0;
    if (TTF_GetGlyphMetrics(_font, 'M', nullptr, nullptr, nullptr, nullptr, &advance))
    {
        _cellWidth = advance;
    }
    else
    {
        _cellWidth = _cellHeight / 2;
    }

    if (_cellWidth <= 0)
        _cellWidth = 8;
    if (_cellHeight <= 0)
        _cellHeight = 16;

    return true;
}

void Font::setRenderer(SDL_Renderer * renderer)
{
    _renderer = renderer;
}

GlyphEntry const * Font::getGlyph(char32_t codepoint)
{
    auto it = _glyphCache.find(codepoint);
    if (it != _glyphCache.end())
        return &it->second;
    return renderGlyph(codepoint);
}

GlyphEntry const * Font::renderGlyph(char32_t codepoint)
{
    if (!_font || !_renderer)
        return nullptr;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface * surface = TTF_RenderGlyph_Blended(_font, static_cast<Uint32>(codepoint), white);
    if (!surface)
    {
        // Fallback to '?'
        surface = TTF_RenderGlyph_Blended(_font, '?', white);
        if (!surface)
            return nullptr;
    }

    SDL_Texture * texture = SDL_CreateTextureFromSurface(_renderer, surface);
    int w = surface->w;
    int h = surface->h;
    SDL_DestroySurface(surface);

    if (!texture)
        return nullptr;

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    GlyphEntry entry;
    entry.texture = texture;
    entry.width = w;
    entry.height = h;
    entry.advance = ventty::utf8::isFullwidth(codepoint) ? 2 : 1;

    auto [it, inserted] = _glyphCache.emplace(codepoint, entry);
    return &it->second;
}

} // namespace ventty::gfx
