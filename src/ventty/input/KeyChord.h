// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/terminal/TerminalBase.h>

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ventty
{
/// A single normalized keypress used for keymap matching.
///
/// Derived from KeyEvent but reduced to what a binding cares about: either a
/// named key (Up, Tab, Enter, ...) or a Unicode codepoint, plus the ctrl/alt/
/// shift modifiers. For printable characters the Shift state is already encoded
/// in the codepoint's case, so `shift` is normalized to false there to avoid
/// double-encoding — `from()` and `parse()` both apply this rule so a chord
/// built from a live event compares equal to one parsed from a binding string.
struct KeyChord
{
    KeyEvent::Key key = KeyEvent::Key::None; ///< Named key, or Char for a codepoint
    char32_t ch = 0;                         ///< Codepoint when key == Char
    bool ctrl = false;
    bool alt = false;
    bool shift = false; ///< Only meaningful for named keys (folded into case for Char)

    /// Build a chord from an (already application-normalized) key event.
    static KeyChord from(KeyEvent const & ev);

    /// Parse a single key token in vim notation: "j", "<C-w>", "<CR>",
    /// "<C-Up>", "<S-Tab>", "<Space>". Returns nullopt on malformed input.
    static std::optional<KeyChord> parse(std::string_view token);

    /// Parse a chord *sequence* such as "dd", "gg", "<C-w>l", "<C-w><C-w>" into
    /// its component chords. Returns an empty vector on any malformed token.
    static std::vector<KeyChord> parseSequence(std::string_view seq);

    /// Render back to vim notation (for which-key / help display).
    std::string toString() const;

    bool operator==(KeyChord const & o) const;
    bool operator!=(KeyChord const & o) const { return !(*this == o); }
};

/// Hash so KeyChord can key an unordered_map.
struct KeyChordHash
{
    std::size_t operator()(KeyChord const & k) const;
};
} // namespace ventty
