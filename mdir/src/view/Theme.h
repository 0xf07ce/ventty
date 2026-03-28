#ifndef MDIR_VIEW_THEME_H
#define MDIR_VIEW_THEME_H

#include <ventty/Color.h>

namespace mdir::view
{

using namespace ventty;

struct Theme
{
    // Main screen
    Color background = Colors::BLACK;
    Color foreground = Colors::LIGHT_GRAY;

    // Borders
    Color border = Colors::CYAN;

    // Function bar
    Color functionBarBg = Colors::CYAN;
    Color functionBarFg = Colors::BLACK;
    Color functionBarKeyFg = Colors::YELLOW;
    Color functionBarSepFg = Colors::BLACK;

    // Path bar
    Color pathBarBg = Colors::LIGHT_GRAY;
    Color pathBarFg = Colors::BLACK;
    Color pathBarPathFg = Colors::BLACK;

    // Panel separator (=== lines)
    Color panelSeparatorFg = Colors::CYAN;
    Color panelSeparatorBg = Colors::BLACK;

    // Scrollbar
    Color scrollBarFg = Colors::CYAN;
    Color scrollBarBg = Colors::BLACK;

    // File list
    Color fileListBg = Colors::BLACK;
    Color fileListFg = Colors::LIGHT_GRAY;
    Color selectionBg = Colors::RED;
    Color selectionFg = Colors::WHITE;

    // File types
    Color directoryFg = Colors::YELLOW;
    Color parentDirFg = Colors::WHITE;
    Color executableFg = Colors::GREEN;
    Color archiveFg = Colors::MAGENTA;
    Color symlinkFg = Colors::LIGHT_CYAN;
    Color hiddenFg = Colors::DARK_GRAY;

    // Status bar
    Color statusBarBg = Colors::LIGHT_GRAY;
    Color statusBarFg = Colors::BLACK;

    // Info bar (bottom)
    Color infoBarBg = Colors::CYAN;
    Color infoBarFg = Colors::BLACK;
    Color infoBarHighlightBg = Colors::RED;
    Color infoBarHighlightFg = Colors::YELLOW;

    // Drive bar
    Color driveBarFg = Colors::YELLOW;

    static Theme const & mdir();
};

inline Theme const & Theme::mdir()
{
    static Theme const theme;
    return theme;
}

} // namespace mdir::view

#endif // MDIR_VIEW_THEME_H
