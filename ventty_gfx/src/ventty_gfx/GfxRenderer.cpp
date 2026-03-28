#include "GfxRenderer.h"

#include <ventty/Cell.h>
#include <ventty/Window.h>

#include <SDL3/SDL.h>

#include <algorithm>

namespace ventty::gfx
{

GfxRenderer::~GfxRenderer()
{
    shutdown();
}

bool GfxRenderer::init(int cols, int rows, int cellW, int cellH,
                    std::string const & title, int requestedScale)
{
    _cellW = cellW;
    _cellH = cellH;
    _fbWidth = cols * cellW;
    _fbHeight = rows * cellH;

    if (requestedScale > 0)
    {
        _scale = requestedScale;
    }
    else
    {
        SDL_DisplayID display = SDL_GetPrimaryDisplay();
        SDL_DisplayMode const * mode = SDL_GetCurrentDisplayMode(display);
        if (mode)
        {
            int maxScaleX = mode->w / _fbWidth;
            int maxScaleY = mode->h / _fbHeight;
            _scale = std::min(maxScaleX, maxScaleY);
            if (_scale < 1)
                _scale = 1;
            if (_scale > 4)
                _scale = 4;
        }
    }

    int winW = _fbWidth * _scale;
    int winH = _fbHeight * _scale;

    _sdlWindow = SDL_CreateWindow(title.c_str(), winW, winH, 0);
    if (!_sdlWindow)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to create window: %s", SDL_GetError());
        return false;
    }

    _renderer = SDL_CreateRenderer(_sdlWindow, nullptr);
    if (!_renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(_sdlWindow);
        _sdlWindow = nullptr;
        return false;
    }

    SDL_SetRenderLogicalPresentation(_renderer, _fbWidth, _fbHeight,
        SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    return true;
}

void GfxRenderer::shutdown()
{
    if (_renderer)
    {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_sdlWindow)
    {
        SDL_DestroyWindow(_sdlWindow);
        _sdlWindow = nullptr;
    }
}

void GfxRenderer::updateLogicalSize(int fbW, int fbH)
{
    _fbWidth = fbW;
    _fbHeight = fbH;
    if (_renderer)
    {
        SDL_SetRenderLogicalPresentation(_renderer, fbW, fbH,
            SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    }
}

void GfxRenderer::beginFrame()
{
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
}

void GfxRenderer::endFrame()
{
    SDL_RenderPresent(_renderer);
}

void GfxRenderer::renderCell(ventty::Cell const & cell, int px, int py,
                          Font & font, int cellW, int cellH)
{
    ventty::Color fg = cell.style.fg;
    ventty::Color bg = cell.style.bg;

    if (ventty::hasAttr(cell.style.attr, ventty::Attr::Reverse))
        std::swap(fg, bg);

    // Skip continuation cells
    if (cell.ch == U'\0')
        return;

    // Background width: fullwidth = 2 cells
    int bgWidth = cellW;
    if (cell.width == 2)
        bgWidth = cellW * 2;

    // Draw background
    SDL_FRect bgRect = {
        static_cast<float>(px),
        static_cast<float>(py),
        static_cast<float>(bgWidth),
        static_cast<float>(cellH)
    };
    SDL_SetRenderDrawColor(_renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderFillRect(_renderer, &bgRect);

    // Skip glyph for space
    if (cell.ch == U' ')
        return;

    GlyphEntry const * glyph = font.getGlyph(cell.ch);
    if (!glyph || !glyph->texture)
        return;

    // Color modulation
    SDL_SetTextureColorMod(glyph->texture, fg.r, fg.g, fg.b);
    SDL_SetTextureAlphaMod(glyph->texture, 255);

    SDL_FRect dstRect = {
        static_cast<float>(px),
        static_cast<float>(py),
        static_cast<float>(glyph->width),
        static_cast<float>(glyph->height)
    };

    SDL_RenderTexture(_renderer, glyph->texture, nullptr, &dstRect);

    // Underline
    if (ventty::hasAttr(cell.style.attr, ventty::Attr::Underline))
    {
        SDL_SetRenderDrawColor(_renderer, fg.r, fg.g, fg.b, 255);
        SDL_RenderLine(_renderer,
            static_cast<float>(px),
            static_cast<float>(py + cellH - 1),
            static_cast<float>(px + bgWidth - 1),
            static_cast<float>(py + cellH - 1));
    }

    // Strikethrough
    if (ventty::hasAttr(cell.style.attr, ventty::Attr::Strike))
    {
        SDL_SetRenderDrawColor(_renderer, fg.r, fg.g, fg.b, 255);
        int midY = py + cellH / 2;
        SDL_RenderLine(_renderer,
            static_cast<float>(px),
            static_cast<float>(midY),
            static_cast<float>(px + bgWidth - 1),
            static_cast<float>(midY));
    }
}

void GfxRenderer::renderWindow(ventty::Window & win, Font & font)
{
    int cellW = font.cellWidth();
    int cellH = font.cellHeight();

    for (int row = 0; row < win.height(); ++row)
    {
        for (int col = 0; col < win.width(); ++col)
        {
            ventty::Cell const & cell = win.cellAt(col, row);
            int px = (win.x() + col) * cellW;
            int py = (win.y() + row) * cellH;
            renderCell(cell, px, py, font, cellW, cellH);
        }
    }
    win.clearDirty();
}

} // namespace ventty::gfx
