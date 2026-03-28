#include "AsciiArt.h"
#include "Utf8.h"

#include <algorithm>

namespace ventty::ascii
{

std::string progressBar(int width, float ratio, ProgressStyle const & style)
{
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    std::string result;

    utf8::encode(style.leftCap, result);

    int inner = width - 2;
    if (inner < 0)
        inner = 0;

    float filled = ratio * static_cast<float>(inner);
    int fullCells = static_cast<int>(filled);
    bool hasPartial = (filled - static_cast<float>(fullCells)) > 0.25f
                   && fullCells < inner;

    for (int i = 0; i < inner; ++i)
    {
        if (i < fullCells)
            utf8::encode(style.filled, result);
        else if (i == fullCells && hasPartial)
            utf8::encode(style.partial, result);
        else
            utf8::encode(style.empty, result);
    }

    utf8::encode(style.rightCap, result);

    return result;
}

} // namespace ventty::ascii
