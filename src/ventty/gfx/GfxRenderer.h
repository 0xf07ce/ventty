#pragma once

#include <ventty/gfx/GfxFont.h>

#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace ventty
{
struct Cell;
class Window;

class GfxRenderer
{
public:
    /// Default constructor
    GfxRenderer() = default;

    /// Destructor (releases SDL resources)
    ~GfxRenderer();

    GfxRenderer(GfxRenderer const &) = delete;
    GfxRenderer & operator=(GfxRenderer const &) = delete;

    /// Initialize SDL window and renderer
    bool init(int cols, int rows, int cellW, int cellH, std::string const & title, int scale = 0);

    /// Release SDL resources
    void shutdown();

    /// Begin frame rendering
    void beginFrame();

    /// Finish frame rendering and present
    void endFrame();

    /// Render a single cell at pixel coordinates
    void renderCell(ventty::Cell const & cell, int px, int py, GfxFont & font, int cellW, int cellH);

    /// Render all cells of a window
    void renderWindow(ventty::Window & win, GfxFont & font);

    /// Return SDL window pointer
    SDL_Window * sdlWindow() const;

    /// Return SDL renderer pointer
    SDL_Renderer * sdlRenderer() const;

    /// Return scale factor
    int scale() const;

    /// Update logical size
    void updateLogicalSize(int fbW, int fbH);

private:
    SDL_Window * _sdlWindow = nullptr;  ///< SDL window
    SDL_Renderer * _renderer = nullptr; ///< SDL renderer
    int _scale = 1;                     ///< Scale factor
    int _fbWidth = 0;                   ///< Framebuffer width
    int _fbHeight = 0;                  ///< Framebuffer height
    int _cellW = 8;                     ///< Cell width (pixels)
    int _cellH = 16;                    ///< Cell height (pixels)
};
} // namespace ventty
