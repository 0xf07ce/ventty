// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/core/Color.h>

#include <vector>

namespace ventty
{
class Window;

/// Off-screen sub-pixel bitmap rendered with Unicode Braille glyphs.
///
/// Each terminal cell carries a 2x4 dot grid (U+2800..U+28FF), so a canvas
/// of `cols` x `rows` cells exposes a `cols*2` x `rows*4` monochrome
/// sub-pixel surface. Plot points/lines in sub-pixel space, then blit() the
/// packed glyphs into a Window with a single Style.
///
/// This generalizes the per-cell braille trick used by oscilloscope- and
/// vinyl-style visualizers: a terminal cell is ~1:2 (W:H), so the 2x4 grid
/// yields roughly square sub-pixels without aspect fudging. The dot-to-bit
/// mapping matches BraillePlot::setDot so the two stay interoperable.
class BrailleCanvas
{
public:
    /// Construct a `cols` x `rows` cell canvas (cleared). Non-positive
    /// dimensions clamp to zero, yielding an empty (no-op) canvas.
    BrailleCanvas(int cols, int rows);

    int cols() const { return _cols; }
    int rows() const { return _rows; }
    int subWidth() const { return _cols * 2; }
    int subHeight() const { return _rows * 4; }

    /// Clear every sub-pixel.
    void clear();

    /// Set / clear / test a single sub-pixel. Out-of-range coordinates are
    /// silently ignored (get() returns false), so callers need not clip.
    void set(int sx, int sy);
    void unset(int sx, int sy);
    bool get(int sx, int sy) const;

    /// Bresenham line in sub-pixel space; both endpoints inclusive, points
    /// outside the surface are skipped.
    void line(int x0, int y0, int x1, int y1);

    /// Draw the packed braille glyphs into `window` with the canvas's
    /// top-left at window coordinate (x, y), all in `style`. By default
    /// empty cells are skipped (the window keeps whatever was there);
    /// pass drawEmpty=true to paint a blank braille cell (U+2800) for
    /// every position, e.g. to overwrite a background.
    void blit(Window & window, int x, int y, Style const & style,
              bool drawEmpty = false) const;

private:
    int _cols = 0;
    int _rows = 0;
    std::vector<unsigned char> _cells; ///< one braille bitmask per cell
};

} // namespace ventty
