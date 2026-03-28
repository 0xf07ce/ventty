#ifndef MDIR_VIEW_FUNCTIONBAR_H
#define MDIR_VIEW_FUNCTIONBAR_H

#include "../widget/Widget.h"
#include "Theme.h"

#include <array>
#include <string>

namespace mdir::view
{

using namespace mdir::widget;

class FunctionBar : public Widget
{
public:
    FunctionBar();

    void setLabel(int fkey, std::string label);  // fkey: 1-12
    std::string const & label(int fkey) const;

    Theme const & theme() const { return _theme; }
    void setTheme(Theme const & t) { _theme = t; }

    void draw(Screen & screen) override;

private:
    std::array<std::string, 12> _labels;
    Theme _theme;
};

} // namespace mdir::view

#endif // MDIR_VIEW_FUNCTIONBAR_H
