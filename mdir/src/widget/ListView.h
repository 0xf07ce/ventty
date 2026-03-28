#ifndef MDIR_WIDGET_LISTVIEW_H
#define MDIR_WIDGET_LISTVIEW_H

#include "Widget.h"

#include <functional>
#include <string>
#include <vector>

namespace mdir::widget
{

struct ListItem
{
    std::string text;
    Style style;
    void * userData = nullptr;
};

class ListView : public Widget
{
public:
    ListView() = default;

    // Items
    void clear();
    void addItem(ListItem item);
    void setItems(std::vector<ListItem> items);
    std::vector<ListItem> const & items() const { return _items; }
    size_t itemCount() const { return _items.size(); }

    // Selection
    int selectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int index);
    ListItem const * selectedItem() const;

    // Scrolling
    int scrollOffset() const { return _scrollOffset; }
    void setScrollOffset(int offset);
    void scrollToSelected();

    // Style
    Style const & normalStyle() const { return _normalStyle; }
    void setNormalStyle(Style const & s) { _normalStyle = s; }

    Style const & selectedStyle() const { return _selectedStyle; }
    void setSelectedStyle(Style const & s) { _selectedStyle = s; }

    // Callbacks
    using SelectionCallback = std::function<void(int index)>;
    using ActivateCallback = std::function<void(int index)>;

    void setOnSelectionChanged(SelectionCallback cb) { _onSelectionChanged = std::move(cb); }
    void setOnActivate(ActivateCallback cb) { _onActivate = std::move(cb); }

    // Widget interface
    void draw(Screen & screen) override;
    bool handleKey(KeyEvent const & event) override;

protected:
    void onResize() override;

private:
    void ensureVisible(int index);
    int visibleItemCount() const;

    std::vector<ListItem> _items;
    int _selectedIndex = 0;
    int _scrollOffset = 0;

    Style _normalStyle{Colors::LIGHT_GRAY, Colors::BLACK};
    Style _selectedStyle{Colors::WHITE, Colors::RED};

    SelectionCallback _onSelectionChanged;
    ActivateCallback _onActivate;
};

} // namespace mdir::widget

#endif // MDIR_WIDGET_LISTVIEW_H
