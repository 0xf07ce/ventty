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
/// 아틀라스 좌표계 글리프 메타데이터
struct GlyphInfo
{
    int atlasX = 0;    ///< 텍스처 아틀라스에서 글리프 좌상단 X(픽셀)
    int atlasY = 0;    ///< 텍스처 아틀라스에서 글리프 좌상단 Y(픽셀)
    int width = 0;     ///< 글리프 너비(픽셀)
    int height = 0;    ///< 글리프 높이(픽셀)
    int advance = 1;   ///< 논리 셀 전진량: 1 반각, 2 전각
    int fontIndex = 0; ///< 로드된 텍스처 배열 인덱스
};

/// 다중 아틀라스 비트맵 폰트 관리자
class GfxFont
{
public:
    GfxFont() = default;
    ~GfxFont();

    GfxFont(GfxFont const &) = delete;
    GfxFont & operator=(GfxFont const &) = delete;
    GfxFont(GfxFont &&) noexcept;
    GfxFont & operator=(GfxFont &&) noexcept;

    /// 파일에서 아틀라스를 로드한다 (PNG + .cfnt 메타데이터)
    bool load(SDL_Renderer * renderer, std::string const & pngPath, std::string const & cfntPath);

    /// 내장 폰트(CP437 + 한글)를 로드한다
    bool loadBuiltin(SDL_Renderer * renderer);

    /// 캐시된 텍스처 해제
    void shutdown();

    /// 코드포인트로 글리프 메타데이터를 조회한다
    GlyphInfo const * getGlyph(char32_t codepoint) const;

    /// 로드된 폰트 인덱스로 아틀라스 텍스처를 반환한다
    SDL_Texture * getTexture(int fontIndex) const;

    /// 기본 셀 너비(픽셀, 첫 로드 폰트 기준) 반환
    int cellWidth() const { return _cellWidth; }

    /// 기본 셀 높이(픽셀, 첫 로드 폰트 기준) 반환
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
