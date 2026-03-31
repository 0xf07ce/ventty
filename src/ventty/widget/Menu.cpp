// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "Menu.h"

#include <algorithm>

namespace ventty
{
using Key = KeyEvent::Key;

// ============================================================================
// MenuItem
// ============================================================================

MenuItem MenuItem::makeSeparator()
{
    MenuItem item;
    item.separator = true;
    return item;
}

// ============================================================================
// Menu
// ============================================================================

Menu::Menu()
{}

void Menu::addItem(MenuItem item)
{
    _items.push_back(std::move(item));
    autoSize();
}

void Menu::addSeparator()
{
    _items.push_back(MenuItem::makeSeparator());
    autoSize();
}

void Menu::clear()
{
    _items.clear();
    _selectedIndex = 0;
}

std::vector<MenuItem> const & Menu::items() const
{
    return _items;
}

int Menu::selectedIndex() const
{
    return _selectedIndex;
}

void Menu::setSelectedIndex(int idx)
{
    if (idx >= 0 && idx < static_cast<int>(_items.size()))
    {
        _selectedIndex = idx;
    }
}

void Menu::show(int x, int y)
{
    _rect.x = x;
    _rect.y = y;
    autoSize();
    _open = true;
    _selectedIndex = nextSelectableItem(-1, 1);
}

void Menu::hide()
{
    _open = false;
}

bool Menu::isOpen() const
{
    return _open;
}

bool Menu::execute()
{
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_items.size()))
    {
        auto const & item = _items[_selectedIndex];
        if (!item.separator && item.enabled && item.action)
        {
            item.action();
            return true;
        }
    }
    return false;
}

void Menu::autoSize()
{
    if (_items.empty())
    {
        _rect.width = 10;
        _rect.height = 2;
        return;
    }

    int maxWidth = 0;
    for (auto const & item : _items)
    {
        if (!item.separator)
        {
            int width = static_cast<int>(item.label.size());
            if (!item.shortcut.empty())
            {
                width += 2 + static_cast<int>(item.shortcut.size());
            }
            maxWidth = std::max(maxWidth, width);
        }
    }

    _rect.width = maxWidth + 4;                         // padding
    _rect.height = static_cast<int>(_items.size()) + 2; // border
}

int Menu::nextSelectableItem(int from, int direction)
{
    int const count = static_cast<int>(_items.size());
    if (count == 0)
    {
        return -1;
    }

    int idx = from + direction;
    for (int i = 0; i < count; ++i)
    {
        if (idx < 0)
        {
            idx = count - 1;
        }
        else if (idx >= count)
        {
            idx = 0;
        }

        if (!_items[idx].separator && _items[idx].enabled)
        {
            return idx;
        }

        idx += direction;
    }

    return from;
}

void Menu::moveSelectionUp()
{
    _selectedIndex = nextSelectableItem(_selectedIndex, -1);
}

void Menu::moveSelectionDown()
{
    _selectedIndex = nextSelectableItem(_selectedIndex, 1);
}

void Menu::draw(Window & window)
{
    if (!_visible || !_open || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    Style boxStyle { Colors::BLACK, Colors::CYAN };
    Style itemStyle { Colors::BLACK, Colors::CYAN };
    Style selectedStyle { Colors::WHITE, Colors::BLACK };
    Style disabledStyle { Colors::DARK_GRAY, Colors::CYAN };
    Style shortcutStyle { Colors::BLACK, Colors::CYAN };

    // Draw shadow
    Style shadowStyle { Colors::BLACK, Colors::DARK_GRAY };
    for (int y = _rect.y + 1; y <= _rect.y + _rect.height; ++y)
    {
        window.putChar(_rect.x + _rect.width, y, U' ', shadowStyle);
    }
    for (int i = 0; i < _rect.width; ++i)
    {
        window.putChar(_rect.x + 1 + i, _rect.y + _rect.height, U' ', shadowStyle);
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

    // Draw items
    for (size_t i = 0; i < _items.size(); ++i)
    {
        auto const & item = _items[i];
        int const y = _rect.y + 1 + static_cast<int>(i);

        if (item.separator)
        {
            window.putChar(_rect.x, y, U'\u251C', boxStyle);
            for (int j = 0; j < _rect.width - 2; ++j)
            {
                window.putChar(_rect.x + 1 + j, y, U'\u2500', boxStyle);
            }
            window.putChar(_rect.x + _rect.width - 1, y, U'\u2524', boxStyle);
        }
        else
        {
            bool const selected = (static_cast<int>(i) == _selectedIndex);
            Style style = selected ? selectedStyle : (item.enabled ? itemStyle : disabledStyle);

            // Clear line
            for (int j = 0; j < _rect.width - 2; ++j)
            {
                window.putChar(_rect.x + 1 + j, y, U' ', style);
            }

            // Draw label
            window.drawText(_rect.x + 2, y, item.label, style);

            // Draw shortcut
            if (!item.shortcut.empty())
            {
                int shortcutX = _rect.x + _rect.width - 2 - static_cast<int>(item.shortcut.size());
                window.drawText(shortcutX, y, item.shortcut, style);
            }
        }
    }
}

bool Menu::handleKey(KeyEvent const & event)
{
    if (!_open)
    {
        return false;
    }

    switch (event.key)
    {
    case Key::Up:
        moveSelectionUp();
        return true;

    case Key::Down:
        moveSelectionDown();
        return true;

    case Key::Enter:
        if (execute())
        {
            hide();
        }
        return true;

    case Key::Escape:
        hide();
        return true;

    case Key::Char:
        // Check for item hotkey (first letter or marked with &)
        for (size_t i = 0; i < _items.size(); ++i)
        {
            auto const & item = _items[i];
            if (!item.separator && item.enabled && !item.label.empty())
            {
                char c = std::tolower(item.label[0]);
                if (std::tolower(event.ch) == c)
                {
                    _selectedIndex = static_cast<int>(i);
                    if (execute())
                    {
                        hide();
                    }
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

// ============================================================================
// MenuBar
// ============================================================================

MenuBar::MenuBar()
{}

void MenuBar::addMenu(std::string label, std::shared_ptr<Menu> menu)
{
    MenuBarEntry entry;
    entry.label = std::move(label);
    entry.menu = std::move(menu);

    // Find hotkey (first letter)
    if (!entry.label.empty())
    {
        entry.hotkey = std::tolower(entry.label[0]);
    }

    _entries.push_back(std::move(entry));
}

void MenuBar::clear()
{
    _entries.clear();
    _selectedIndex = 0;
    _active = false;
}

std::vector<MenuBarEntry> const & MenuBar::entries() const
{
    return _entries;
}

int MenuBar::selectedIndex() const
{
    return _selectedIndex;
}

void MenuBar::setSelectedIndex(int idx)
{
    if (idx >= 0 && idx < static_cast<int>(_entries.size()))
    {
        // Close current menu
        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_entries.size()))
        {
            _entries[_selectedIndex].menu->hide();
        }

        _selectedIndex = idx;

        // Open new menu if active
        if (_active && _entries[_selectedIndex].menu)
        {
            int x = _rect.x;
            for (int i = 0; i < _selectedIndex; ++i)
            {
                x += static_cast<int>(_entries[i].label.size()) + 2;
            }
            _entries[_selectedIndex].menu->show(x, _rect.y + 1);
        }
    }
}

void MenuBar::activate()
{
    _active = true;
    if (_selectedIndex < 0 && !_entries.empty())
    {
        _selectedIndex = 0;
    }
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_entries.size()))
    {
        int x = _rect.x;
        for (int i = 0; i < _selectedIndex; ++i)
        {
            x += static_cast<int>(_entries[i].label.size()) + 2;
        }
        _entries[_selectedIndex].menu->show(x, _rect.y + 1);
    }
}

void MenuBar::deactivate()
{
    _active = false;
    for (auto & entry : _entries)
    {
        if (entry.menu)
        {
            entry.menu->hide();
        }
    }
}

bool MenuBar::isActive() const
{
    return _active;
}

Menu * MenuBar::currentMenu()
{
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_entries.size()))
    {
        return _entries[_selectedIndex].menu.get();
    }
    return nullptr;
}

void MenuBar::draw(Window & window)
{
    if (!_visible || _rect.width <= 0)
    {
        return;
    }

    Style barStyle { Colors::BLACK, Colors::CYAN };
    Style selectedStyle { Colors::WHITE, Colors::BLACK };
    Style hotkeyStyle { Colors::RED, Colors::CYAN };
    Style hotkeySelectedStyle { Colors::LIGHT_RED, Colors::BLACK };

    // Clear bar
    for (int i = 0; i < _rect.width; ++i)
    {
        window.putChar(_rect.x + i, _rect.y, U' ', barStyle);
    }

    // Draw entries
    int x = _rect.x + 1;
    for (size_t i = 0; i < _entries.size(); ++i)
    {
        auto const & entry = _entries[i];
        bool const selected = (_active && static_cast<int>(i) == _selectedIndex);

        Style const & baseStyle = selected ? selectedStyle : barStyle;

        window.putChar(x, _rect.y, U' ', baseStyle);
        int cx = x + 1;

        // Draw label with hotkey highlighted
        for (size_t j = 0; j < entry.label.size(); ++j)
        {
            if (j == 0)
            {
                // Hotkey
                window.putChar(cx, _rect.y, entry.label[j],
                    selected ? hotkeySelectedStyle : hotkeyStyle);
            }
            else
            {
                window.putChar(cx, _rect.y, entry.label[j], baseStyle);
            }
            ++cx;
        }

        window.putChar(cx, _rect.y, U' ', baseStyle);

        x += static_cast<int>(entry.label.size()) + 2;
    }

    // Draw current menu
    if (_active && currentMenu() && currentMenu()->isOpen())
    {
        currentMenu()->draw(window);
    }
}

bool MenuBar::handleKey(KeyEvent const & event)
{
    if (!_active)
    {
        // Check for Alt+key
        if (event.alt && event.key == Key::Char)
        {
            for (size_t i = 0; i < _entries.size(); ++i)
            {
                if (std::tolower(event.ch) == _entries[i].hotkey)
                {
                    _selectedIndex = static_cast<int>(i);
                    activate();
                    return true;
                }
            }
        }

        // F12 activates menu
        if (event.key == Key::F12)
        {
            activate();
            return true;
        }

        return false;
    }

    // Forward to current menu first
    if (currentMenu() && currentMenu()->isOpen())
    {
        if (currentMenu()->handleKey(event))
        {
            if (!currentMenu()->isOpen())
            {
                deactivate();
            }
            return true;
        }
    }

    switch (event.key)
    {
    case Key::Left:
        setSelectedIndex((_selectedIndex - 1 + static_cast<int>(_entries.size()))
            % static_cast<int>(_entries.size()));
        return true;

    case Key::Right:
        setSelectedIndex((_selectedIndex + 1) % static_cast<int>(_entries.size()));
        return true;

    case Key::Escape:
        deactivate();
        return true;

    case Key::F12:
        deactivate();
        return true;

    default:
        break;
    }

    return false;
}
} // namespace ventty
