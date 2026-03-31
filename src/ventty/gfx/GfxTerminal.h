#pragma once

#include <ventty/gfx/GfxFont.h>
#include <ventty/gfx/GfxInput.h>
#include <ventty/gfx/GfxRenderer.h>

#include <ventty/terminal/TerminalBase.h>
#include <ventty/core/Window.h>

#include <memory>
#include <string>
#include <vector>

namespace ventty
{
class GfxTerminal : public TerminalBase
{
public:
    /// Constructor
    GfxTerminal();

    /// Destructor
    ~GfxTerminal() override;

    /// Initialize SDL3 window and renderer
    bool init(int cols, int rows, std::string const & title, int scale = 0);

    /// Load atlas font (multiple calls supported for multiple atlases)
    bool loadFont(std::string const & pngPath, std::string const & fntPath);

    /// Load built-in font (CP437 + Hangul)
    bool loadBuiltinFont();

    /// Shut down and release resources
    void shutdown() override;

    /// Poll events
    bool pollEvent() override;

    /// Render screen
    void render() override;

    /// Force full screen redraw
    void forceRedraw() override;

    // -- drawing on root screen --

    /// Clear root screen with default style
    void clear() override;

    /// Clear root screen with specified background color
    void clear(Color bg) override;

    /// Put character with default style
    void putChar(int x, int y, char32_t cp) override;

    /// Put character with style
    void putChar(int x, int y, char32_t cp, Style const & style) override;

    /// Put character with foreground, background, and attributes
    void putChar(int x, int y, char32_t cp, Color fg, Color bg, Attr attr = Attr::None) override;

    /// Draw text with default style
    void drawText(int x, int y, std::string_view text) override;

    /// Draw text with style
    void drawText(int x, int y, std::string_view text, Style const & style) override;

    /// Draw text with foreground, background, and attributes
    void drawText(int x, int y, std::string_view text, Color fg, Color bg, Attr attr = Attr::None) override;

    /// Fill region with foreground and background
    void fill(int x, int y, int w, int h, char32_t cp, Color fg, Color bg) override;

    /// Fill region with style
    void fill(int x, int y, int w, int h, char32_t cp, Style const & style) override;

    /// Set default style
    void setDefaultStyle(Style const & style) override;

    // -- window management --

    /// Create a window
    Window * createWindow(int x, int y, int w, int h) override;

    /// Destroy a window
    void destroyWindow(Window * win) override;

    // -- queries --

    /// Return number of terminal columns
    int cols() const override;

    /// Return number of terminal rows
    int rows() const override;

    /// Return cell width (pixels)
    int cellWidth() const;

    /// Return cell height (pixels)
    int cellHeight() const;

    // -- callbacks --

    /// Register key event callback
    void onKey(KeyCallback cb) override;

    /// Register mouse event callback
    void onMouse(MouseCallback cb) override;

    /// Register resize event callback
    void onResize(ResizeCallback cb) override;

    /// Register text input (IME committed) callback
    void onTextInput(TextInputCallback cb);

    /// Register text editing (IME composing) callback
    void onTextEditing(TextEditingCallback cb);

    // -- IME --

    /// Start text input (IME)
    void startTextInput();

    /// Stop text input (IME)
    void stopTextInput();

    // -- direct cell access --

    /// Return cell reference at specified coordinates
    Cell & cellAt(int x, int y) override;

    /// Return const cell reference at specified coordinates
    Cell const & cellAt(int x, int y) const override;

    /// Return executable base path (SDL_GetBasePath wrapper)
    static std::string basePath();

private:
    int _cols = 80; ///< Number of terminal columns
    int _rows = 25; ///< Number of terminal rows

    Style _defaultStyle; ///< Default style

    GfxFont _font;         ///< Bitmap atlas font
    GfxRenderer _renderer; ///< SDL renderer
    GfxInput _input;       ///< Input handler

    bool _fontLoaded = false; ///< Whether font is loaded

    std::unique_ptr<Window> _rootScreen;           ///< Root screen window
    std::vector<std::unique_ptr<Window>> _windows; ///< Managed window list

    ResizeCallback _resizeCb; ///< Resize callback
};
} // namespace ventty
