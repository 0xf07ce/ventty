// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#ifndef VENTTY_WIDGET_WIDGET_H
#define VENTTY_WIDGET_WIDGET_H

#include <ventty/core/Color.h>
#include <ventty/core/Rect.h>
#include <ventty/core/Window.h>
#include <ventty/terminal/TerminalBase.h>

namespace ventty
{
/// Base widget class
class Widget
{
public:
    Widget() = default;

    virtual ~Widget() = default;

    // Geometry

    /// Return the widget rect
    Rect const & rect() const;

    /// Set the widget rect from a Rect
    void setRect(Rect const & r);

    /// Set the widget rect from position and size
    void setRect(int x, int y, int w, int h);

    /// Return the X coordinate
    int x() const;

    /// Return the Y coordinate
    int y() const;

    /// Return the width
    int width() const;

    /// Return the height
    int height() const;

    // Visibility

    /// Return whether the widget is visible
    bool isVisible() const;

    /// Set visibility
    void setVisible(bool v);

    /// Show the widget
    void show();

    /// Hide the widget
    virtual void hide();

    // Focus

    /// Return whether the widget is focused
    bool isFocused() const;

    /// Set focus state
    void setFocused(bool f);

    // Rendering

    /// Draw the widget to a window (pure virtual)
    virtual void draw(Window & window) = 0;

    // Input handling

    /// Handle a key event (return true if handled)
    virtual bool handleKey(KeyEvent const & event);

protected:
    /// Callback invoked on resize
    virtual void onResize();

    /// Callback invoked on focus change
    virtual void onFocusChanged();

    Rect _rect;            ///< Widget rect
    bool _visible = true;  ///< Visibility flag
    bool _focused = false; ///< Focus flag
};
} // namespace ventty

#endif // VENTTY_WIDGET_WIDGET_H
