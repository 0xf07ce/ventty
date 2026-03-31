// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "GfxTerminal.h"

#include <ventty/core/Utf8.h>

#include <SDL3/SDL.h>

#include <algorithm>

namespace ventty
{
GfxTerminal::GfxTerminal() = default;

GfxTerminal::~GfxTerminal()
{
    shutdown();
}

std::string GfxTerminal::basePath()
{
    char const * p = SDL_GetBasePath();
    return p ? std::string(p) : std::string("./");
}

bool GfxTerminal::init(int cols, int rows, std::string const & title, int scale)
{
    _cols = cols;
    _rows = rows;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Initialize renderer with default cell size (updated on first loadFont)
    if (!_renderer.init(cols, rows, _font.cellWidth(), _font.cellHeight(), title, scale))
        return false;

    // Create root screen
    _rootScreen = std::make_unique<ventty::Window>(0, 0, cols, rows);

    // Configure input with default cell size
    _input.setCellSize(_font.cellWidth(), _font.cellHeight());
    _input.setScale(_renderer.scale());
    _input.setWindow(_renderer.sdlWindow());

    _running = true;
    return true;
}

bool GfxTerminal::loadFont(std::string const & pngPath, std::string const & fntPath)
{
    if (!_font.load(_renderer.sdlRenderer(), pngPath, fntPath))
        return false;

    if (!_fontLoaded)
    {
        _fontLoaded = true;
        _renderer.updateLogicalSize(_cols * _font.cellWidth(), _rows * _font.cellHeight());
        _input.setCellSize(_font.cellWidth(), _font.cellHeight());
    }

    return true;
}

bool GfxTerminal::loadBuiltinFont()
{
    if (!_font.loadBuiltin(_renderer.sdlRenderer()))
        return false;

    if (!_fontLoaded)
    {
        _fontLoaded = true;
        _renderer.updateLogicalSize(_cols * _font.cellWidth(), _rows * _font.cellHeight());
        _input.setCellSize(_font.cellWidth(), _font.cellHeight());
    }

    return true;
}

void GfxTerminal::shutdown()
{
    _windows.clear();
    _rootScreen.reset();
    _font.shutdown();     // destroy textures while renderer is still valid
    _renderer.shutdown(); // destroy SDL renderer / window
    SDL_Quit();
    _running = false;
}

bool GfxTerminal::pollEvent()
{
    SDL_Event event;
    if (!SDL_PollEvent(&event))
        return false;

    if (event.type == SDL_EVENT_QUIT)
    {
        _running = false;
        return true;
    }

    _input.processEvent(event);
    return true;
}

void GfxTerminal::render()
{
    _renderer.beginFrame();

    if (_rootScreen)
        _renderer.renderWindow(*_rootScreen, _font);

    // Sort windows by z-order and render visible ones
    std::vector<ventty::Window *> sorted;
    for (auto & w : _windows)
    {
        if (w->visible())
            sorted.push_back(w.get());
    }
    std::sort(sorted.begin(), sorted.end(),
        [](ventty::Window const * a, ventty::Window const * b)
        {
            return a->zOrder() < b->zOrder();
        });

    for (auto * win : sorted)
        _renderer.renderWindow(*win, _font);

    _renderer.endFrame();
    _input.endFrame();
}

void GfxTerminal::forceRedraw()
{
    if (_rootScreen)
        _rootScreen->markDirty();
    for (auto & w : _windows)
        w->markDirty();
}

// -- drawing delegates to root screen --

void GfxTerminal::clear()
{
    if (_rootScreen)
        _rootScreen->clear();
}

void GfxTerminal::clear(ventty::Color bg)
{
    if (_rootScreen)
        _rootScreen->clear(bg);
}

void GfxTerminal::setDefaultStyle(ventty::Style const & style)
{
    _defaultStyle = style;
}

void GfxTerminal::putChar(int x, int y, char32_t cp)
{
    if (_rootScreen)
        _rootScreen->putChar(x, y, cp, _defaultStyle);
}

void GfxTerminal::putChar(int x, int y, char32_t cp, ventty::Style const & style)
{
    if (_rootScreen)
        _rootScreen->putChar(x, y, cp, style);
}

void GfxTerminal::putChar(int x, int y, char32_t cp, ventty::Color fg, ventty::Color bg, ventty::Attr attr)
{
    if (_rootScreen)
        _rootScreen->putChar(x, y, cp, fg, bg, attr);
}

void GfxTerminal::drawText(int x, int y, std::string_view text)
{
    if (_rootScreen)
        _rootScreen->drawText(x, y, text, _defaultStyle);
}

void GfxTerminal::drawText(int x, int y, std::string_view text, ventty::Style const & style)
{
    if (_rootScreen)
        _rootScreen->drawText(x, y, text, style);
}

void GfxTerminal::drawText(int x, int y, std::string_view text, ventty::Color fg, ventty::Color bg, ventty::Attr attr)
{
    if (_rootScreen)
        _rootScreen->drawText(x, y, text, fg, bg, attr);
}

void GfxTerminal::fill(int x, int y, int w, int h, char32_t cp, ventty::Color fg, ventty::Color bg)
{
    if (_rootScreen)
        _rootScreen->fill(x, y, w, h, cp, fg, bg);
}

void GfxTerminal::fill(int x, int y, int w, int h, char32_t cp, ventty::Style const & style)
{
    if (_rootScreen)
        _rootScreen->fill(x, y, w, h, cp, style);
}

ventty::Window * GfxTerminal::createWindow(int x, int y, int w, int h)
{
    _windows.push_back(std::make_unique<ventty::Window>(x, y, w, h));
    return _windows.back().get();
}

void GfxTerminal::destroyWindow(ventty::Window * win)
{
    _windows.erase(
        std::remove_if(_windows.begin(), _windows.end(),
            [win](auto const & w) { return w.get() == win; }),
        _windows.end());
}

ventty::Cell & GfxTerminal::cellAt(int x, int y)
{
    return _rootScreen->cellAt(x, y);
}

ventty::Cell const & GfxTerminal::cellAt(int x, int y) const
{
    return _rootScreen->cellAt(x, y);
}

int GfxTerminal::cols() const { return _cols; }
int GfxTerminal::rows() const { return _rows; }
int GfxTerminal::cellWidth() const { return _font.cellWidth(); }
int GfxTerminal::cellHeight() const { return _font.cellHeight(); }
void GfxTerminal::onKey(ventty::KeyCallback cb) { _input.clearKeyCallbacks(); _input.onKey(std::move(cb)); }
void GfxTerminal::onMouse(ventty::MouseCallback cb) { _input.onMouse(std::move(cb)); }
void GfxTerminal::onResize(ventty::ResizeCallback cb) { _resizeCb = std::move(cb); }
void GfxTerminal::onTextInput(TextInputCallback cb) { _input.onTextInput(std::move(cb)); }
void GfxTerminal::onTextEditing(TextEditingCallback cb) { _input.clearTextEditingCallbacks(); _input.onTextEditing(std::move(cb)); }
void GfxTerminal::startTextInput() { _input.startTextInput(); }
void GfxTerminal::stopTextInput() { _input.stopTextInput(); }
} // namespace ventty
