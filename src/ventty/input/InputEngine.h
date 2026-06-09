// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/input/KeyChord.h>
#include <ventty/input/Keymap.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace ventty
{
/// Modal, chord- and count-aware input front-end over opaque tokens.
///
/// The engine knows nothing about what tokens mean. It owns the universal vi
/// grammar: a current mode, a numeric repeat accumulator, and a pending-chord
/// buffer matched against the active mode's Keymap. Feed it one KeyChord at a
/// time; it returns a token to act on, absorbs the key (a count digit or a live
/// prefix), or reports a passthrough so the caller can apply its own default.
///
/// There is deliberately no timeout: a chord that is a strict prefix of a
/// longer binding stays Pending until the next key arrives. Presets are written
/// so a bound prefix never also completes a binding, so no disambiguation timer
/// is needed (which suits a terminal loop with no fast tick).
class InputEngine
{
public:
    enum class ResultKind
    {
        None,        ///< Absorbed (count digit, live prefix, or cleared dead branch).
        Emit,        ///< `token` (with `count`) is ready to dispatch.
        Passthrough, ///< No binding and not mid-sequence — caller may handle the key.
    };

    struct Result
    {
        ResultKind kind = ResultKind::None;
        std::string token;
        int count = 0; ///< Repeat count; 0 means unspecified (caller treats as 1).
    };

    /// Install the mode set and their keymaps. `modes` lists mode names with the
    /// first as the initial mode; `counts` enables numeric repeat accumulation.
    void configure(std::vector<std::string> modes,
                   std::unordered_map<std::string, Keymap> keymaps,
                   bool counts);

    /// Feed one normalized chord.
    Result feed(KeyChord const & chord);

    /// Handle Escape. Cancels a pending count/chord first; otherwise pops to the
    /// initial mode. Returns true if it consumed the key; false if there was
    /// nothing to cancel and we are already in the initial mode (the caller may
    /// then apply its own Escape behavior).
    bool feedEsc();

    /// Switch the active mode (e.g. an `enter-visual` action). Clears any
    /// pending count/chord. Unknown modes are ignored.
    void setMode(std::string const & mode);

    std::string const & mode() const { return _mode; }

    /// Buffered count + chords for a which-key hint, e.g. "5d" or "<C-w>".
    std::string pendingDisplay() const;

    bool hasPending() const { return _hasCount || !_pending.empty(); }

    /// Drop any pending count/chord and return to the initial mode.
    void reset();

private:
    Keymap const * activeKeymap() const;
    bool isCountDigit(KeyChord const & chord) const;
    void clearPending();

    std::vector<std::string> _modes;
    std::unordered_map<std::string, Keymap> _keymaps;
    std::string _mode;
    std::string _initialMode;
    bool _counts = false;

    int _count = 0;
    bool _hasCount = false;
    std::vector<KeyChord> _pending;
};
} // namespace ventty
