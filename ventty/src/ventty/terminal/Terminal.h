#pragma once

#include <ventty/terminal/TerminalBase.hpp>
#include <ventty/core/Utf8.h>

#include <memory>
#include <string>
#include <vector>

namespace ventty
{

/// Real terminal backend — termios raw mode + ANSI escape sequences.
class AnsiTerminal : public TerminalBase
{
public:
    AnsiTerminal();
    ~AnsiTerminal() override;

    /// Enter raw mode, switch to alternate screen, enable mouse
    bool init();

    void shutdown() override;
    bool pollEvent() override;
    void render() override;
    void forceRedraw() override;

    // -- drawing on the root screen --

    void clear() override;
    void clear(Color bg) override;
    void putChar(int x, int y, char32_t cp) override;
    void putChar(int x, int y, char32_t cp, Style const & style) override;
    void putChar(int x, int y, char32_t cp, Color fg, Color bg,
                 Attr attr = Attr::None) override;
    void drawText(int x, int y, std::string_view text) override;
    void drawText(int x, int y, std::string_view text, Style const & style) override;
    void drawText(int x, int y, std::string_view text, Color fg, Color bg,
                  Attr attr = Attr::None) override;
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) override;
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style) override;

    void setDefaultStyle(Style const & style) override;

    // -- window management --

    Window * createWindow(int x, int y, int w, int h) override;
    void destroyWindow(Window * win) override;

    // -- queries --

    int cols() const override;
    int rows() const override;

    // -- callbacks --

    void onKey(KeyCallback cb) override;
    void onMouse(MouseCallback cb) override;
    void onResize(ResizeCallback cb) override;

    // -- direct cell access --

    Cell & cellAt(int x, int y) override;
    Cell const & cellAt(int x, int y) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

    int _cols = 0;
    int _rows = 0;

    /// Signal handler needs direct access for emergency restore
    friend void signalHandler(int);

    Style _defaultStyle;

    std::vector<Cell> _root;   ///< root (background) layer
    std::vector<Cell> _screen; ///< composited frame
    std::vector<Cell> _prev;   ///< previous frame (for diff)
    bool _fullRedraw = true;

    std::vector<std::unique_ptr<Window>> _windows;

    KeyCallback _keyCb;
    MouseCallback _mouseCb;
    ResizeCallback _resizeCb;

    void querySize();
    void composite(); ///< blit windows onto _screen
    void emitDiff();  ///< output ANSI escapes for changed cells
    void emitCell(int x, int y, Cell const & cell);
    void handleResize();
};

} // namespace ventty
