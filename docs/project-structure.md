# Project Structure

```
ventty/
├── src/ventty/
│   ├── ventty.h              # Single-include header
│   ├── core/                 # Cell, Color, Style, Rect, UTF-8, Window
│   ├── terminal/             # TerminalBase, Terminal (ANSI), Renderer, Input
│   ├── art/                  # ASCII art & UI elements
│   └── widget/               # Label, ListView, Dialog, InputDialog, Menu
├── cmake/                    # CMake package config template
├── examples/                 # Interactive demos & widget samples
├── tests/                    # Unit tests
└── docs/                     # Assets & documentation
```

## Architecture

| Layer | Description |
|-------|-------------|
| **Core** | `Cell`, `Color`, `Style`, `Rect`, `Utf8`, `Window` — fundamental types and 2D cell buffers with drawing primitives |
| **Terminal** | `TerminalBase` (abstract), `Terminal` (ANSI/VT100) — backend-agnostic interface with POSIX implementation |
| **Widget** | `Label`, `ListView`, `Dialog`, `InputDialog`, `Menu`, `MenuBar` — reusable UI components |
| **Art** | `AsciiArt`, `BrailleCanvas` — box drawing, spinners, progress bars, braille plotting |

The library is designed to be backend-agnostic at the `TerminalBase` interface level. ventty itself ships only the POSIX `Terminal` (ANSI/VT100) implementation; alternative backends are split into sibling projects so the core stays free of GUI/system dependencies:

- **[ventty-sdl](../../ventty-sdl)** — `GfxTerminal` (SDL3 window + bitmap atlas font).
- **[ventty-win](../../ventty-win)** — Windows Console backend (scaffolding only, planned).
