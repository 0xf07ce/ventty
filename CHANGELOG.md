# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0/).

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
