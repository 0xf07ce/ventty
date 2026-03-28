#include "Label.h"

#include <algorithm>

namespace mdir::widget
{

Label::Label(std::string text)
    : _text(std::move(text))
{
}

void Label::setText(std::string text)
{
    _text = std::move(text);
}

void Label::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    screen.setStyle(_style);

    // Calculate text position based on alignment
    int textLen = 0;
    for (size_t i = 0; i < _text.size(); )
    {
        unsigned char c = _text[i];
        if ((c & 0x80) == 0)
        {
            ++textLen;
            ++i;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            ++textLen;
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            textLen += 2;  // CJK characters are typically double-width
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            textLen += 2;
            i += 4;
        }
        else
        {
            ++textLen;
            ++i;
        }
    }

    int startX = _rect.x;
    switch (_align)
    {
        case Align::Left:
            startX = _rect.x;
            break;
        case Align::Center:
            startX = _rect.x + (_rect.width - textLen) / 2;
            break;
        case Align::Right:
            startX = _rect.x + _rect.width - textLen;
            break;
    }

    startX = std::max(startX, _rect.x);

    // Clear the line first
    screen.setCursor(_rect.x, _rect.y);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U' ');
    }

    // Draw text
    screen.setCursor(startX, _rect.y);
    screen.putString(_text);
}

} // namespace mdir::widget
