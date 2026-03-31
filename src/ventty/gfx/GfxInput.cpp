// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "GfxInput.h"

#include <ventty/core/Utf8.h>

#include <SDL3/SDL.h>

namespace ventty
{
void GfxInput::setCellSize(int cellW, int cellH)
{
    _cellWidth = cellW;
    _cellHeight = cellH;
}

void GfxInput::setScale(int scale)
{
    _scale = scale > 0 ? scale : 1;
}

std::pair<int, int> GfxInput::pixelToCell(float px, float py) const
{
    int cellX = static_cast<int>(px) / (_cellWidth * _scale);
    int cellY = static_cast<int>(py) / (_cellHeight * _scale);
    return { cellX, cellY };
}

static ventty::KeyEvent::Key translateSDLKey(SDL_Keycode key)
{
    using Key = ventty::KeyEvent::Key;
    switch (key)
    {
    case SDLK_UP: return Key::Up;
    case SDLK_DOWN: return Key::Down;
    case SDLK_LEFT: return Key::Left;
    case SDLK_RIGHT: return Key::Right;
    case SDLK_HOME: return Key::Home;
    case SDLK_END: return Key::End;
    case SDLK_PAGEUP: return Key::PageUp;
    case SDLK_PAGEDOWN: return Key::PageDown;
    case SDLK_INSERT: return Key::Insert;
    case SDLK_DELETE: return Key::Delete;
    case SDLK_BACKSPACE: return Key::Backspace;
    case SDLK_TAB: return Key::Tab;
    case SDLK_RETURN: return Key::Enter;
    case SDLK_ESCAPE: return Key::Escape;
    case SDLK_F1: return Key::F1;
    case SDLK_F2: return Key::F2;
    case SDLK_F3: return Key::F3;
    case SDLK_F4: return Key::F4;
    case SDLK_F5: return Key::F5;
    case SDLK_F6: return Key::F6;
    case SDLK_F7: return Key::F7;
    case SDLK_F8: return Key::F8;
    case SDLK_F9: return Key::F9;
    case SDLK_F10: return Key::F10;
    case SDLK_F11: return Key::F11;
    case SDLK_F12: return Key::F12;
    default: return Key::None;
    }
}

bool GfxInput::processEvent(SDL_Event const & event)
{
    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
        {
            ventty::KeyEvent ev;
            ev.key = translateSDLKey(event.key.key);
            ev.shift = (event.key.mod & SDL_KMOD_SHIFT) != 0;
            ev.ctrl = (event.key.mod & SDL_KMOD_CTRL) != 0;
            ev.alt = (event.key.mod & SDL_KMOD_ALT) != 0;

            // For printable keys without IME, generate Char events
            if (ev.key == ventty::KeyEvent::Key::None)
            {
                // Let TEXT_INPUT handle printable characters
                return false;
            }

            for (auto & cb : _keyCallbacks)
                cb(ev);
            return true;
        }

    case SDL_EVENT_TEXT_INPUT:
        {
            // Generate ventty::KeyEvent for each codepoint
            std::string_view text = event.text.text;
            size_t pos = 0;
            while (pos < text.size())
            {
                auto cp = ventty::decode(text, pos);
                if (cp == U'\0')
                    break;

                ventty::KeyEvent ev;
                ev.key = ventty::KeyEvent::Key::Char;
                ev.ch = cp;
                for (auto & cb : _keyCallbacks)
                    cb(ev);
            }

            // Also dispatch to text input callbacks
            TextInputEvent tie;
            tie.text = event.text.text;
            for (auto & cb : _textInputCallbacks)
                cb(tie);
            return true;
        }

    case SDL_EVENT_TEXT_EDITING:
        {
            TextEditingEvent tee;
            tee.text = event.edit.text;
            tee.cursor = event.edit.start;
            tee.length = event.edit.length;
            for (auto & cb : _textEditingCallbacks)
                cb(tee);
            return true;
        }

    case SDL_EVENT_MOUSE_MOTION:
        {
            auto [cx, cy] = pixelToCell(event.motion.x, event.motion.y);
            _mouseCellX = cx;
            _mouseCellY = cy;

            ventty::MouseEvent ev;
            ev.action = ventty::MouseEvent::Action::Move;
            ev.x = cx;
            ev.y = cy;
            for (auto & cb : _mouseCallbacks)
                cb(ev);
            return true;
        }

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            auto [cx, cy] = pixelToCell(event.button.x, event.button.y);

            ventty::MouseEvent ev;
            ev.action = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                ? ventty::MouseEvent::Action::Press
                : ventty::MouseEvent::Action::Release;
            ev.x = cx;
            ev.y = cy;

            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT: ev.button = ventty::MouseEvent::Button::Left;
                break;
            case SDL_BUTTON_MIDDLE: ev.button = ventty::MouseEvent::Button::Middle;
                break;
            case SDL_BUTTON_RIGHT: ev.button = ventty::MouseEvent::Button::Right;
                break;
            default: break;
            }

            for (auto & cb : _mouseCallbacks)
                cb(ev);
            return true;
        }

    case SDL_EVENT_MOUSE_WHEEL:
        {
            ventty::MouseEvent ev;
            ev.action = ventty::MouseEvent::Action::Press;
            ev.x = _mouseCellX;
            ev.y = _mouseCellY;
            ev.button = (event.wheel.y > 0)
                ? ventty::MouseEvent::Button::ScrollUp
                : ventty::MouseEvent::Button::ScrollDown;
            for (auto & cb : _mouseCallbacks)
                cb(ev);
            return true;
        }

    default:
        return false;
    }
}

void GfxInput::endFrame()
{
    // No per-frame state to clear for callback-based model
}

void GfxInput::setWindow(SDL_Window * window)
{
    _window = window;
}

void GfxInput::startTextInput()
{
    if (_window)
        SDL_StartTextInput(_window);
    _textInputActive = true;
}

void GfxInput::stopTextInput()
{
    if (_window)
        SDL_StopTextInput(_window);
    _textInputActive = false;
}

void GfxInput::onKey(ventty::KeyCallback cb) { _keyCallbacks.push_back(std::move(cb)); }
void GfxInput::clearKeyCallbacks() { _keyCallbacks.clear(); }
void GfxInput::clearTextEditingCallbacks() { _textEditingCallbacks.clear(); }
void GfxInput::onMouse(ventty::MouseCallback cb) { _mouseCallbacks.push_back(std::move(cb)); }
void GfxInput::onTextInput(TextInputCallback cb) { _textInputCallbacks.push_back(std::move(cb)); }
void GfxInput::onTextEditing(TextEditingCallback cb) { _textEditingCallbacks.push_back(std::move(cb)); }
bool GfxInput::isTextInputActive() const { return _textInputActive; }
int GfxInput::mouseX() const { return _mouseCellX; }
int GfxInput::mouseY() const { return _mouseCellY; }
} // namespace ventty
