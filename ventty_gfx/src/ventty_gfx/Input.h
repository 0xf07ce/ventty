#pragma once

#include <ventty/Terminal.h>

#include <functional>
#include <string>
#include <vector>

union SDL_Event;
struct SDL_Window;

namespace ventty::gfx
{

/// IME committed text event
struct TextInputEvent
{
    std::string text;
};

/// IME composing text event
struct TextEditingEvent
{
    std::string text;
    int cursor = 0;
    int length = 0;
};

using TextInputCallback  = std::function<void(TextInputEvent const &)>;
using TextEditingCallback = std::function<void(TextEditingEvent const &)>;

class Input
{
public:
    void setCellSize(int cellW, int cellH);
    void setScale(int scale);

    bool processEvent(SDL_Event const & event);

    void endFrame();

    // Callbacks (ventty-compatible types)
    void onKey(ventty::KeyCallback cb)     { _keyCallbacks.push_back(std::move(cb)); }
    void onMouse(ventty::MouseCallback cb) { _mouseCallbacks.push_back(std::move(cb)); }
    void onTextInput(TextInputCallback cb)   { _textInputCallbacks.push_back(std::move(cb)); }
    void onTextEditing(TextEditingCallback cb) { _textEditingCallbacks.push_back(std::move(cb)); }

    // IME
    void setWindow(SDL_Window * window);
    void startTextInput();
    void stopTextInput();
    bool isTextInputActive() const { return _textInputActive; }

    int mouseX() const { return _mouseCellX; }
    int mouseY() const { return _mouseCellY; }

private:
    std::pair<int, int> pixelToCell(float px, float py) const;

    int _cellWidth = 8;
    int _cellHeight = 16;
    int _scale = 1;
    int _mouseCellX = 0;
    int _mouseCellY = 0;

    SDL_Window * _window = nullptr;
    bool _textInputActive = false;

    std::vector<ventty::KeyCallback> _keyCallbacks;
    std::vector<ventty::MouseCallback> _mouseCallbacks;
    std::vector<TextInputCallback> _textInputCallbacks;
    std::vector<TextEditingCallback> _textEditingCallbacks;
};

} // namespace ventty::gfx
