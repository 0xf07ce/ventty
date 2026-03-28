#include "InfoBar.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace mdir::view
{

InfoBar::InfoBar()
{
}

std::string InfoBar::getCurrentDateTime() const
{
    auto const now = std::chrono::system_clock::now();
    std::time_t const tt = std::chrono::system_clock::to_time_t(now);
    std::tm const * tm = std::localtime(&tt);

    std::ostringstream oss;

    // Date
    oss << std::setfill('0')
        << std::setw(2) << (tm->tm_year % 100) << "-"
        << std::setw(2) << (tm->tm_mon + 1) << "-"
        << std::setw(2) << tm->tm_mday;

    // Day of week
    static constexpr std::array days = {
        "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
    };

    oss << " " << days[tm->tm_wday];

    // Time (12-hour format)
    int hour = tm->tm_hour;
    char const * ampm = (hour >= 12) ? "pm" : "am";
    if (hour == 0)
    {
        hour = 12;
    }
    else if (hour > 12)
    {
        hour -= 12;
    }

    oss << " " << std::setfill(' ') << std::setw(2) << hour << ":"
        << std::setfill('0') << std::setw(2) << tm->tm_min << ":"
        << std::setw(2) << tm->tm_sec << ampm;

    return oss.str();
}

void InfoBar::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0)
    {
        return;
    }

    // Clear line with info bar colors
    screen.setStyle({_theme.infoBarFg, _theme.infoBarBg});
    screen.setCursor(_rect.x, _rect.y);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U' ');
    }

    // Left side: current file or message
    screen.setCursor(_rect.x, _rect.y);
    std::string leftText = _message.empty() ? _currentFile : _message;
    if (static_cast<int>(leftText.size()) > _rect.width / 2)
    {
        leftText = leftText.substr(0, _rect.width / 2 - 3) + "...";
    }
    screen.putString(leftText);

    // Right side: date/time and function key hints
    std::string dateTime = getCurrentDateTime();
    std::string hints = "q=Quit";

    std::string rightText = dateTime + " " + hints;
    int const rightX = _rect.x + _rect.width - static_cast<int>(rightText.size()) - 1;

    if (rightX > static_cast<int>(_rect.x + leftText.size() + 2))
    {
        screen.setCursor(rightX, _rect.y);
        screen.putString(dateTime);
        screen.putString(" ");

        // Highlight hints
        screen.setStyle({_theme.infoBarHighlightFg, _theme.infoBarHighlightBg});
        screen.putString(hints);
    }
}

} // namespace mdir::view
