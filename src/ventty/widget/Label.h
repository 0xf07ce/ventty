// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#ifndef VENTTY_WIDGET_LABEL_H
#define VENTTY_WIDGET_LABEL_H

#include "Widget.h"

#include <string>

namespace ventty
{
/// Text alignment direction
enum class Align
{
    Left,   ///< Left-aligned
    Center, ///< Center-aligned
    Right,  ///< Right-aligned
};

/// Simple text display widget
class Label : public Widget
{
public:
    Label() = default;

    /// Create a label with the given text
    explicit Label(std::string text);

    /// Return the text
    std::string const & text() const;

    /// Set the text
    void setText(std::string text);

    /// Return the style
    Style const & style() const;

    /// Set the style
    void setStyle(Style const & s);

    /// Return the alignment
    Align align() const;

    /// Set the alignment
    void setAlign(Align a);

    /// Draw the label to a window
    void draw(Window & window) override;

private:
    std::string _text;          ///< Display text
    Style _style;               ///< Label style
    Align _align = Align::Left; ///< Text alignment
};
} // namespace ventty

#endif // VENTTY_WIDGET_LABEL_H
