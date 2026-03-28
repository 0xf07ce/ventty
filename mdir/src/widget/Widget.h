#ifndef MDIR_WIDGET_WIDGET_H
#define MDIR_WIDGET_WIDGET_H

#include <ventty/Color.h>
#include <ventty/Screen.h>
#include <ventty/Input.h>

namespace mdir::widget
{

using namespace ventty;
using Key = KeyEvent::Key;

struct Rect
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    bool contains(int px, int py) const
    {
        return px >= x && px < x + width && py >= y && py < y + height;
    }
};

class Widget
{
public:
    Widget() = default;
    virtual ~Widget() = default;

    // Geometry
    Rect const & rect() const { return _rect; }
    void setRect(Rect const & r) { _rect = r; onResize(); }
    void setRect(int x, int y, int w, int h) { _rect = {x, y, w, h}; onResize(); }

    int x() const { return _rect.x; }
    int y() const { return _rect.y; }
    int width() const { return _rect.width; }
    int height() const { return _rect.height; }

    // Visibility
    bool isVisible() const { return _visible; }
    void setVisible(bool v) { _visible = v; }
    void show() { setVisible(true); }
    void hide() { setVisible(false); }

    // Focus
    bool isFocused() const { return _focused; }
    void setFocused(bool f) { _focused = f; onFocusChanged(); }

    // Rendering
    virtual void draw(Screen & screen) = 0;

    // Input handling
    virtual bool handleKey(KeyEvent const & event) { (void)event; return false; }

protected:
    virtual void onResize() {}
    virtual void onFocusChanged() {}

    Rect _rect;
    bool _visible = true;
    bool _focused = false;
};

} // namespace mdir::widget

#endif // MDIR_WIDGET_WIDGET_H
