#ifndef MDIR_VIEW_PATHBAR_H
#define MDIR_VIEW_PATHBAR_H

#include "../widget/Widget.h"
#include "Theme.h"

#include <string>

namespace mdir::view
{

using namespace mdir::widget;

class PathBar : public Widget
{
public:
    PathBar();

    void setPath(std::string path) { _path = std::move(path); }
    std::string const & path() const { return _path; }

    void setVolume(std::string vol) { _volume = std::move(vol); }
    std::string const & volume() const { return _volume; }

    Theme const & theme() const { return _theme; }
    void setTheme(Theme const & t) { _theme = t; }

    void draw(Screen & screen) override;

private:
    std::string _path;
    std::string _volume;
    Theme _theme;
};

} // namespace mdir::view

#endif // MDIR_VIEW_PATHBAR_H
