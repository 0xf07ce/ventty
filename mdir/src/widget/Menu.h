#ifndef MDIR_WIDGET_MENU_H
#define MDIR_WIDGET_MENU_H

#include "Widget.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mdir::widget
{

struct MenuItem
{
    std::string label;
    std::string shortcut;
    Key key = Key::None;
    bool separator = false;
    bool enabled = true;
    std::function<void()> action;

    static MenuItem makeSeparator()
    {
        MenuItem item;
        item.separator = true;
        return item;
    }
};

class Menu : public Widget
{
public:
    Menu();

    void addItem(MenuItem item);
    void addSeparator();
    void clear();

    std::vector<MenuItem> const & items() const { return _items; }
    int selectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int idx);

    void show(int x, int y);
    void hide();
    bool isOpen() const { return _open; }

    // Execute selected item
    bool execute();

    void draw(Screen & screen) override;
    bool handleKey(KeyEvent const & event) override;

    // Calculate size based on content
    void autoSize();

private:
    void moveSelectionUp();
    void moveSelectionDown();
    int nextSelectableItem(int from, int direction);

    std::vector<MenuItem> _items;
    int _selectedIndex = 0;
    bool _open = false;
};

struct MenuBarEntry
{
    std::string label;
    char hotkey = 0;  // Alt+key
    std::shared_ptr<Menu> menu;
};

class MenuBar : public Widget
{
public:
    MenuBar();

    void addMenu(std::string label, std::shared_ptr<Menu> menu);
    void clear();

    std::vector<MenuBarEntry> const & entries() const { return _entries; }
    int selectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int idx);

    void activate();
    void deactivate();
    bool isActive() const { return _active; }

    Menu * currentMenu();

    void draw(Screen & screen) override;
    bool handleKey(KeyEvent const & event) override;

private:
    std::vector<MenuBarEntry> _entries;
    int _selectedIndex = 0;
    bool _active = false;
};

} // namespace mdir::widget

#endif // MDIR_WIDGET_MENU_H
