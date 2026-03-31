// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "Dialog.h"

#include <algorithm>
#include <ranges>

namespace ventty
{
using Key = KeyEvent::Key;

Dialog::Dialog()
{}

void Dialog::setTitle(std::string title)
{
    _title = std::move(title);
}

std::string const & Dialog::title() const
{
    return _title;
}

void Dialog::setMessage(std::string message)
{
    _message = std::move(message);
}

std::string const & Dialog::message() const
{
    return _message;
}

void Dialog::clearButtons()
{
    _buttons.clear();
}

DialogResult Dialog::result() const
{
    return _result;
}

void Dialog::setResult(DialogResult r)
{
    _result = r;
}

int Dialog::selectedButton() const
{
    return _selectedButton;
}

void Dialog::addButton(std::string label, DialogResult result)
{
    _buttons.emplace_back(std::move(label), result);
}

void Dialog::setSelectedButton(int idx)
{
    if (idx >= 0 && idx < static_cast<int>(_buttons.size()))
    {
        _selectedButton = idx;
    }
}

void Dialog::autoSize(int screenWidth, int screenHeight)
{
    // Calculate minimum width based on content
    int minWidth = static_cast<int>(_title.size()) + 4;
    minWidth = std::max(minWidth, static_cast<int>(_message.size()) + 4);

    // Calculate buttons width
    int buttonsWidth = 0;
    for (auto const & label : _buttons | std::views::keys)
    {
        buttonsWidth += static_cast<int>(label.size()) + 4; // [label] + spaces
    }
    minWidth = std::max(minWidth, buttonsWidth + 2);

    // Ensure minimum and maximum width
    int width = std::clamp(minWidth, 30, screenWidth - 4);
    int height = 7; // title + border + message + border + buttons + border

    // Center on screen
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    setRect(x, y, width, height);
}

void Dialog::draw(Window & window)
{
    if (!_visible || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    // Draw dialog box with shadow effect
    Style boxStyle { Colors::BLACK, Colors::CYAN };
    Style titleStyle { Colors::WHITE, Colors::CYAN, Attr::Bold };
    Style messageStyle { Colors::BLACK, Colors::CYAN };
    Style buttonStyle { Colors::BLACK, Colors::CYAN };
    Style selectedButtonStyle { Colors::WHITE, Colors::RED };

    // Draw shadow
    Style shadowStyle { Colors::BLACK, Colors::DARK_GRAY };
    for (int y = _rect.y + 1; y < _rect.y + _rect.height + 1; ++y)
    {
        window.putChar(_rect.x + _rect.width, y, U' ', shadowStyle);
        if (y == _rect.y + _rect.height)
        {
            for (int i = 0; i < _rect.width; ++i)
            {
                window.putChar(_rect.x + 1 + i, y, U' ', shadowStyle);
            }
        }
    }

    // Draw box
    window.drawBox(_rect.x, _rect.y, _rect.width, _rect.height, boxStyle);

    // Fill interior
    for (int y = _rect.y + 1; y < _rect.y + _rect.height - 1; ++y)
    {
        for (int i = 0; i < _rect.width - 2; ++i)
        {
            window.putChar(_rect.x + 1 + i, y, U' ', boxStyle);
        }
    }

    // Draw title
    if (!_title.empty())
    {
        int titleX = _rect.x + (_rect.width - static_cast<int>(_title.size()) - 2) / 2;
        window.drawText(titleX, _rect.y, " " + _title + " ", titleStyle);
    }

    // Draw message
    int msgX = _rect.x + (_rect.width - static_cast<int>(_message.size())) / 2;
    int msgY = _rect.y + 2;
    window.drawText(msgX, msgY, _message, messageStyle);

    // Draw buttons
    int buttonsY = _rect.y + _rect.height - 2;
    int totalWidth = 0;
    for (auto const & label : _buttons | std::views::keys)
    {
        totalWidth += static_cast<int>(label.size()) + 4;
    }
    int buttonX = _rect.x + (_rect.width - totalWidth) / 2;

    for (size_t i = 0; i < _buttons.size(); ++i)
    {
        auto const & [label, _] = _buttons[i];

        Style const & btnStyle = (static_cast<int>(i) == _selectedButton)
            ? selectedButtonStyle
            : buttonStyle;

        window.drawText(buttonX, buttonsY, "[ " + label + " ]", btnStyle);
        buttonX += static_cast<int>(label.size()) + 4;
    }
}

bool Dialog::handleKey(KeyEvent const & event)
{
    switch (event.key)
    {
    case Key::Left:
    case Key::Tab:
        if (event.shift || event.key == Key::Left)
        {
            _selectedButton = (_selectedButton - 1 + static_cast<int>(_buttons.size()))
                % static_cast<int>(_buttons.size());
        }
        else
        {
            _selectedButton = (_selectedButton + 1) % static_cast<int>(_buttons.size());
        }
        return true;

    case Key::Right:
        _selectedButton = (_selectedButton + 1) % static_cast<int>(_buttons.size());
        return true;

    case Key::Enter:
        if (_selectedButton >= 0 && _selectedButton < static_cast<int>(_buttons.size()))
        {
            _result = _buttons[_selectedButton].second;
        }
        return true;

    case Key::Escape:
        _result = DialogResult::Cancel;
        return true;

    case Key::Char:
        // Check for button hotkeys (first letter)
        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            if (!_buttons[i].first.empty())
            {
                char c = std::tolower(_buttons[i].first[0]);
                if (std::tolower(event.ch) == c)
                {
                    _selectedButton = static_cast<int>(i);
                    _result = _buttons[i].second;
                    return true;
                }
            }
        }
        break;

    default:
        break;
    }

    return false;
}
} // namespace ventty
