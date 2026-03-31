#!/bin/bash
set -euo pipefail
conda run -n mango python gen_font_atlas.py --font fonts/IyagiGGC.ttf --size 16x16 --range hangul --output ../assets/hangul_16x16
conda run -n mango python gen_font_atlas.py --font fonts/Px437_IBM_PGC.ttf --size 8x16 --range cp437 --output ../assets/cp437_8x16
conda run -n mango python gen_font_atlas.py --font fonts/IyagiGGC.ttf --size 16x16 --range jamo --output ../assets/jamo_16x16