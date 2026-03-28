#ifndef MDIR_VIEW_STATUSBAR_H
#define MDIR_VIEW_STATUSBAR_H

#include "../widget/Widget.h"
#include "Theme.h"

#include <string>

namespace mdir::view
{

using namespace mdir::widget;

class StatusBar : public Widget
{
public:
    StatusBar();

    // Statistics
    void setFileCount(int count) { _fileCount = count; }
    void setDirCount(int count) { _dirCount = count; }
    void setTotalSize(std::uintmax_t size) { _totalSize = size; }
    void setFreeSpace(std::uintmax_t size) { _freeSpace = size; }
    void setTotalSpace(std::uintmax_t size) { _totalSpace = size; }

    Theme const & theme() const { return _theme; }
    void setTheme(Theme const & t) { _theme = t; }

    void draw(Screen & screen) override;

private:
    std::string formatSize(std::uintmax_t size) const;
    std::string formatSizeWithCommas(std::uintmax_t size) const;

    int _fileCount = 0;
    int _dirCount = 0;
    std::uintmax_t _totalSize = 0;
    std::uintmax_t _freeSpace = 0;
    std::uintmax_t _totalSpace = 0;
    Theme _theme;
};

} // namespace mdir::view

#endif // MDIR_VIEW_STATUSBAR_H
