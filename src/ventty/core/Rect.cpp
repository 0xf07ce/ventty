// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include "Rect.h"

namespace ventty
{
bool Rect::contains(int px, int py) const
{
    return px >= x && px < x + width && py >= y && py < y + height;
}
} // namespace ventty
