#include "FunctionBar.h"

#include <cstdio>

namespace mdir::view
{

FunctionBar::FunctionBar()
{
    // Default MDIR-style labels
    _labels[0] = "Help";
    _labels[1] = "";
    _labels[2] = "View";
    _labels[3] = "Edit";
    _labels[4] = "Copy";
    _labels[5] = "Move";
    _labels[6] = "MkDir";
    _labels[7] = "Del";
    _labels[8] = "";
    _labels[9] = "Quit";
    _labels[10] = "";
    _labels[11] = "";
}

void FunctionBar::setLabel(int fkey, std::string label)
{
    if (fkey >= 1 && fkey <= 12)
    {
        _labels[fkey - 1] = std::move(label);
    }
}

std::string const & FunctionBar::label(int fkey) const
{
    static constexpr std::string empty;
    if (fkey >= 1 && fkey <= 12)
    {
        return _labels[fkey - 1];
    }
    return empty;
}

void FunctionBar::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0)
    {
        return;
    }

    // Calculate width per function key
    int const itemWidth = _rect.width / 10;  // F1-F10 visible

    // Clear line with function bar background
    screen.setStyle({_theme.functionBarFg, _theme.functionBarBg});
    screen.setCursor(_rect.x, _rect.y);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U' ');
    }

    // Draw F1-F10
    int x = _rect.x;
    for (int i = 0; i < 10; ++i)
    {
        // Draw separator before item (except first)
        if (i > 0)
        {
            screen.setCursor(x, _rect.y);
            screen.setStyle({_theme.functionBarSepFg, _theme.functionBarBg});
            screen.putChar(U'|');
            x += 1;
        }

        // Draw function key number
        screen.setCursor(x, _rect.y);
        screen.setStyle({_theme.functionBarKeyFg, _theme.functionBarBg});

        char numBuf[4];
        int const fkeyNum = (i + 1) % 10;
        if (i == 0)
        {
            std::snprintf(numBuf, sizeof(numBuf), "F%d", i + 1);
        }
        else
        {
            std::snprintf(numBuf, sizeof(numBuf), "%d", fkeyNum);
        }
        screen.putString(numBuf);
        int numLen = static_cast<int>(std::strlen(numBuf));
        x += numLen;

        // Draw label
        screen.setStyle({_theme.functionBarFg, _theme.functionBarBg});

        std::string const & lbl = _labels[i];
        int const nextX = _rect.x + (i + 1) * itemWidth;
        int const maxLabelLen = nextX - x - (i < 9 ? 0 : 0);
        std::string displayLabel = lbl.substr(0, std::max(0, maxLabelLen));
        screen.putString(displayLabel);
        x += static_cast<int>(displayLabel.size());

        // Fill remaining space to next item
        while (x < nextX && x < _rect.x + _rect.width)
        {
            screen.putChar(U' ');
            x += 1;
        }
    }
}

} // namespace mdir::view
