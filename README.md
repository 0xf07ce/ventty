# venttyx

Monorepo containing the ventty terminal library and related projects.

## Projects

| Directory | Description | Dependencies |
|-----------|-------------|--------------|
| `ventty/` | Terminal abstraction library (cell buffer, ANSI rendering, input, window management) | POSIX |
| `ventty_gfx/` | SDL3-based graphical backend for ventty | ventty, SDL3, SDL3_ttf, SDL3_image |
| `mdir/` | MDIR-style file manager built on ventty | ventty |

## Structure

This is a monorepo. All three projects share a single build and are developed together.
They are intended to be split into separate repositories once the APIs stabilize.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```
