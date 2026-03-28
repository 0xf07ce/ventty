#include "StatusBar.h"

#include <cstdio>

namespace mdir::view
{
StatusBar::StatusBar()
{}

std::string StatusBar::formatSize(std::uintmax_t size) const
{
    char buf[32];
    if (size >= 1024ULL * 1024 * 1024 * 1024)
    {
        std::snprintf(buf, sizeof(buf), "%.1fT",
            static_cast<double>(size) / (1024.0 * 1024 * 1024 * 1024));
    }
    else if (size >= 1024ULL * 1024 * 1024)
    {
        std::snprintf(buf, sizeof(buf), "%.1fG",
            static_cast<double>(size) / (1024.0 * 1024 * 1024));
    }
    else if (size >= 1024ULL * 1024)
    {
        std::snprintf(buf, sizeof(buf), "%.1fM",
            static_cast<double>(size) / (1024.0 * 1024));
    }
    else if (size >= 1024)
    {
        std::snprintf(buf, sizeof(buf), "%.1fK",
            static_cast<double>(size) / 1024.0);
    }
    else
    {
        std::snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(size));
    }
    return buf;
}

std::string StatusBar::formatSizeWithCommas(std::uintmax_t size) const
{
    std::string s = std::to_string(size);
    int insertPosition = static_cast<int>(s.length()) - 3;
    while (insertPosition > 0)
    {
        s.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return s;
}

void StatusBar::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0)
    {
        return;
    }

    screen.setStyle({ _theme.statusBarFg, _theme.statusBarBg });

    // Clear line
    screen.setCursor(_rect.x, _rect.y);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U' ');
    }

    // Format: " 10 File   5 Dir       1,234,567 Byte     50.0G free"
    char buf[256];

    int const usedPercent = (_totalSpace > 0)
        ? static_cast<int>(100 - (_freeSpace * 100 / _totalSpace))
        : 0;

    std::snprintf(buf, sizeof(buf),
        " %d File   %d Dir       %s Byte    %s(%d%%)byte free (%s)",
        _fileCount,
        _dirCount,
        formatSizeWithCommas(_totalSize).c_str(),
        formatSizeWithCommas(_freeSpace).c_str(),
        usedPercent,
        formatSize(_freeSpace).c_str()
    );

    screen.setCursor(_rect.x, _rect.y);
    screen.putString(buf);
}
} // namespace mdir::view
