#pragma once

#include <ventty/terminal/TerminalBase.h>

#include <functional>
#include <string>
#include <vector>

union SDL_Event;
struct SDL_Window;

namespace ventty
{
/// IME committed text event
struct TextInputEvent
{
    std::string text; ///< Committed text
};

/// IME composing text event
struct TextEditingEvent
{
    std::string text; ///< Composing text
    int cursor = 0;   ///< Composing cursor position
    int length = 0;   ///< Composing text length
};

/// Text input callback type
using TextInputCallback = std::function<void(TextInputEvent const &)>;

/// Text editing callback type
using TextEditingCallback = std::function<void(TextEditingEvent const &)>;

/// Input handler that converts SDL events to ventty events
class GfxInput
{
public:
    /// Set cell size (pixels)
    void setCellSize(int cellW, int cellH);

    /// Set scale factor
    void setScale(int scale);

    /// Process SDL event (returns true if handled)
    bool processEvent(SDL_Event const & event);

    /// Called at end of frame
    void endFrame();

    // Callbacks (ventty-compatible types)

    /// Register key event callback
    void onKey(ventty::KeyCallback cb);

    /// Remove key event callbacks
    void clearKeyCallbacks();

    /// Remove text editing callbacks
    void clearTextEditingCallbacks();

    /// Register mouse event callback
    void onMouse(ventty::MouseCallback cb);

    /// Register text input callback
    void onTextInput(TextInputCallback cb);

    /// Register text editing callback
    void onTextEditing(TextEditingCallback cb);

    // IME

    /// Set SDL window (for IME)
    void setWindow(SDL_Window * window);

    /// Start text input (IME)
    void startTextInput();

    /// Stop text input (IME)
    void stopTextInput();

    /// Check if text input is active
    bool isTextInputActive() const;

    /// Return mouse cell X coordinate
    int mouseX() const;

    /// Return mouse cell Y coordinate
    int mouseY() const;

private:
    /// Convert pixel coordinates to cell coordinates
    std::pair<int, int> pixelToCell(float px, float py) const;

    int _cellWidth = 8;   ///< Cell width (pixels)
    int _cellHeight = 16; ///< Cell height (pixels)
    int _scale = 1;       ///< Scale factor
    int _mouseCellX = 0;  ///< Mouse cell X coordinate
    int _mouseCellY = 0;  ///< Mouse cell Y coordinate

    SDL_Window * _window = nullptr; ///< SDL window
    bool _textInputActive = false;  ///< Whether text input is active

    std::vector<ventty::KeyCallback> _keyCallbacks;         ///< Key event callback list
    std::vector<ventty::MouseCallback> _mouseCallbacks;     ///< Mouse event callback list
    std::vector<TextInputCallback> _textInputCallbacks;     ///< Text input callback list
    std::vector<TextEditingCallback> _textEditingCallbacks; ///< Text editing callback list
};
} // namespace ventty
