#!/bin/bash
set -euo pipefail
OUTPUT_DIR="../build/generated/font"
mkdir -p "$OUTPUT_DIR"
conda run -n mango python gen_font_atlas.py --font fonts/IyagiGGC.ttf --size 16x16 --range hangul --output "$OUTPUT_DIR/hangul_16x16"
conda run -n mango python gen_font_atlas.py --font fonts/Px437_IBM_PGC.ttf --size 8x16 --range cp437 --output "$OUTPUT_DIR/cp437_8x16"
conda run -n mango python gen_font_atlas.py --font fonts/IyagiGGC.ttf --size 16x16 --range jamo --output "$OUTPUT_DIR/jamo_16x16"