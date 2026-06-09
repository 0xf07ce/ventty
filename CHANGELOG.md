# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0/).

## [Unreleased]

## 0.4.1 - 2026-06-09

### Fixed

- Raw mode now also clears `ISIG`, so Ctrl+C / Ctrl+\ / Ctrl+Z reach the
  application as ordinary key bytes instead of raising SIGINT / SIGQUIT /
  SIGTSTP. Previously an accidental Ctrl+\ terminated a full-screen TUI via
  SIGQUIT (bypassing its teardown); control keys are now the app's to handle
  or ignore.

## 0.4.0 - 2026-06-09

### Added

- `ventty/input/` — a modal, configurable input engine that resolves a key
  stream to opaque action tokens. It owns the vi-style grammar (modes, numeric
  repeat counts, chord sequences) so consumers only supply bindings and give
  the tokens meaning:
  - `KeyChord` — a normalized keypress with vim-notation parsing/printing
    (`<C-w>`, `<CR>`, `<Esc>`, `<C-Up>`, `dd`, `gg`) and `from(KeyEvent)`.
  - `Keymap` — a trie of chord sequences mapped to tokens, with
    NoMatch / Pending / Matched resolution so a bound prefix waits for the
    next key without a timer.
  - `InputEngine` — current mode, a count accumulator (with the vim `0` rule),
    a pending-chord buffer, timeout-free prefix handling, and `feedEsc()` to
    cancel a pending count/chord or leave a non-initial mode.
  - `KeymapFile` — a parser for vim-style preset directives
    (`modes = …`, `counts = on|off`, `map <mode> <lhs> <token>`) with an
    optional token validator; `motion`/`op` are reserved for a future
    operator+motion tier.
- `tests/test_input.cpp` — headless unit tests for the input engine.

## 0.3.1 - 2026-05-28

### Fixed

- `Utf8::isFullwidth` now covers the East Asian Wide / Fullwidth ranges
  per Unicode UAX #11. The previous table omitted Hiragana, Katakana,
  and several adjacent blocks, so kana such as U+306E (の) were treated
  as width 1 and overwritten by neighboring fullwidth glyphs on the
  terminal grid.

## 0.3.0 - 2026-05-22

### Added

- `TerminalBase::setCursorPos(x, y)` and `setCursorVisible(bool)` for
  managing the hardware terminal cursor. Default implementations are
  no-ops so GUI backends can ignore them or draw their own cursor.
- `Terminal` (ANSI backend) honors the hardware cursor settings every
  frame: after `emitDiff()`, `render()` parks the cursor at the requested
  cell and toggles visibility, so text-input widgets show a real caret
  while the rest of the UI stays cursor-less.
- CMake package export rules. Consumers can now do
  `find_package(ventty CONFIG)` and link against `ventty::ventty`.
  Controlled by the new `VENTTY_INSTALL` option (defaults to ON when
  ventty is the top-level project, OFF when consumed via
  `add_subdirectory` / `FetchContent`).
- `cmake/venttyConfig.cmake.in` package config template and a minimal
  `docker/Dockerfile` for reproducible Linux builds.

### Changed

- **Backend split.** ventty now ships only the POSIX terminal backend.
  The SDL3 graphical backend moved to the sibling `ventty-sdl` project;
  the Windows Console backend is scaffolded in `ventty-win`.
- ventty now has **no third-party dependencies**. The `VENTTY_BUILD_GFX`
  option, SDL3 / `stb_image` resolution, and the embedded CP437 / Hangul
  / Jamo font assets are gone from this repository.
- `target_include_directories(ventty)` uses `BUILD_INTERFACE` /
  `INSTALL_INTERFACE` generator expressions so the install tree exposes
  the headers under `<prefix>/include/ventty`.
- Updated README, Korean README, and `docs/project-structure.md` to
  describe the new single-backend layout and link out to the sibling
  projects.

### Removed

- `src/ventty/gfx/` and `src/ventty/ventty_gfx.h` (moved to ventty-sdl).
- `examples/demo_gfx_terminal.cpp` (moved to ventty-sdl).
- Bundled font tooling and assets under `tools/` — `embed_font.py`,
  `gen_font_atlas.py`, the `.ttf` fonts, and `requirements.txt` (moved
  to ventty-sdl).
- `deps/include/stb/stb_image.h` (moved to ventty-sdl).
- `tools/gen_overworld.py` (unused content-generation script).

## 0.2.0 - 2026-05-19

### Added

- `art/BrailleCanvas`: off-screen Braille sub-pixel canvas (2x4 dots per
  cell) with `set`/`unset`/`get`, Bresenham `line`, and `blit` into a
  `Window`. Generalizes the per-cell braille trick used by oscilloscope-
  and vinyl-style plots; dot-to-bit mapping matches `BraillePlot::setDot`.
- Modifier-aware key reporting via xterm `modifyOtherKeys=1`.
- `MouseMode` setting, defaulting to button-event reporting.

### Fixed

- Generate `Char` events from `KEY_DOWN` for ASCII printable keys.
- Dispatch a bare ESC as `Key::Escape`.
- Map `0x7f` (DEL) input to the Backspace key.
- Buffer partial input across reads to stop the coordinate-as-key leak.
- Guard `pollEvent()` against re-entrancy: a nested call from a dispatched
  callback no longer mutates the shared input buffer mid-parse, preventing a
  negative-size memmove and heap corruption.
- Resolve `ventty_gfx` deps/include paths relative to `CMAKE_CURRENT_SOURCE_DIR`.

## 0.1.0

Initial release.

### Added

- `ventty` terminal library (C++20).
- CMake-based build system with SDL3 fetched via `FetchContent` when not
  installed locally.
- `ventty_gfx` graphics module with `stb_image` for image loading.
- Jamo font support for Hangul rendering.
- `demo_gfx` example application.
- GitHub Actions build workflow with Docker-based Linux builds.
- Project documentation: README, quick-start, and project-structure guides.
