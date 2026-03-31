# Project Structure

```
ventty/
├── src/ventty/
│   ├── ventty.h              # Single-include header (terminal backend)
│   ├── ventty_gfx.h          # Single-include header (graphical backend)
│   ├── core/                  # Cell, Color, Style, Rect, UTF-8, Window
│   ├── terminal/              # TerminalBase, Terminal (ANSI), Renderer, Input
│   ├── gfx/                   # GfxTerminal (SDL3), GfxRenderer, GfxFont, GfxInput
│   ├── art/                   # ASCII art & UI elements
│   └── widget/                # Label, ListView, Dialog, InputDialog, Menu
├── examples/                  # Interactive demos & widget samples
├── tests/                     # Unit tests
├── tools/                     # Font atlas generator
└── docs/                      # Assets & documentation
```

## Architecture

| Layer | Description |
|-------|-------------|
| **Core** | `Cell`, `Color`, `Style`, `Rect`, `Utf8`, `Window` — fundamental types and 2D cell buffers with drawing primitives |
| **Terminal** | `TerminalBase` (abstract), `Terminal` (ANSI/VT100) — backend-agnostic interface with POSIX implementation |
| **Gfx** | `GfxTerminal`, `GfxRenderer`, `GfxFont`, `GfxInput` — SDL3 graphical backend with bitmap font atlas rendering |
| **Widget** | `Label`, `ListView`, `Dialog`, `InputDialog`, `Menu`, `MenuBar` — reusable UI components |
| **Art** | `AsciiArt` — box drawing, spinners, progress bars, braille plotting |

The library is designed to be backend-agnostic. Both `Terminal` (ANSI/VT100) and `GfxTerminal` (SDL3) implement the common `TerminalBase` interface, allowing widgets and application code to work with either backend.
