#ifndef MDIR_WIDGET_LABEL_H
#define MDIR_WIDGET_LABEL_H

#include "Widget.h"

#include <string>

namespace mdir::widget
{

enum class Align
{
    Left,
    Center,
    Right,
};

class Label : public Widget
{
public:
    Label() = default;
    explicit Label(std::string text);

    std::string const & text() const { return _text; }
    void setText(std::string text);

    Style const & style() const { return _style; }
    void setStyle(Style const & s) { _style = s; }

    Align align() const { return _align; }
    void setAlign(Align a) { _align = a; }

    void draw(Screen & screen) override;

private:
    std::string _text;
    Style _style;
    Align _align = Align::Left;
};

} // namespace mdir::widget

#endif // MDIR_WIDGET_LABEL_H
