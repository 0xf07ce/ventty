#!/usr/bin/env python3
"""Generate test overworld PNG maps for Dice-n-Destiny.

Outputs 128x128 8-bit grayscale PNGs to assets/worlds/.
Uses value noise with octave layering for natural terrain.
Portal locations are marked with pixel value 255.
"""

import os
import random
import math

from PIL import Image


def lerp(a: float, b: float, t: float) -> float:
    return a + (b - a) * t


def smoothstep(t: float) -> float:
    return t * t * (3.0 - 2.0 * t)


class ValueNoise:
    """Simple 2D value noise with smooth interpolation."""

    def __init__(self, seed: int, size: int = 256):
        rng = random.Random(seed)
        self._size = size
        self._table = [rng.random() for _ in range(size * size)]

    def _at(self, ix: int, iy: int) -> float:
        ix = ix % self._size
        iy = iy % self._size
        return self._table[iy * self._size + ix]

    def sample(self, x: float, y: float) -> float:
        ix = int(math.floor(x))
        iy = int(math.floor(y))
        fx = smoothstep(x - ix)
        fy = smoothstep(y - iy)
        v00 = self._at(ix, iy)
        v10 = self._at(ix + 1, iy)
        v01 = self._at(ix, iy + 1)
        v11 = self._at(ix + 1, iy + 1)
        return lerp(lerp(v00, v10, fx), lerp(v01, v11, fx), fy)


def generate_noise(width: int, height: int, seed: int, octaves: int = 4) -> list[list[float]]:
    """Generate a 2D noise field with octave layering. Returns values in [0, 1]."""
    noise = ValueNoise(seed)
    grid = [[0.0] * width for _ in range(height)]
    amp_sum = 0.0
    amp = 1.0
    freq = 1.0

    for _ in range(octaves):
        amp_sum += amp
        for y in range(height):
            for x in range(width):
                nx = x * freq / width * 8.0
                ny = y * freq / height * 8.0
                grid[y][x] += noise.sample(nx, ny) * amp
        amp *= 0.5
        freq *= 2.0

    # Normalize to [0, 1]
    for y in range(height):
        for x in range(width):
            grid[y][x] /= amp_sum

    return grid


def make_world(
    width: int,
    height: int,
    seed: int,
    scale: float = 1.0,
    offset: float = 0.0,
    portals: list[tuple[int, int]] | None = None,
) -> Image.Image:
    """Create a grayscale world image from noise.

    scale/offset transform noise values: pixel = clamp(noise * scale + offset, 0, 254).
    Portal positions get pixel value 255.
    """
    grid = generate_noise(width, height, seed)
    img = Image.new("L", (width, height))
    pixels = img.load()

    for y in range(height):
        for x in range(width):
            v = grid[y][x] * scale + offset
            v = max(0.0, min(1.0, v))
            pixels[x, y] = min(int(v * 255), 254)  # reserve 255 for portals

    if portals:
        for px, py in portals:
            if 0 <= px < width and 0 <= py < height:
                pixels[px, py] = 255

    return img


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    out_dir = os.path.join(script_dir, "..", "assets", "worlds")
    os.makedirs(out_dir, exist_ok=True)

    size = 128

    # World 1: mixed terrain, 2 portals
    img1 = make_world(size, size, seed=42, portals=[(30, 30), (100, 100)])
    img1.save(os.path.join(out_dir, "world1.png"))
    print(f"Generated world1.png ({size}x{size})")

    # World 2: watery terrain (lower values), 1 portal
    img2 = make_world(size, size, seed=137, scale=0.7, offset=0.0, portals=[(64, 64)])
    img2.save(os.path.join(out_dir, "world2.png"))
    print(f"Generated world2.png ({size}x{size})")

    # World 3: mountainous terrain (higher values), 1 portal
    img3 = make_world(size, size, seed=256, scale=0.7, offset=0.3, portals=[(64, 64)])
    img3.save(os.path.join(out_dir, "world3.png"))
    print(f"Generated world3.png ({size}x{size})")


if __name__ == "__main__":
    main()
