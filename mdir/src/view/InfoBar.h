#ifndef MDIR_VIEW_INFOBAR_H
#define MDIR_VIEW_INFOBAR_H

#include "../widget/Widget.h"
#include "Theme.h"

#include <string>

namespace mdir::view
{
using namespace mdir::widget;

class InfoBar : public Widget
{
public:
    InfoBar();

    void setCurrentFile(std::string name) { _currentFile = std::move(name); }
    void setMessage(std::string msg) { _message = std::move(msg); }

    Theme const & theme() const { return _theme; }
    void setTheme(Theme const & t) { _theme = t; }

    void draw(Screen & screen) override;

private:
    std::string getCurrentDateTime() const;

    std::string _currentFile;
    std::string _message;
    Theme _theme;
};
} // namespace mdir::view

#endif // MDIR_VIEW_INFOBAR_H
