# ventty

![ventty](docs/ventty.png)

![build](https://github.com/0xf07ce/ventty/actions/workflows/build.yml/badge.svg)
![version](https://img.shields.io/badge/version-0.1.0-blue)
![C++20](https://img.shields.io/badge/C%2B%2B-20-brightgreen)
![license](https://img.shields.io/badge/license-MIT-green)

A modern C++20 terminal UI library designed for both humans and AI agents.

[한국어](README.ko.md)

---

## Overview

**ventty** is a cell-based terminal abstraction library that provides high-level building blocks for creating TUI (Text User Interface) applications on POSIX terminals. It features diff-based rendering, overlapping window management, full Unicode/UTF-8 support, 24-bit true color, and a widget toolkit. The backend is ANSI/VT100 over termios.

For a graphical (SDL3 window) backend, see the sibling project [ventty-sdl](../ventty-sdl). A Windows Console backend lives in [ventty-win](../ventty-win) (scaffolding only).

## Why ventty?

**ventty is built as a harness for AI agents.** The core idea is harness engineering — providing a well-structured, type-safe API surface that guides AI code generation toward correct, safe output by design.

When an AI agent uses ncurses directly, it must deal with a decades-old C API full of implicit global state, macro-based configuration, and platform-specific quirks. The result is fragile, error-prone code that often requires manual correction. ventty eliminates this problem by wrapping the complexity behind a modern C++20 interface where the type system and API design prevent entire classes of mistakes before they happen.

**How ventty differs from ncurses:**

| | ncurses | ventty |
|---|---|---|
| **Language** | C API with global state | C++20, no global state |
| **Rendering** | Imperative cursor movement | Cell-based with automatic diff |
| **Backend** | Terminal only | POSIX terminal (sibling `ventty-sdl` adds SDL3 GUI) |
| **Unicode** | Partial, configuration-dependent | Built-in UTF-8 with Korean support |
| **Widgets** | None (separate libraries needed) | Dialog, Menu, ListView, Label built-in |
| **AI friendliness** | Low — implicit state, macros, footguns | High — typed API, RAII, zero implicit state |

## Features

- **Cell-based rendering** — The terminal is modeled as a 2D grid of styled cells, enabling precise control over every character
- **Diff-based updates** — Only changed cells are re-rendered, minimizing I/O for smooth performance
- **Window management** — Overlapping windows with Z-ordering, scrollback buffers, and visibility control
- **Full Unicode support** — Proper UTF-8 encoding/decoding with Korean (Hangul) built-in support
- **24-bit true color** — RGB, xterm256, and CGA 16-color palettes with color interpolation
- **Input handling** — Keyboard (with modifiers), mouse events, and terminal resize detection
- **Widget toolkit** — Label, ListView, Dialog, InputDialog, Menu/MenuBar widgets
- **ASCII art toolkit** — Box drawing, spinners, progress bars, bar graphs, and braille plotting
- **ANSI/VT100 terminal backend** (`ventty::Terminal`) over termios raw mode

## Requirements

- CMake >= 3.20
- C++20 compiler
- POSIX-compliant system (Linux, macOS, BSD)

## Build

### Using Docker (recommended for Linux)

A Docker image with all build dependencies pre-installed is provided:

```bash
docker build -t ventty-build -f docker/Dockerfile .
docker run --rm -v $(pwd):/workspace ventty-build bash -c "cmake -S . -B build && cmake --build build"
```

### Native build (macOS)

```bash
cmake -S . -B build
cmake --build build
```

Options:

| Option | Default | Description |
|---|---|---|
| `VENTTY_BUILD_TESTS` | `ON` | Build unit tests |
| `VENTTY_BUILD_EXAMPLES` | `ON` | Build example programs |
| `VENTTY_INSTALL` | `ON` (top-level) | Generate install / CMake package export rules |

## Sibling projects

- [ventty-sdl](../ventty-sdl) — SDL3 graphical backend (`GfxTerminal`, embedded CP437/Hangul/Jamo fonts). Use this to render ventty UIs into a native window.
- [ventty-win](../ventty-win) — Windows Console backend (scaffolding only, not yet implemented).

## Documentation

- [Quick Start](docs/quick-start.md)
- [Project Structure & Architecture](docs/project-structure.md)

## License

See [LICENSE](LICENSE) for details.
