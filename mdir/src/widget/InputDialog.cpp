#include "InputDialog.h"

#include <algorithm>

namespace mdir::widget
{

InputDialog::InputDialog()
{
}

void InputDialog::setValue(std::string value)
{
    _value = std::move(value);
    _cursorPos = static_cast<int>(_value.size());
}

void InputDialog::autoSize(int screenWidth, int screenHeight)
{
    int width = std::max(40, static_cast<int>(_title.size()) + 4);
    width = std::max(width, static_cast<int>(_prompt.size()) + 4);
    width = std::min(width, screenWidth - 4);

    int height = 7;

    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    setRect(x, y, width, height);
}

void InputDialog::insertChar(char32_t ch)
{
    // Convert char32_t to UTF-8
    std::string utf8;
    if (ch < 0x80)
    {
        utf8 += static_cast<char>(ch);
    }
    else if (ch < 0x800)
    {
        utf8 += static_cast<char>(0xC0 | (ch >> 6));
        utf8 += static_cast<char>(0x80 | (ch & 0x3F));
    }
    else if (ch < 0x10000)
    {
        utf8 += static_cast<char>(0xE0 | (ch >> 12));
        utf8 += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (ch & 0x3F));
    }
    else
    {
        utf8 += static_cast<char>(0xF0 | (ch >> 18));
        utf8 += static_cast<char>(0x80 | ((ch >> 12) & 0x3F));
        utf8 += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (ch & 0x3F));
    }

    _value.insert(_cursorPos, utf8);
    _cursorPos += static_cast<int>(utf8.size());
}

void InputDialog::deleteChar()
{
    if (_cursorPos < static_cast<int>(_value.size()))
    {
        // Find the start of next character
        size_t pos = _cursorPos;
        unsigned char c = _value[pos];
        int charLen = 1;
        if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;

        _value.erase(pos, charLen);
    }
}

void InputDialog::backspace()
{
    if (_cursorPos > 0)
    {
        // Find the start of previous character
        int pos = _cursorPos - 1;
        while (pos > 0 && (static_cast<unsigned char>(_value[pos]) & 0xC0) == 0x80)
        {
            --pos;
        }

        int charLen = _cursorPos - pos;
        _value.erase(pos, charLen);
        _cursorPos = pos;
    }
}

void InputDialog::moveCursorLeft()
{
    if (_cursorPos > 0)
    {
        --_cursorPos;
        while (_cursorPos > 0 &&
               (static_cast<unsigned char>(_value[_cursorPos]) & 0xC0) == 0x80)
        {
            --_cursorPos;
        }
    }
}

void InputDialog::moveCursorRight()
{
    if (_cursorPos < static_cast<int>(_value.size()))
    {
        unsigned char c = _value[_cursorPos];
        int charLen = 1;
        if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;

        _cursorPos = std::min(_cursorPos + charLen, static_cast<int>(_value.size()));
    }
}

void InputDialog::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    Style boxStyle{Colors::BLACK, Colors::CYAN};
    Style titleStyle{Colors::WHITE, Colors::CYAN, Attr::Bold};
    Style promptStyle{Colors::BLACK, Colors::CYAN};
    Style inputStyle{Colors::BLACK, Colors::WHITE};
    Style buttonStyle{Colors::BLACK, Colors::CYAN};
    Style selectedButtonStyle{Colors::WHITE, Colors::RED};

    // Draw shadow
    screen.setStyle({Colors::BLACK, Colors::DARK_GRAY});
    for (int y = _rect.y + 1; y < _rect.y + _rect.height + 1; ++y)
    {
        screen.setCursor(_rect.x + _rect.width, y);
        screen.putChar(U' ');
        if (y == _rect.y + _rect.height)
        {
            screen.setCursor(_rect.x + 1, y);
            for (int i = 0; i < _rect.width; ++i)
            {
                screen.putChar(U' ');
            }
        }
    }

    // Draw box
    screen.setStyle(boxStyle);
    screen.drawBox(_rect.x, _rect.y, _rect.width, _rect.height);

    // Fill interior
    for (int y = _rect.y + 1; y < _rect.y + _rect.height - 1; ++y)
    {
        screen.setCursor(_rect.x + 1, y);
        for (int i = 0; i < _rect.width - 2; ++i)
        {
            screen.putChar(U' ');
        }
    }

    // Draw title
    if (!_title.empty())
    {
        screen.setStyle(titleStyle);
        int titleX = _rect.x + (_rect.width - static_cast<int>(_title.size()) - 2) / 2;
        screen.setCursor(titleX, _rect.y);
        screen.putString(" " + _title + " ");
    }

    // Draw prompt
    screen.setStyle(promptStyle);
    screen.setCursor(_rect.x + 2, _rect.y + 2);
    screen.putString(_prompt);

    // Draw input field
    int inputX = _rect.x + 2;
    int inputY = _rect.y + 3;
    int inputWidth = _rect.width - 4;

    screen.setStyle(inputStyle);
    screen.setCursor(inputX, inputY);
    for (int i = 0; i < inputWidth; ++i)
    {
        screen.putChar(U' ');
    }

    // Draw value
    screen.setCursor(inputX, inputY);
    std::string displayValue = _value;
    if (static_cast<int>(displayValue.size()) > inputWidth - 1)
    {
        displayValue = displayValue.substr(0, inputWidth - 1);
    }
    screen.putString(displayValue);

    // Draw cursor
    int cursorX = inputX;
    for (int i = 0; i < _cursorPos && i < static_cast<int>(_value.size()); )
    {
        unsigned char c = _value[i];
        int charLen = 1;
        int charWidth = 1;
        if ((c & 0xE0) == 0xC0) { charLen = 2; charWidth = 1; }
        else if ((c & 0xF0) == 0xE0) { charLen = 3; charWidth = 2; }
        else if ((c & 0xF8) == 0xF0) { charLen = 4; charWidth = 2; }
        cursorX += charWidth;
        i += charLen;
    }
    screen.setCursor(cursorX, inputY);
    screen.setStyle({Colors::WHITE, Colors::BLACK});
    if (_cursorPos < static_cast<int>(_value.size()))
    {
        // Show character under cursor with inverted colors
        unsigned char c = _value[_cursorPos];
        int charLen = 1;
        if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        screen.putString(_value.substr(_cursorPos, charLen));
    }
    else
    {
        screen.putChar(U'_');
    }

    // Draw buttons
    int buttonsY = _rect.y + _rect.height - 2;
    std::string okLabel = "[ OK ]";
    std::string cancelLabel = "[ Cancel ]";
    int totalWidth = static_cast<int>(okLabel.size() + cancelLabel.size()) + 2;
    int buttonX = _rect.x + (_rect.width - totalWidth) / 2;

    screen.setStyle(buttonStyle);
    screen.setCursor(buttonX, buttonsY);
    screen.putString(okLabel);
    buttonX += static_cast<int>(okLabel.size()) + 2;
    screen.setCursor(buttonX, buttonsY);
    screen.putString(cancelLabel);
}

bool InputDialog::handleKey(KeyEvent const & event)
{
    switch (event.key)
    {
        case Key::Enter:
            _result = DialogResult::Ok;
            return true;

        case Key::Escape:
            _result = DialogResult::Cancel;
            return true;

        case Key::Backspace:
            backspace();
            return true;

        case Key::Delete:
            deleteChar();
            return true;

        case Key::Left:
            moveCursorLeft();
            return true;

        case Key::Right:
            moveCursorRight();
            return true;

        case Key::Home:
            _cursorPos = 0;
            return true;

        case Key::End:
            _cursorPos = static_cast<int>(_value.size());
            return true;

        case Key::Char:
            if (!event.ctrl && !event.alt)
            {
                insertChar(event.ch);
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

} // namespace mdir::widget
