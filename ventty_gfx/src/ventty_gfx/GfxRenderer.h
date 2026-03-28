#pragma once

#include "Font.h"

#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace ventty { struct Cell; class Window; }

namespace ventty::gfx
{

class GfxRenderer
{
public:
    GfxRenderer() = default;
    ~GfxRenderer();

    GfxRenderer(GfxRenderer const &) = delete;
    GfxRenderer & operator=(GfxRenderer const &) = delete;

    bool init(int cols, int rows, int cellW, int cellH,
              std::string const & title, int scale = 0);
    void shutdown();

    void beginFrame();
    void endFrame();

    void renderCell(ventty::Cell const & cell, int px, int py, Font & font, int cellW, int cellH);
    void renderWindow(ventty::Window & win, Font & font);

    SDL_Window * sdlWindow() const { return _sdlWindow; }
    SDL_Renderer * sdlRenderer() const { return _renderer; }
    int scale() const { return _scale; }

    void updateLogicalSize(int fbW, int fbH);

private:
    SDL_Window * _sdlWindow = nullptr;
    SDL_Renderer * _renderer = nullptr;
    int _scale = 1;
    int _fbWidth = 0;
    int _fbHeight = 0;
    int _cellW = 8;
    int _cellH = 16;
};

} // namespace ventty::gfx
