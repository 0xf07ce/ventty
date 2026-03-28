#include "ListView.h"

#include <algorithm>

namespace mdir::widget
{

void ListView::clear()
{
    _items.clear();
    _selectedIndex = 0;
    _scrollOffset = 0;
}

void ListView::addItem(ListItem item)
{
    _items.push_back(std::move(item));
}

void ListView::setItems(std::vector<ListItem> items)
{
    _items = std::move(items);
    _selectedIndex = std::clamp(_selectedIndex, 0,
        static_cast<int>(_items.size()) - 1);
    scrollToSelected();
}

void ListView::setSelectedIndex(int index)
{
    int const newIndex = std::clamp(index, 0,
        static_cast<int>(_items.size()) - 1);

    if (newIndex != _selectedIndex)
    {
        _selectedIndex = newIndex;
        ensureVisible(_selectedIndex);

        if (_onSelectionChanged)
        {
            _onSelectionChanged(_selectedIndex);
        }
    }
}

ListItem const * ListView::selectedItem() const
{
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_items.size()))
    {
        return &_items[_selectedIndex];
    }
    return nullptr;
}

void ListView::setScrollOffset(int offset)
{
    int const maxOffset = std::max(0,
        static_cast<int>(_items.size()) - visibleItemCount());
    _scrollOffset = std::clamp(offset, 0, maxOffset);
}

void ListView::scrollToSelected()
{
    ensureVisible(_selectedIndex);
}

int ListView::visibleItemCount() const
{
    return _rect.height;
}

void ListView::ensureVisible(int index)
{
    if (index < _scrollOffset)
    {
        _scrollOffset = index;
    }
    else if (index >= _scrollOffset + visibleItemCount())
    {
        _scrollOffset = index - visibleItemCount() + 1;
    }

    // Clamp scroll offset
    int const maxOffset = std::max(0,
        static_cast<int>(_items.size()) - visibleItemCount());
    _scrollOffset = std::clamp(_scrollOffset, 0, maxOffset);
}

void ListView::onResize()
{
    ensureVisible(_selectedIndex);
}

void ListView::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    int const visible = visibleItemCount();
    for (int i = 0; i < visible; ++i)
    {
        int const itemIndex = _scrollOffset + i;
        int const y = _rect.y + i;

        screen.setCursor(_rect.x, y);

        if (itemIndex < static_cast<int>(_items.size()))
        {
            ListItem const & item = _items[itemIndex];

            // Determine style
            Style style = item.style;
            if (itemIndex == _selectedIndex && _focused)
            {
                style = _selectedStyle;
            }
            else if (itemIndex == _selectedIndex)
            {
                // Unfocused selection - use dim highlight
                style.bg = Colors::DARK_GRAY;
            }

            screen.setStyle(style);

            // Draw item text, truncated to fit
            int col = 0;
            for (size_t j = 0; j < item.text.size() && col < _rect.width; )
            {
                unsigned char c = item.text[j];
                int charBytes = 1;
                int charWidth = 1;

                if ((c & 0x80) == 0)
                {
                    charBytes = 1;
                    charWidth = 1;
                }
                else if ((c & 0xE0) == 0xC0)
                {
                    charBytes = 2;
                    charWidth = 1;
                }
                else if ((c & 0xF0) == 0xE0)
                {
                    charBytes = 3;
                    charWidth = 2;  // CJK
                }
                else if ((c & 0xF8) == 0xF0)
                {
                    charBytes = 4;
                    charWidth = 2;
                }

                if (col + charWidth > _rect.width)
                {
                    break;
                }

                screen.putString(item.text.substr(j, charBytes));
                col += charWidth;
                j += charBytes;
            }

            // Fill remaining space
            while (col < _rect.width)
            {
                screen.putChar(U' ');
                ++col;
            }
        }
        else
        {
            // Empty row
            screen.setStyle(_normalStyle);
            for (int j = 0; j < _rect.width; ++j)
            {
                screen.putChar(U' ');
            }
        }
    }
}

bool ListView::handleKey(KeyEvent const & event)
{
    if (_items.empty())
    {
        return false;
    }

    switch (event.key)
    {
        case Key::Up:
            setSelectedIndex(_selectedIndex - 1);
            return true;

        case Key::Down:
            setSelectedIndex(_selectedIndex + 1);
            return true;

        case Key::PageUp:
            setSelectedIndex(_selectedIndex - visibleItemCount());
            return true;

        case Key::PageDown:
            setSelectedIndex(_selectedIndex + visibleItemCount());
            return true;

        case Key::Home:
            setSelectedIndex(0);
            return true;

        case Key::End:
            setSelectedIndex(static_cast<int>(_items.size()) - 1);
            return true;

        case Key::Enter:
            if (_onActivate && _selectedIndex >= 0)
            {
                _onActivate(_selectedIndex);
            }
            return true;

        default:
            break;
    }

    return false;
}

} // namespace mdir::widget
