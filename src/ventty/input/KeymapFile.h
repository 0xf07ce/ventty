// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/input/Keymap.h>

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ventty
{
/// Result of parsing a keymap preset: the mode list (first entry is the initial
/// mode), per-mode keymaps, settings, and any non-fatal warnings.
struct KeymapConfig
{
    std::vector<std::string> modes;
    std::unordered_map<std::string, Keymap> keymaps;
    bool counts = false;
    std::vector<std::string> warnings;

    /// A usable config has at least one declared mode.
    bool ok() const { return !modes.empty(); }
};

/// Token validator: return true if `token` names a known action. When empty,
/// every token is accepted. Unknown tokens produce a warning but the binding is
/// still installed (so a hand-written file round-trips); it simply never
/// resolves to anything.
using TokenValidator = std::function<bool(std::string const &)>;

/// Parse vim-style keymap directives:
///
///   modes  = normal, visual      # first mode is the initial one
///   counts = on | off
///   map <mode> <lhs> <token>     # e.g. map normal <C-w>l focus-right
///
/// Lines are comment-stripped at a '#' that begins the line or follows
/// whitespace. `motion` and `op` are accepted syntactically but reserved for a
/// future operator+motion tier — they warn and are ignored in this version.
KeymapConfig parseKeymap(std::string_view text, TokenValidator validator = {});
} // namespace ventty
