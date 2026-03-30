#!/usr/bin/env python3
"""
gen_font_atlas.py - Bitmap font atlas generator for the Dice-n-Destiny/RetroTerm engine.

Generates a PNG atlas and BMFont-format .fnt metadata file from a TTF font.

Usage:
    python gen_font_atlas.py --font /path/to/font.ttf --size 8x16 --range cp437 --output assets/cp437_8x16
    python gen_font_atlas.py --font /path/to/font.ttf --size 16x16 --range hangul --output assets/hangul_16x16
    python gen_font_atlas.py --font /path/to/font.ttf --size 8x16 --range U+0020-U+007E --output assets/ascii_8x16
"""

import argparse
import math
import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

# ---------------------------------------------------------------------------
# CP437 to Unicode mapping (full 256-character table)
# Source: https://en.wikipedia.org/wiki/Code_page_437
# ---------------------------------------------------------------------------
CP437_TO_UNICODE = [
    0x0000,  # 0x00 - NULL (rendered as space)
    0x263A,  # 0x01 - WHITE SMILING FACE
    0x263B,  # 0x02 - BLACK SMILING FACE
    0x2665,  # 0x03 - BLACK HEART SUIT
    0x2666,  # 0x04 - BLACK DIAMOND SUIT
    0x2663,  # 0x05 - BLACK CLUB SUIT
    0x2660,  # 0x06 - BLACK SPADE SUIT
    0x2022,  # 0x07 - BULLET
    0x25D8,  # 0x08 - INVERSE BULLET
    0x25CB,  # 0x09 - WHITE CIRCLE
    0x25D9,  # 0x0A - INVERSE WHITE CIRCLE
    0x2642,  # 0x0B - MALE SIGN
    0x2640,  # 0x0C - FEMALE SIGN
    0x266A,  # 0x0D - EIGHTH NOTE
    0x266B,  # 0x0E - BEAMED EIGHTH NOTES
    0x263C,  # 0x0F - WHITE SUN WITH RAYS
    0x25BA,  # 0x10 - BLACK RIGHT-POINTING POINTER
    0x25C4,  # 0x11 - BLACK LEFT-POINTING POINTER
    0x2195,  # 0x12 - UP DOWN ARROW
    0x203C,  # 0x13 - DOUBLE EXCLAMATION MARK
    0x00B6,  # 0x14 - PILCROW SIGN
    0x00A7,  # 0x15 - SECTION SIGN
    0x25AC,  # 0x16 - BLACK RECTANGLE
    0x21A8,  # 0x17 - UP DOWN ARROW WITH BASE
    0x2191,  # 0x18 - UPWARDS ARROW
    0x2193,  # 0x19 - DOWNWARDS ARROW
    0x2192,  # 0x1A - RIGHTWARDS ARROW
    0x2190,  # 0x1B - LEFTWARDS ARROW
    0x221F,  # 0x1C - RIGHT ANGLE
    0x2194,  # 0x1D - LEFT RIGHT ARROW
    0x25B2,  # 0x1E - BLACK UP-POINTING TRIANGLE
    0x25BC,  # 0x1F - BLACK DOWN-POINTING TRIANGLE
    0x0020,  # 0x20 - SPACE
    0x0021,  # 0x21 - !
    0x0022,  # 0x22 - "
    0x0023,  # 0x23 - #
    0x0024,  # 0x24 - $
    0x0025,  # 0x25 - %
    0x0026,  # 0x26 - &
    0x0027,  # 0x27 - '
    0x0028,  # 0x28 - (
    0x0029,  # 0x29 - )
    0x002A,  # 0x2A - *
    0x002B,  # 0x2B - +
    0x002C,  # 0x2C - ,
    0x002D,  # 0x2D - -
    0x002E,  # 0x2E - .
    0x002F,  # 0x2F - /
    0x0030,  # 0x30 - 0
    0x0031,  # 0x31 - 1
    0x0032,  # 0x32 - 2
    0x0033,  # 0x33 - 3
    0x0034,  # 0x34 - 4
    0x0035,  # 0x35 - 5
    0x0036,  # 0x36 - 6
    0x0037,  # 0x37 - 7
    0x0038,  # 0x38 - 8
    0x0039,  # 0x39 - 9
    0x003A,  # 0x3A - :
    0x003B,  # 0x3B - ;
    0x003C,  # 0x3C - <
    0x003D,  # 0x3D - =
    0x003E,  # 0x3E - >
    0x003F,  # 0x3F - ?
    0x0040,  # 0x40 - @
    0x0041,  # 0x41 - A
    0x0042,  # 0x42 - B
    0x0043,  # 0x43 - C
    0x0044,  # 0x44 - D
    0x0045,  # 0x45 - E
    0x0046,  # 0x46 - F
    0x0047,  # 0x47 - G
    0x0048,  # 0x48 - H
    0x0049,  # 0x49 - I
    0x004A,  # 0x4A - J
    0x004B,  # 0x4B - K
    0x004C,  # 0x4C - L
    0x004D,  # 0x4D - M
    0x004E,  # 0x4E - N
    0x004F,  # 0x4F - O
    0x0050,  # 0x50 - P
    0x0051,  # 0x51 - Q
    0x0052,  # 0x52 - R
    0x0053,  # 0x53 - S
    0x0054,  # 0x54 - T
    0x0055,  # 0x55 - U
    0x0056,  # 0x56 - V
    0x0057,  # 0x57 - W
    0x0058,  # 0x58 - X
    0x0059,  # 0x59 - Y
    0x005A,  # 0x5A - Z
    0x005B,  # 0x5B - [
    0x005C,  # 0x5C - \
    0x005D,  # 0x5D - ]
    0x005E,  # 0x5E - ^
    0x005F,  # 0x5F - _
    0x0060,  # 0x60 - `
    0x0061,  # 0x61 - a
    0x0062,  # 0x62 - b
    0x0063,  # 0x63 - c
    0x0064,  # 0x64 - d
    0x0065,  # 0x65 - e
    0x0066,  # 0x66 - f
    0x0067,  # 0x67 - g
    0x0068,  # 0x68 - h
    0x0069,  # 0x69 - i
    0x006A,  # 0x6A - j
    0x006B,  # 0x6B - k
    0x006C,  # 0x6C - l
    0x006D,  # 0x6D - m
    0x006E,  # 0x6E - n
    0x006F,  # 0x6F - o
    0x0070,  # 0x70 - p
    0x0071,  # 0x71 - q
    0x0072,  # 0x72 - r
    0x0073,  # 0x73 - s
    0x0074,  # 0x74 - t
    0x0075,  # 0x75 - u
    0x0076,  # 0x76 - v
    0x0077,  # 0x77 - w
    0x0078,  # 0x78 - x
    0x0079,  # 0x79 - y
    0x007A,  # 0x7A - z
    0x007B,  # 0x7B - {
    0x007C,  # 0x7C - |
    0x007D,  # 0x7D - }
    0x007E,  # 0x7E - ~
    0x2302,  # 0x7F - HOUSE
    0x00C7,  # 0x80 - LATIN CAPITAL LETTER C WITH CEDILLA
    0x00FC,  # 0x81 - LATIN SMALL LETTER U WITH DIAERESIS
    0x00E9,  # 0x82 - LATIN SMALL LETTER E WITH ACUTE
    0x00E2,  # 0x83 - LATIN SMALL LETTER A WITH CIRCUMFLEX
    0x00E4,  # 0x84 - LATIN SMALL LETTER A WITH DIAERESIS
    0x00E0,  # 0x85 - LATIN SMALL LETTER A WITH GRAVE
    0x00E5,  # 0x86 - LATIN SMALL LETTER A WITH RING ABOVE
    0x00E7,  # 0x87 - LATIN SMALL LETTER C WITH CEDILLA
    0x00EA,  # 0x88 - LATIN SMALL LETTER E WITH CIRCUMFLEX
    0x00EB,  # 0x89 - LATIN SMALL LETTER E WITH DIAERESIS
    0x00E8,  # 0x8A - LATIN SMALL LETTER E WITH GRAVE
    0x00EF,  # 0x8B - LATIN SMALL LETTER I WITH DIAERESIS
    0x00EE,  # 0x8C - LATIN SMALL LETTER I WITH CIRCUMFLEX
    0x00EC,  # 0x8D - LATIN SMALL LETTER I WITH GRAVE
    0x00C4,  # 0x8E - LATIN CAPITAL LETTER A WITH DIAERESIS
    0x00C5,  # 0x8F - LATIN CAPITAL LETTER A WITH RING ABOVE
    0x00C9,  # 0x90 - LATIN CAPITAL LETTER E WITH ACUTE
    0x00E6,  # 0x91 - LATIN SMALL LETTER AE
    0x00C6,  # 0x92 - LATIN CAPITAL LETTER AE
    0x00F4,  # 0x93 - LATIN SMALL LETTER O WITH CIRCUMFLEX
    0x00F6,  # 0x94 - LATIN SMALL LETTER O WITH DIAERESIS
    0x00F2,  # 0x95 - LATIN SMALL LETTER O WITH GRAVE
    0x00FB,  # 0x96 - LATIN SMALL LETTER U WITH CIRCUMFLEX
    0x00F9,  # 0x97 - LATIN SMALL LETTER U WITH GRAVE
    0x00FF,  # 0x98 - LATIN SMALL LETTER Y WITH DIAERESIS
    0x00D6,  # 0x99 - LATIN CAPITAL LETTER O WITH DIAERESIS
    0x00DC,  # 0x9A - LATIN CAPITAL LETTER U WITH DIAERESIS
    0x00A2,  # 0x9B - CENT SIGN
    0x00A3,  # 0x9C - POUND SIGN
    0x00A5,  # 0x9D - YEN SIGN
    0x20A7,  # 0x9E - PESETA SIGN
    0x0192,  # 0x9F - LATIN SMALL LETTER F WITH HOOK
    0x00E1,  # 0xA0 - LATIN SMALL LETTER A WITH ACUTE
    0x00ED,  # 0xA1 - LATIN SMALL LETTER I WITH ACUTE
    0x00F3,  # 0xA2 - LATIN SMALL LETTER O WITH ACUTE
    0x00FA,  # 0xA3 - LATIN SMALL LETTER U WITH ACUTE
    0x00F1,  # 0xA4 - LATIN SMALL LETTER N WITH TILDE
    0x00D1,  # 0xA5 - LATIN CAPITAL LETTER N WITH TILDE
    0x00AA,  # 0xA6 - FEMININE ORDINAL INDICATOR
    0x00BA,  # 0xA7 - MASCULINE ORDINAL INDICATOR
    0x00BF,  # 0xA8 - INVERTED QUESTION MARK
    0x2310,  # 0xA9 - REVERSED NOT SIGN
    0x00AC,  # 0xAA - NOT SIGN
    0x00BD,  # 0xAB - VULGAR FRACTION ONE HALF
    0x00BC,  # 0xAC - VULGAR FRACTION ONE QUARTER
    0x00A1,  # 0xAD - INVERTED EXCLAMATION MARK
    0x00AB,  # 0xAE - LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    0x00BB,  # 0xAF - RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    0x2591,  # 0xB0 - LIGHT SHADE
    0x2592,  # 0xB1 - MEDIUM SHADE
    0x2593,  # 0xB2 - DARK SHADE
    0x2502,  # 0xB3 - BOX DRAWINGS LIGHT VERTICAL
    0x2524,  # 0xB4 - BOX DRAWINGS LIGHT VERTICAL AND LEFT
    0x2561,  # 0xB5 - BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE
    0x2562,  # 0xB6 - BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE
    0x2556,  # 0xB7 - BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE
    0x2555,  # 0xB8 - BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE
    0x2563,  # 0xB9 - BOX DRAWINGS DOUBLE VERTICAL AND LEFT
    0x2551,  # 0xBA - BOX DRAWINGS DOUBLE VERTICAL
    0x2557,  # 0xBB - BOX DRAWINGS DOUBLE DOWN AND LEFT
    0x255D,  # 0xBC - BOX DRAWINGS DOUBLE UP AND LEFT
    0x255C,  # 0xBD - BOX DRAWINGS UP DOUBLE AND LEFT SINGLE
    0x255B,  # 0xBE - BOX DRAWINGS UP SINGLE AND LEFT DOUBLE
    0x2510,  # 0xBF - BOX DRAWINGS LIGHT DOWN AND LEFT
    0x2514,  # 0xC0 - BOX DRAWINGS LIGHT UP AND RIGHT
    0x2534,  # 0xC1 - BOX DRAWINGS LIGHT UP AND HORIZONTAL
    0x252C,  # 0xC2 - BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
    0x251C,  # 0xC3 - BOX DRAWINGS LIGHT VERTICAL AND RIGHT
    0x2500,  # 0xC4 - BOX DRAWINGS LIGHT HORIZONTAL
    0x253C,  # 0xC5 - BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
    0x255E,  # 0xC6 - BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE
    0x255F,  # 0xC7 - BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE
    0x255A,  # 0xC8 - BOX DRAWINGS DOUBLE UP AND RIGHT
    0x2554,  # 0xC9 - BOX DRAWINGS DOUBLE DOWN AND RIGHT
    0x2569,  # 0xCA - BOX DRAWINGS DOUBLE UP AND HORIZONTAL
    0x2566,  # 0xCB - BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL
    0x2560,  # 0xCC - BOX DRAWINGS DOUBLE VERTICAL AND RIGHT
    0x2550,  # 0xCD - BOX DRAWINGS DOUBLE HORIZONTAL
    0x256C,  # 0xCE - BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL
    0x2567,  # 0xCF - BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE
    0x2568,  # 0xD0 - BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE
    0x2564,  # 0xD1 - BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE
    0x2565,  # 0xD2 - BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE
    0x2559,  # 0xD3 - BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE
    0x2558,  # 0xD4 - BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE
    0x2552,  # 0xD5 - BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE
    0x2553,  # 0xD6 - BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE
    0x256B,  # 0xD7 - BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE
    0x256A,  # 0xD8 - BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE
    0x2518,  # 0xD9 - BOX DRAWINGS LIGHT UP AND LEFT
    0x250C,  # 0xDA - BOX DRAWINGS LIGHT DOWN AND RIGHT
    0x2588,  # 0xDB - FULL BLOCK
    0x2584,  # 0xDC - LOWER HALF BLOCK
    0x258C,  # 0xDD - LEFT HALF BLOCK
    0x2590,  # 0xDE - RIGHT HALF BLOCK
    0x2580,  # 0xDF - UPPER HALF BLOCK
    0x03B1,  # 0xE0 - GREEK SMALL LETTER ALPHA
    0x00DF,  # 0xE1 - LATIN SMALL LETTER SHARP S
    0x0393,  # 0xE2 - GREEK CAPITAL LETTER GAMMA
    0x03C0,  # 0xE3 - GREEK SMALL LETTER PI
    0x03A3,  # 0xE4 - GREEK CAPITAL LETTER SIGMA
    0x03C3,  # 0xE5 - GREEK SMALL LETTER SIGMA
    0x00B5,  # 0xE6 - MICRO SIGN
    0x03C4,  # 0xE7 - GREEK SMALL LETTER TAU
    0x03A6,  # 0xE8 - GREEK CAPITAL LETTER PHI
    0x0398,  # 0xE9 - GREEK CAPITAL LETTER THETA
    0x03A9,  # 0xEA - GREEK CAPITAL LETTER OMEGA
    0x03B4,  # 0xEB - GREEK SMALL LETTER DELTA
    0x221E,  # 0xEC - INFINITY
    0x03C6,  # 0xED - GREEK SMALL LETTER PHI
    0x03B5,  # 0xEE - GREEK SMALL LETTER EPSILON
    0x2229,  # 0xEF - INTERSECTION
    0x2261,  # 0xF0 - IDENTICAL TO
    0x00B1,  # 0xF1 - PLUS-MINUS SIGN
    0x2265,  # 0xF2 - GREATER-THAN OR EQUAL TO
    0x2264,  # 0xF3 - LESS-THAN OR EQUAL TO
    0x2320,  # 0xF4 - TOP HALF INTEGRAL
    0x2321,  # 0xF5 - BOTTOM HALF INTEGRAL
    0x00F7,  # 0xF6 - DIVISION SIGN
    0x2248,  # 0xF7 - ALMOST EQUAL TO
    0x00B0,  # 0xF8 - DEGREE SIGN
    0x2219,  # 0xF9 - BULLET OPERATOR
    0x00B7,  # 0xFA - MIDDLE DOT
    0x221A,  # 0xFB - SQUARE ROOT
    0x207F,  # 0xFC - SUPERSCRIPT LATIN SMALL LETTER N
    0x00B2,  # 0xFD - SUPERSCRIPT TWO
    0x25A0,  # 0xFE - BLACK SQUARE
    0x00A0,  # 0xFF - NO-BREAK SPACE
]


# ---------------------------------------------------------------------------
# Range resolution
# ---------------------------------------------------------------------------

def resolve_range(range_arg: str) -> list[int]:
    """Return a list of Unicode codepoints for the given range name or spec."""
    if range_arg == "cp437":
        return list(CP437_TO_UNICODE)

    if range_arg == "ascii":
        return list(range(0x0020, 0x007F))  # U+0020-U+007E inclusive

    if range_arg == "hangul":
        return list(range(0xAC00, 0xD7B0))  # U+AC00-U+D7AF inclusive (11,172 syllables)

    # Custom range: U+XXXX-U+XXXX
    match = re.fullmatch(r"U\+([0-9A-Fa-f]{4,6})-U\+([0-9A-Fa-f]{4,6})", range_arg)
    if match:
        start = int(match.group(1), 16)
        end = int(match.group(2), 16)
        if start > end:
            raise ValueError(
                f"Range start U+{start:04X} is after range end U+{end:04X}."
            )
        return list(range(start, end + 1))

    raise ValueError(
        f"Unknown range '{range_arg}'. "
        "Use 'cp437', 'ascii', 'hangul', or 'U+XXXX-U+XXXX'."
    )


# ---------------------------------------------------------------------------
# Column count heuristics
# ---------------------------------------------------------------------------

def default_cols(codepoints: list[int], range_arg: str) -> int:
    """Pick a sensible column count for the atlas grid."""
    count = len(codepoints)
    if range_arg == "cp437":
        return 32   # 256 / 32 = 8 rows — classic layout
    if range_arg == "ascii":
        return 16
    if range_arg == "hangul":
        return 128  # 11172 / 128 ≈ 88 rows
    # Generic: aim for a roughly square-ish atlas capped at a power of two
    cols = 1
    while cols * cols < count:
        cols *= 2
    return min(cols, 256)


# ---------------------------------------------------------------------------
# Font size parsing
# ---------------------------------------------------------------------------

def parse_size(size_str: str) -> tuple[int, int]:
    """Parse 'WxH' string into (width, height) integers."""
    match = re.fullmatch(r"(\d+)[xX](\d+)", size_str)
    if not match:
        raise ValueError(
            f"Invalid size '{size_str}'. Expected format like '8x16' or '16x16'."
        )
    return int(match.group(1)), int(match.group(2))


# ---------------------------------------------------------------------------
# Glyph rendering helpers
# ---------------------------------------------------------------------------

def render_glyph_baseline(
    draw: ImageDraw.ImageDraw,
    font: ImageFont.FreeTypeFont,
    codepoint: int,
    cell_x: int,
    cell_y: int,
    cell_w: int,
    cell_h: int,
    baseline_offset_y: int,
) -> None:
    """Render a single glyph aligned to a shared baseline within its cell.

    All glyphs share the same vertical offset (baseline_offset_y) so that
    the baseline is consistent across the atlas.  Only horizontal position
    is adjusted per-glyph to centre it within the cell width.
    """
    char = chr(codepoint)

    # getbbox returns (left, top, right, bottom) relative to the pen origin.
    # Some control-character codepoints (like U+0000) have no visible glyph;
    # we skip them gracefully.
    try:
        bbox = font.getbbox(char)
    except Exception:
        return

    if bbox is None:
        return

    glyph_w = bbox[2] - bbox[0]
    if glyph_w <= 0:
        return

    # Horizontal: centre the glyph in the cell
    offset_x = cell_x + (cell_w - glyph_w) // 2 - bbox[0]
    # Vertical: shared baseline position for all glyphs
    offset_y = cell_y + baseline_offset_y

    draw.text((offset_x, offset_y), char, font=font, fill=(255, 255, 255, 255))


# ---------------------------------------------------------------------------
# Main atlas generation
# ---------------------------------------------------------------------------

def generate_atlas(
    font_path: str,
    cell_w: int,
    cell_h: int,
    codepoints: list[int],
    cols: int,
    output_prefix: str,
    range_arg: str,
) -> None:
    """Build and save the PNG atlas and BMFont .fnt metadata."""

    rows = math.ceil(len(codepoints) / cols)
    atlas_w = cols * cell_w
    atlas_h = rows * cell_h

    print(f"Atlas dimensions: {atlas_w}x{atlas_h} pixels ({cols} cols x {rows} rows)")
    print(f"Total glyphs: {len(codepoints)}")

    # ------------------------------------------------------------------
    # Load font
    # ------------------------------------------------------------------
    # Pillow's truetype size parameter is in points. We want the rendered
    # glyph to fit inside cell_h pixels. Using cell_h as the point size
    # works well for most monospace fonts at 96 dpi (the PIL default).
    try:
        pil_font = ImageFont.truetype(font_path, size=cell_h)
    except OSError as exc:
        print(f"ERROR: Could not load font '{font_path}': {exc}", file=sys.stderr)
        sys.exit(1)

    font_name = os.path.splitext(os.path.basename(font_path))[0]

    # ------------------------------------------------------------------
    # Compute shared baseline offset
    # ------------------------------------------------------------------
    # getmetrics() returns (ascent, descent) for the font.  We centre the
    # full line height (ascent + descent) vertically within the cell and
    # use that fixed offset for every glyph so the baseline stays aligned.
    ascent, descent = pil_font.getmetrics()
    line_height = ascent + descent
    baseline_offset_y = (cell_h - line_height) // 2

    print(f"Font metrics: ascent={ascent}, descent={descent}, "
          f"lineHeight={line_height}, baselineOffsetY={baseline_offset_y}")

    # ------------------------------------------------------------------
    # Create image (RGBA, black transparent background)
    # ------------------------------------------------------------------
    atlas = Image.new("RGBA", (atlas_w, atlas_h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(atlas)

    # ------------------------------------------------------------------
    # Render glyphs
    # ------------------------------------------------------------------
    skipped = 0
    for idx, cp in enumerate(codepoints):
        col = idx % cols
        row = idx // cols
        cell_x = col * cell_w
        cell_y = row * cell_h

        if cp == 0x0000:
            # Null codepoint — leave cell transparent (acts as a space)
            skipped += 1
            continue

        render_glyph_baseline(draw, pil_font, cp, cell_x, cell_y, cell_w, cell_h, baseline_offset_y)

        if (idx + 1) % 500 == 0 or (idx + 1) == len(codepoints):
            print(f"  Rendered {idx + 1}/{len(codepoints)} glyphs...", end="\r")

    print()  # newline after progress

    if skipped:
        print(f"  Skipped {skipped} null/invisible codepoints.")

    # ------------------------------------------------------------------
    # Save PNG
    # ------------------------------------------------------------------
    png_path = output_prefix + ".png"
    os.makedirs(os.path.dirname(png_path) if os.path.dirname(png_path) else ".", exist_ok=True)
    atlas.save(png_path)
    print(f"Saved PNG: {png_path}")

    # ------------------------------------------------------------------
    # Write BMFont .fnt metadata
    # ------------------------------------------------------------------
    fnt_path = output_prefix + ".fnt"
    png_filename = os.path.basename(png_path)

    # Approximate base line: 75% of cell height is a reasonable default.
    base_line = int(cell_h * 0.75)

    lines = []

    # Header
    lines.append(
        f'info face="{font_name}" size={cell_h} bold=0 italic=0 charset="" '
        f'unicode=1 stretchH=100 smooth=0 aa=0 padding=0,0,0,0 spacing=1,1'
    )
    lines.append(
        f"common lineHeight={cell_h} base={base_line} "
        f"scaleW={atlas_w} scaleH={atlas_h} pages=1 packed=0"
    )
    lines.append(f'page id=0 file="{png_filename}"')
    lines.append(f"chars count={len(codepoints)}")

    # One line per glyph
    for idx, cp in enumerate(codepoints):
        col = idx % cols
        row = idx // cols
        x = col * cell_w
        y = row * cell_h
        # xadvance matches cell width (half-width=cell_w, full-width typically 2*cell_w,
        # but for a fixed-cell atlas we always store cell_w here).
        lines.append(
            f"char id={cp} x={x} y={y} width={cell_w} height={cell_h} "
            f"xoffset=0 yoffset=0 xadvance={cell_w} page=0 chnl=0"
        )

    fnt_content = "\n".join(lines) + "\n"

    with open(fnt_path, "w", encoding="utf-8") as fh:
        fh.write(fnt_content)

    print(f"Saved FNT:  {fnt_path}")
    print("Done.")


# ---------------------------------------------------------------------------
# CLI entry point
# ---------------------------------------------------------------------------

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Generate a bitmap font atlas (PNG) and BMFont metadata (.fnt) "
            "from a TTF font for the Dice-n-Destiny/RetroTerm engine."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # CP437 atlas at 8x16 pixels per glyph
  python gen_font_atlas.py --font fonts/Px437_IBM_VGA_8x16.ttf \\
      --size 8x16 --range cp437 --output assets/cp437_8x16

  # Hangul atlas at 16x16
  python gen_font_atlas.py --font fonts/NanumGothicCoding.ttf \\
      --size 16x16 --range hangul --output assets/hangul_16x16

  # Custom Unicode range
  python gen_font_atlas.py --font fonts/unifont.ttf \\
      --size 8x16 --range U+0020-U+00FF --output assets/latin_8x16
""",
    )

    parser.add_argument(
        "--font",
        required=True,
        metavar="PATH",
        help="Path to the TTF (or OTF) font file.",
    )
    parser.add_argument(
        "--size",
        required=True,
        metavar="WxH",
        help='Glyph cell size in pixels, e.g. "8x16" or "16x16".',
    )
    parser.add_argument(
        "--range",
        required=True,
        metavar="RANGE",
        dest="range_arg",
        help=(
            'Predefined range name ("cp437", "ascii", "hangul") '
            'or custom range in "U+XXXX-U+XXXX" format.'
        ),
    )
    parser.add_argument(
        "--output",
        required=True,
        metavar="PREFIX",
        help=(
            'Output path prefix. The script appends ".png" and ".fnt". '
            'Example: "assets/cp437_8x16" produces "assets/cp437_8x16.png" '
            'and "assets/cp437_8x16.fnt".'
        ),
    )
    parser.add_argument(
        "--cols",
        type=int,
        default=None,
        metavar="N",
        help=(
            "Number of columns in the atlas grid. "
            "Defaults to a sensible value per range (32 for cp437, 128 for hangul)."
        ),
    )

    return parser.parse_args()


def main() -> None:
    args = parse_args()

    # Validate font path
    if not os.path.isfile(args.font):
        print(f"ERROR: Font file not found: {args.font}", file=sys.stderr)
        sys.exit(1)

    # Parse cell dimensions
    try:
        cell_w, cell_h = parse_size(args.size)
    except ValueError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)

    print(f"Cell size: {cell_w}x{cell_h} pixels")

    # Resolve glyph list
    try:
        codepoints = resolve_range(args.range_arg)
    except ValueError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)

    print(f"Range '{args.range_arg}': {len(codepoints)} codepoints")

    # Determine column count
    cols = args.cols if args.cols is not None else default_cols(codepoints, args.range_arg)
    if cols <= 0:
        print("ERROR: --cols must be a positive integer.", file=sys.stderr)
        sys.exit(1)

    print(f"Grid columns: {cols}")
    print(f"Font: {args.font}")
    print(f"Output prefix: {args.output}")
    print()

    generate_atlas(
        font_path=args.font,
        cell_w=cell_w,
        cell_h=cell_h,
        codepoints=codepoints,
        cols=cols,
        output_prefix=args.output,
        range_arg=args.range_arg,
    )


if __name__ == "__main__":
    main()
