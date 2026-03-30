#include "AsciiArt.h"
#include <ventty/core/Utf8.h>

#include <algorithm>

namespace ventty
{
std::string progressBar(int width, float ratio, ProgressStyle const & style)
{
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    std::string result;

    encode(style.leftCap, result);

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
            encode(style.filled, result);
        else if (i == fullCells && hasPartial)
            encode(style.partial, result);
        else
            encode(style.empty, result);
    }

    encode(style.rightCap, result);

    return result;
}
} // namespace ventty
