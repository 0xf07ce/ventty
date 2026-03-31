// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#ifndef VENTTY_WIDGET_LISTVIEW_H
#define VENTTY_WIDGET_LISTVIEW_H

#include "Widget.h"

#include <functional>
#include <string>
#include <vector>

namespace ventty
{
/// List view item
struct ListItem
{
    std::string text;          ///< Item text
    Style style;               ///< Item style
    void * userData = nullptr; ///< User data pointer
};

/// Scrollable item list widget
class ListView : public Widget
{
public:
    ListView() = default;

    // Items

    /// Remove all items
    void clear();

    /// Add an item
    void addItem(ListItem item);

    /// Set all items at once
    void setItems(std::vector<ListItem> items);

    /// Return the item list
    std::vector<ListItem> const & items() const;

    /// Return the item count
    size_t itemCount() const;

    // Selection

    /// Return the selected item index
    int selectedIndex() const;

    /// Set the selected index
    void setSelectedIndex(int index);

    /// Return the selected item pointer (nullptr if none)
    ListItem const * selectedItem() const;

    // Scrolling

    /// Return the scroll offset
    int scrollOffset() const;

    /// Set the scroll offset
    void setScrollOffset(int offset);

    /// Scroll to make the selected item visible
    void scrollToSelected();

    // Style

    /// Return the normal item style
    Style const & normalStyle() const;

    /// Set the normal item style
    void setNormalStyle(Style const & s);

    /// Return the selected item style
    Style const & selectedStyle() const;

    /// Set the selected item style
    void setSelectedStyle(Style const & s);

    // Callbacks

    /// Selection change callback type
    using SelectionCallback = std::function<void(int index)>;

    /// Item activation (Enter) callback type
    using ActivateCallback = std::function<void(int index)>;

    /// Set the selection change callback
    void setOnSelectionChanged(SelectionCallback cb);

    /// Set the item activation callback
    void setOnActivate(ActivateCallback cb);

    // Widget interface

    /// Draw the list view to the window
    void draw(Window & window) override;

    /// Handle key event
    bool handleKey(KeyEvent const & event) override;

protected:
    /// Called on resize
    void onResize() override;

private:
    /// Adjust scroll to make the given index visible
    void ensureVisible(int index);

    /// Return the number of items visible on screen
    int visibleItemCount() const;

    std::vector<ListItem> _items;                             ///< Item list
    int _selectedIndex = 0;                                   ///< Current selected index
    int _scrollOffset = 0;                                    ///< Scroll offset
    Style _normalStyle { Colors::LIGHT_GRAY, Colors::BLACK }; ///< Normal item style
    Style _selectedStyle { Colors::WHITE, Colors::RED };      ///< Selected item style
    SelectionCallback _onSelectionChanged;                    ///< Selection change callback
    ActivateCallback _onActivate;                             ///< Item activation callback
};
} // namespace ventty

#endif // VENTTY_WIDGET_LISTVIEW_H
