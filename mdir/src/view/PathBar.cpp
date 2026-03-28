#include "PathBar.h"

namespace mdir::view
{
PathBar::PathBar()
{}

void PathBar::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0)
    {
        return;
    }

    // Clear line
    screen.setStyle({ _theme.pathBarFg, _theme.pathBarBg });
    screen.setCursor(_rect.x, _rect.y);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U' ');
    }

    // Draw "Path" label
    screen.setCursor(_rect.x, _rect.y);
    screen.setStyle({ _theme.pathBarFg, _theme.pathBarBg });
    screen.putString("Path ");

    // Draw path
    screen.setStyle({ _theme.pathBarPathFg, _theme.pathBarBg });

    // Truncate path if needed
    std::string displayPath = _path + "/*";
    int const maxPathLen = _rect.width - 5 - 20; // Reserve space for "Path " and volume
    if (static_cast<int>(displayPath.size()) > maxPathLen)
    {
        displayPath = "..." + displayPath.substr(displayPath.size() - maxPathLen + 3);
    }
    screen.putString(displayPath);

    // Draw volume on right side
    if (!_volume.empty())
    {
        std::string volStr = "Volume [" + _volume + "]";
        int const volX = _rect.x + _rect.width - static_cast<int>(volStr.size());
        screen.setCursor(volX, _rect.y);
        screen.setStyle({ _theme.pathBarFg, _theme.pathBarBg });
        screen.putString("Volume [");
        screen.setStyle({ _theme.pathBarPathFg, _theme.pathBarBg });
        screen.putString(_volume);
        screen.setStyle({ _theme.pathBarFg, _theme.pathBarBg });
        screen.putString("]");
    }
}
} // namespace mdir::view
