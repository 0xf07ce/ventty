#pragma once

#include "Font.h"
#include "Input.h"
#include "Renderer.h"

#include <ventty/Cell.h>
#include <ventty/Color.h>
#include <ventty/Window.h>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ventty::gfx
{
class Terminal
{
public:
    Terminal();
    ~Terminal();

    Terminal(Terminal const &) = delete;
    Terminal & operator=(Terminal const &) = delete;

    /// Initialize SDL3 window, renderer, and TTF font
    bool init(int cols, int rows, std::string const & title, std::string const & fontPath, float fontSize, int scale = 0);

    void shutdown();

    /// Poll one SDL event (non-blocking). Returns true if event was processed.
    bool pollEvent();

    /// Render all windows to the SDL window
    void render();

    /// Force full redraw on next render()
    void forceRedraw();

    // -- drawing on root screen --

    void clear();
    void clear(ventty::Color bg);
    void putChar(int x, int y, char32_t cp);
    void putChar(int x, int y, char32_t cp, ventty::Style const & style);
    void putChar(int x, int y, char32_t cp, ventty::Color fg, ventty::Color bg, ventty::Attr attr = ventty::Attr::None);
    void drawText(int x, int y, std::string_view text);
    void drawText(int x, int y, std::string_view text, ventty::Style const & style);
    void drawText(int x, int y, std::string_view text, ventty::Color fg, ventty::Color bg, ventty::Attr attr = ventty::Attr::None);
    void fill(int x, int y, int w, int h, char32_t cp, ventty::Color fg, ventty::Color bg);
    void fill(int x, int y, int w, int h, char32_t cp, ventty::Style const & style);

    void setDefaultStyle(ventty::Style const & style);

    // -- window management --

    ventty::Window * createWindow(int x, int y, int w, int h);
    void destroyWindow(ventty::Window * win);

    // -- queries --

    int cols() const { return _cols; }
    int rows() const { return _rows; }
    bool isRunning() const { return _running; }
    void quit() { _running = false; }

    int cellWidth() const { return _font.cellWidth(); }
    int cellHeight() const { return _font.cellHeight(); }

    // -- callbacks --

    void onKey(ventty::KeyCallback cb) { _input.onKey(std::move(cb)); }
    void onMouse(ventty::MouseCallback cb) { _input.onMouse(std::move(cb)); }
    void onResize(ventty::ResizeCallback cb) { _resizeCb = std::move(cb); }
    void onTextInput(TextInputCallback cb) { _input.onTextInput(std::move(cb)); }
    void onTextEditing(TextEditingCallback cb) { _input.onTextEditing(std::move(cb)); }

    // -- IME --

    void startTextInput() { _input.startTextInput(); }
    void stopTextInput() { _input.stopTextInput(); }

    // -- direct cell access --

    ventty::Cell & cellAt(int x, int y);
    ventty::Cell const & cellAt(int x, int y) const;

    /// Executable base path (SDL_GetBasePath wrapper)
    static std::string basePath();

private:
    int _cols = 80;
    int _rows = 25;
    bool _running = false;

    ventty::Style _defaultStyle;

    Font _font;
    Renderer _renderer;
    Input _input;

    std::unique_ptr<ventty::Window> _rootScreen;
    std::vector<std::unique_ptr<ventty::Window>> _windows;

    ventty::ResizeCallback _resizeCb;
};
} // namespace ventty::gfx
