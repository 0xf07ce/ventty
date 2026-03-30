#include "Widget.h"

namespace ventty
{
Rect const & Widget::rect() const
{
    return _rect;
}

void Widget::setRect(Rect const & r)
{
    _rect = r;
    onResize();
}

void Widget::setRect(int x, int y, int w, int h)
{
    _rect = { x, y, w, h };
    onResize();
}

int Widget::x() const
{
    return _rect.x;
}

int Widget::y() const
{
    return _rect.y;
}

int Widget::width() const
{
    return _rect.width;
}

int Widget::height() const
{
    return _rect.height;
}

bool Widget::isVisible() const
{
    return _visible;
}

void Widget::setVisible(bool v)
{
    _visible = v;
}

void Widget::show()
{
    setVisible(true);
}

void Widget::hide()
{
    setVisible(false);
}

bool Widget::isFocused() const
{
    return _focused;
}

void Widget::setFocused(bool f)
{
    _focused = f;
    onFocusChanged();
}

bool Widget::handleKey(KeyEvent const & event)
{
    (void)event;
    return false;
}

void Widget::onResize()
{}

void Widget::onFocusChanged()
{}
} // namespace ventty
