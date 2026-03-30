#!/usr/bin/env python3
"""Generate C++ source files and compact .cfnt metadata from font atlas assets.

Usage: python embed_font.py

Reads PNG + FNT from tools/output/
Writes:
  - src/ventty/gfx/fonts/{name}.cpp  (PNG hex + codepage/range constants)
  - src/ventty/gfx/fonts/EmbeddedFonts.h
  - tools/output/{name}.cfnt  (compact font metadata for file-based loading)
"""

from pathlib import Path


def parse_fnt(fnt_path: Path):
    """Parse BMFont .fnt, return (codepoints, atlas_cols, glyph_w, glyph_h, advance)."""
    codepoints = []
    glyph_w = 0
    glyph_h = 0
    atlas_width = 0
    cell_width = 8

    with open(fnt_path) as f:
        for line in f:
            if line.startswith("common"):
                parts = {k: v for k, v in (p.split("=") for p in line.split() if "=" in p)}
                atlas_width = int(parts.get("scaleW", 256))
                glyph_h = int(parts.get("lineHeight", 16))
            elif line.startswith("char "):
                parts = {k: v for k, v in (p.split("=") for p in line.split() if "=" in p)}
                codepoints.append(int(parts["id"]))
                if glyph_w == 0:
                    glyph_w = int(parts.get("width", 8))

    atlas_cols = atlas_width // glyph_w
    advance = 2 if glyph_w > cell_width else 1
    return codepoints, atlas_cols, glyph_w, glyph_h, advance


def is_contiguous(codepoints: list) -> bool:
    for i in range(1, len(codepoints)):
        if codepoints[i] != codepoints[i - 1] + 1:
            return False
    return True


def write_cfnt(out_path: Path, codepoints, atlas_cols, glyph_w, glyph_h, advance):
    """Write compact .cfnt metadata file."""
    contiguous = is_contiguous(codepoints)
    lines = [
        f"glyph {glyph_w} {glyph_h}",
        f"atlas_cols {atlas_cols}",
        f"advance {advance}",
    ]
    if contiguous:
        lines.append(f"range {codepoints[0]} {len(codepoints)}")
    else:
        lines.append(f"codepage {len(codepoints)}")
        for i in range(0, len(codepoints), 16):
            chunk = codepoints[i:i+16]
            lines.append(" ".join(str(cp) for cp in chunk))

    out_path.write_text("\n".join(lines) + "\n")


def generate_cpp(png_path: Path, prefix: str, codepoints, atlas_cols, glyph_w, glyph_h, advance):
    """Generate a single .cpp with PNG data + codepage/range constants."""
    data = png_path.read_bytes()
    contiguous = is_contiguous(codepoints)

    # PNG hex string
    hex_lines = []
    for i in range(0, len(data), 64):
        chunk = data[i:i+64]
        hex_lines.append('    "' + "".join(f"\\x{b:02x}" for b in chunk) + '"')

    parts = [
        '#include "EmbeddedFonts.h"\n',
        "namespace ventty::fonts\n{",
        f"// {png_path.name} ({len(data)} bytes)",
        f"unsigned char const {prefix}_PNG[] =",
        "\n".join(hex_lines) + ";",
        f"unsigned int const {prefix}_PNG_SIZE = {len(data)};",
    ]

    if not contiguous:
        # Codepage array
        cp_lines = []
        for i in range(0, len(codepoints), 16):
            chunk = codepoints[i:i+16]
            cp_lines.append("    " + ", ".join(str(cp) for cp in chunk) + ",")
        parts.append(f"\n// codepage ({len(codepoints)} entries)")
        parts.append(f"char32_t const {prefix}_CODEPAGE[] = {{")
        parts.append("\n".join(cp_lines))
        parts.append("};")

    parts.append("} // namespace ventty::fonts")
    return "\n".join(parts) + "\n"


def generate_header(font_infos):
    """Generate EmbeddedFonts.h."""
    lines = [
        "#pragma once\n",
        "#include <cstdint>\n",
        "namespace ventty::fonts\n{",
    ]

    for info in font_infos:
        prefix = info["prefix"]
        lines.append(f"extern unsigned char const {prefix}_PNG[];")
        lines.append(f"extern unsigned int const {prefix}_PNG_SIZE;")
        lines.append(f"constexpr int {prefix}_GLYPH_WIDTH = {info['glyph_w']};")
        lines.append(f"constexpr int {prefix}_GLYPH_HEIGHT = {info['glyph_h']};")
        lines.append(f"constexpr int {prefix}_ATLAS_COLS = {info['atlas_cols']};")
        lines.append(f"constexpr int {prefix}_ADVANCE = {info['advance']};")
        lines.append(f"constexpr unsigned int {prefix}_COUNT = {info['count']};")

        if info["contiguous"]:
            lines.append(f"constexpr char32_t {prefix}_START = {info['start']};")
        else:
            lines.append(f"extern char32_t const {prefix}_CODEPAGE[];")

        lines.append("")

    lines.append("} // namespace ventty::fonts")
    return "\n".join(lines) + "\n"


def main():
    root = Path(__file__).resolve().parent.parent
    assets_dir = root / "tools" / "output"
    cpp_dir = root / "src" / "ventty" / "gfx" / "fonts"
    cpp_dir.mkdir(parents=True, exist_ok=True)

    fonts = [
        {"name": "cp437_8x16", "prefix": "CP437_8X16"},
        {"name": "hangul_16x16", "prefix": "HANGUL_16X16"},
    ]

    font_infos = []

    for font in fonts:
        name = font["name"]
        prefix = font["prefix"]
        png_path = assets_dir / f"{name}.png"
        fnt_path = assets_dir / f"{name}.fnt"

        codepoints, atlas_cols, glyph_w, glyph_h, advance = parse_fnt(fnt_path)
        contiguous = is_contiguous(codepoints)

        tag = f"range U+{codepoints[0]:04X}+{len(codepoints)}" if contiguous \
            else f"codepage {len(codepoints)}"
        print(f"  {name}: {len(codepoints)} glyphs, {glyph_w}x{glyph_h}, "
              f"cols={atlas_cols}, adv={advance}, {tag}")

        # Write .cfnt
        cfnt_path = assets_dir / f"{name}.cfnt"
        write_cfnt(cfnt_path, codepoints, atlas_cols, glyph_w, glyph_h, advance)
        print(f"    -> {cfnt_path.name}")

        # Write .cpp
        cpp_path = cpp_dir / f"{name}.cpp"
        cpp_path.write_text(generate_cpp(
            png_path, prefix, codepoints, atlas_cols, glyph_w, glyph_h, advance))
        print(f"    -> {cpp_path.name}")

        font_infos.append({
            "prefix": prefix,
            "glyph_w": glyph_w,
            "glyph_h": glyph_h,
            "atlas_cols": atlas_cols,
            "advance": advance,
            "count": len(codepoints),
            "contiguous": contiguous,
            "start": codepoints[0] if contiguous else 0,
        })

    # Write header
    header_path = cpp_dir / "EmbeddedFonts.h"
    header_path.write_text(generate_header(font_infos))
    print(f"  -> EmbeddedFonts.h")
    print("Done.")


if __name__ == "__main__":
    main()
