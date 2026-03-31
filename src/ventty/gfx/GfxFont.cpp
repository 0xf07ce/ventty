#include "GfxFont.h"
#include "fonts/EmbeddedFonts.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace ventty
{
GfxFont::~GfxFont()
{
    shutdown();
}

GfxFont::GfxFont(GfxFont && other) noexcept :
    _textures(std::move(other._textures)),
    _glyphs(std::move(other._glyphs)),
    _cellWidth(other._cellWidth),
    _cellHeight(other._cellHeight)
{
    other._textures.clear();
}

GfxFont & GfxFont::operator=(GfxFont && other) noexcept
{
    if (this != &other)
    {
        shutdown();
        _textures = std::move(other._textures);
        _glyphs = std::move(other._glyphs);
        _cellWidth = other._cellWidth;
        _cellHeight = other._cellHeight;
        other._textures.clear();
    }
    return *this;
}

void GfxFont::shutdown()
{
    for (auto * tex : _textures)
    {
        if (tex)
            SDL_DestroyTexture(tex);
    }
    _textures.clear();
    _glyphs.clear();
}

bool GfxFont::load(SDL_Renderer * renderer, std::string const & pngPath, std::string const & cfntPath)
{
    SDL_Surface * surface = IMG_Load(pngPath.c_str());
    if (!surface)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to load font atlas image: %s (%s)", pngPath.c_str(), SDL_GetError());
        return false;
    }

    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to create texture from atlas: %s", SDL_GetError());
        return false;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    int fontIndex = static_cast<int>(_textures.size());
    _textures.push_back(texture);

    std::ifstream file(cfntPath);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to open font metadata: %s", cfntPath.c_str());
        return false;
    }

    if (!parseCfnt(file, fontIndex))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to parse font metadata: %s", cfntPath.c_str());
        return false;
    }

    return true;
}

static SDL_Texture * loadPngFromMemory(SDL_Renderer * renderer, unsigned char const * data, unsigned int size)
{
    SDL_IOStream * io = SDL_IOFromConstMem(data, size);
    if (!io)
        return nullptr;

    SDL_Surface * surface = IMG_Load_IO(io, true);
    if (!surface)
        return nullptr;

    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture)
        return nullptr;

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

void GfxFont::registerGlyphs(char32_t codepoint, unsigned int count,
    int cols, int glyphW, int glyphH, int advance, int fontIndex)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        GlyphInfo info;
        info.atlasX = static_cast<int>(i % cols) * glyphW;
        info.atlasY = static_cast<int>(i / cols) * glyphH;
        info.width = glyphW;
        info.height = glyphH;
        info.advance = advance;
        info.fontIndex = fontIndex;
        _glyphs[codepoint + i] = info;
    }
}

void GfxFont::registerCodepage(char32_t const * codepage, unsigned int count,
    int cols, int glyphW, int glyphH, int advance, int fontIndex)
{
    for (unsigned int i = 0; i < count; ++i)
    {
        GlyphInfo info;
        info.atlasX = static_cast<int>(i % cols) * glyphW;
        info.atlasY = static_cast<int>(i / cols) * glyphH;
        info.width = glyphW;
        info.height = glyphH;
        info.advance = advance;
        info.fontIndex = fontIndex;
        _glyphs[codepage[i]] = info;
    }
}

bool GfxFont::loadBuiltin(SDL_Renderer * renderer)
{
    // CP437 (half-width, codepage mapping)
    SDL_Texture * cp437 = loadPngFromMemory(renderer,
        fonts::CP437_8X16_PNG, fonts::CP437_8X16_PNG_SIZE);
    if (!cp437)
        return false;

    int cp437Index = static_cast<int>(_textures.size());
    _textures.push_back(cp437);
    _cellWidth = fonts::CP437_8X16_GLYPH_WIDTH;
    _cellHeight = fonts::CP437_8X16_GLYPH_HEIGHT;

    registerCodepage(fonts::CP437_8X16_CODEPAGE, fonts::CP437_8X16_COUNT,
        fonts::CP437_8X16_ATLAS_COLS,
        fonts::CP437_8X16_GLYPH_WIDTH, fonts::CP437_8X16_GLYPH_HEIGHT,
        fonts::CP437_8X16_ADVANCE, cp437Index);

    // Hangul (full-width, contiguous range U+AC00..U+D7AF)
    SDL_Texture * hangul = loadPngFromMemory(renderer, fonts::HANGUL_16X16_PNG, fonts::HANGUL_16X16_PNG_SIZE);
    if (!hangul)
        return false;

    int hangulIndex = static_cast<int>(_textures.size());
    _textures.push_back(hangul);

    registerGlyphs(fonts::HANGUL_16X16_START, fonts::HANGUL_16X16_COUNT,
        fonts::HANGUL_16X16_ATLAS_COLS,
        fonts::HANGUL_16X16_GLYPH_WIDTH, fonts::HANGUL_16X16_GLYPH_HEIGHT,
        fonts::HANGUL_16X16_ADVANCE, hangulIndex);

    // Hangul Compatibility Jamo (full-width, contiguous range U+3131..U+318E)
    SDL_Texture * jamo = loadPngFromMemory(renderer, fonts::JAMO_16X16_PNG, fonts::JAMO_16X16_PNG_SIZE);
    if (!jamo)
        return false;

    int jamoIndex = static_cast<int>(_textures.size());
    _textures.push_back(jamo);

    registerGlyphs(fonts::JAMO_16X16_START, fonts::JAMO_16X16_COUNT,
        fonts::JAMO_16X16_ATLAS_COLS,
        fonts::JAMO_16X16_GLYPH_WIDTH, fonts::JAMO_16X16_GLYPH_HEIGHT,
        fonts::JAMO_16X16_ADVANCE, jamoIndex);

    return true;
}

bool GfxFont::parseCfnt(std::istream & stream, int fontIndex)
{
    int glyphW = 0;
    int glyphH = 0;
    int atlasCols = 0;
    int advance = 1;

    std::string token;
    while (stream >> token)
    {
        if (token == "glyph")
        {
            stream >> glyphW >> glyphH;
            if (fontIndex == 0)
            {
                _cellWidth = glyphW;
                _cellHeight = glyphH;
            }
        }
        else if (token == "atlas_cols")
        {
            stream >> atlasCols;
        }
        else if (token == "advance")
        {
            stream >> advance;
        }
        else if (token == "range")
        {
            uint32_t start = 0;
            unsigned int count = 0;
            stream >> start >> count;
            registerGlyphs(static_cast<char32_t>(start), count,
                atlasCols, glyphW, glyphH, advance, fontIndex);
        }
        else if (token == "codepage")
        {
            unsigned int count = 0;
            stream >> count;
            std::vector<char32_t> codepage(count);
            for (unsigned int i = 0; i < count; ++i)
            {
                uint32_t cp = 0;
                stream >> cp;
                codepage[i] = static_cast<char32_t>(cp);
            }
            registerCodepage(codepage.data(), count, atlasCols, glyphW, glyphH, advance, fontIndex);
        }
    }

    return glyphW > 0 && atlasCols > 0;
}

GlyphInfo const * GfxFont::getGlyph(char32_t codepoint) const
{
    auto it = _glyphs.find(codepoint);
    if (it != _glyphs.end())
        return &it->second;
    it = _glyphs.find(U'?');
    if (it != _glyphs.end())
        return &it->second;
    return nullptr;
}

SDL_Texture * GfxFont::getTexture(int fontIndex) const
{
    if (fontIndex < 0 || fontIndex >= static_cast<int>(_textures.size()))
        return nullptr;
    return _textures[fontIndex];
}
} // namespace ventty
