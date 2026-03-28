#pragma once

#include "Font.h"
#include "Input.h"
#include "GfxRenderer.h"

#include <ventty/Terminal.h>
#include <ventty/Window.h>

#include <memory>
#include <string>
#include <vector>

namespace ventty::gfx
{

class GfxTerminal : public ventty::Terminal
{
public:
    GfxTerminal();
    ~GfxTerminal() override;

    /// Initialize SDL3 window, renderer, and TTF font
    bool init(int cols, int rows, std::string const & title,
              std::string const & fontPath, float fontSize, int scale = 0);

    void shutdown() override;
    bool pollEvent() override;
    void render() override;
    void forceRedraw() override;

    // -- drawing on root screen --

    void clear() override;
    void clear(ventty::Color bg) override;
    void putChar(int x, int y, char32_t cp) override;
    void putChar(int x, int y, char32_t cp, ventty::Style const & style) override;
    void putChar(int x, int y, char32_t cp, ventty::Color fg, ventty::Color bg,
                 ventty::Attr attr = ventty::Attr::None) override;
    void drawText(int x, int y, std::string_view text) override;
    void drawText(int x, int y, std::string_view text, ventty::Style const & style) override;
    void drawText(int x, int y, std::string_view text, ventty::Color fg, ventty::Color bg,
                  ventty::Attr attr = ventty::Attr::None) override;
    void fill(int x, int y, int w, int h, char32_t cp, ventty::Color fg, ventty::Color bg) override;
    void fill(int x, int y, int w, int h, char32_t cp, ventty::Style const & style) override;

    void setDefaultStyle(ventty::Style const & style) override;

    // -- window management --

    ventty::Window * createWindow(int x, int y, int w, int h) override;
    void destroyWindow(ventty::Window * win) override;

    // -- queries --

    int cols() const override { return _cols; }
    int rows() const override { return _rows; }

    int cellWidth() const { return _font.cellWidth(); }
    int cellHeight() const { return _font.cellHeight(); }

    // -- callbacks --

    void onKey(ventty::KeyCallback cb) override { _input.onKey(std::move(cb)); }
    void onMouse(ventty::MouseCallback cb) override { _input.onMouse(std::move(cb)); }
    void onResize(ventty::ResizeCallback cb) override { _resizeCb = std::move(cb); }
    void onTextInput(TextInputCallback cb) { _input.onTextInput(std::move(cb)); }
    void onTextEditing(TextEditingCallback cb) { _input.onTextEditing(std::move(cb)); }

    // -- IME --

    void startTextInput() { _input.startTextInput(); }
    void stopTextInput() { _input.stopTextInput(); }

    // -- direct cell access --

    ventty::Cell & cellAt(int x, int y) override;
    ventty::Cell const & cellAt(int x, int y) const override;

    /// Executable base path (SDL_GetBasePath wrapper)
    static std::string basePath();

private:
    int _cols = 80;
    int _rows = 25;

    ventty::Style _defaultStyle;

    Font _font;
    GfxRenderer _renderer;
    Input _input;

    std::unique_ptr<ventty::Window> _rootScreen;
    std::vector<std::unique_ptr<ventty::Window>> _windows;

    ventty::ResizeCallback _resizeCb;
};

} // namespace ventty::gfx
