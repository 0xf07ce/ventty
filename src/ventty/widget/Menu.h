// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#ifndef VENTTY_WIDGET_MENU_H
#define VENTTY_WIDGET_MENU_H

#include "Widget.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ventty
{
/// Menu item
struct MenuItem
{
    std::string label;                       ///< Item label
    std::string shortcut;                    ///< Shortcut display text
    KeyEvent::Key key = KeyEvent::Key::None; ///< Associated key
    bool separator = false;                  ///< Whether this is a separator
    bool enabled = true;                     ///< Whether enabled
    std::function<void()> action;            ///< Action to execute on selection

    /// Create a separator item
    static MenuItem makeSeparator();
};

/// Popup menu widget
class Menu : public Widget
{
public:
    /// Constructor
    Menu();

    /// Add a menu item
    void addItem(MenuItem item);

    /// Add a separator
    void addSeparator();

    /// Remove all items
    void clear();

    /// Return the list of items
    std::vector<MenuItem> const & items() const;

    /// Return the selected item index
    int selectedIndex() const;

    /// Set the selected index
    void setSelectedIndex(int idx);

    /// Show the menu at the specified position
    void show(int x, int y);

    /// Hide the menu
    void hide() override;

    /// Check whether the menu is open
    bool isOpen() const;

    /// Execute the selected item
    bool execute();

    /// Draw the menu to a window
    void draw(Window & window) override;

    /// Handle a key event
    bool handleKey(KeyEvent const & event) override;

    /// Auto-size to fit content
    void autoSize();

private:
    /// Move selection up
    void moveSelectionUp();

    /// Move selection down
    void moveSelectionDown();

    /// Return the next selectable item index
    int nextSelectableItem(int from, int direction);

    std::vector<MenuItem> _items; ///< List of menu items
    int _selectedIndex = 0;       ///< Selected item index
    bool _open = false;           ///< Whether the menu is open
};

/// Menu bar entry
struct MenuBarEntry
{
    std::string label;          ///< Label displayed on the menu bar
    char hotkey = 0;            ///< Alt+key shortcut (0 if none)
    std::shared_ptr<Menu> menu; ///< Associated menu
};

/// Top-of-screen menu bar widget
class MenuBar : public Widget
{
public:
    /// Constructor
    MenuBar();

    /// Add a menu with the given label
    void addMenu(std::string label, std::shared_ptr<Menu> menu);

    /// Remove all entries
    void clear();

    /// Return the list of menu bar entries
    std::vector<MenuBarEntry> const & entries() const;

    /// Return the selected entry index
    int selectedIndex() const;

    /// Set the selected index
    void setSelectedIndex(int idx);

    /// Activate the menu bar
    void activate();

    /// Deactivate the menu bar
    void deactivate();

    /// Check whether the menu bar is active
    bool isActive() const;

    /// Return the currently selected menu
    Menu * currentMenu();

    /// Draw the menu bar to a window
    void draw(Window & window) override;

    /// Handle a key event
    bool handleKey(KeyEvent const & event) override;

private:
    std::vector<MenuBarEntry> _entries; ///< List of menu bar entries
    int _selectedIndex = 0;             ///< Selected entry index
    bool _active = false;               ///< Whether active
};
} // namespace ventty

#endif // VENTTY_WIDGET_MENU_H
