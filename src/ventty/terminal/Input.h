// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/terminal/TerminalBase.h>

#include <optional>
#include <string>

namespace ventty
{
/// Standalone input reader for blocking/timeout key reading.
/// Useful for widget-based applications that need synchronous input.
class Input
{
public:
    /// Constructor
    Input();

    /// Destructor
    ~Input();

    /// Read a key event (returns Key::None on timeout, -1 waits indefinitely)
    KeyEvent read(int timeoutMs = -1);

    /// Check whether input is available
    bool poll(int timeoutMs = 0);

private:
    /// Read a single byte (with timeout support)
    int readByte(int timeoutMs);

    /// Parse an ESC sequence
    KeyEvent parseEscapeSequence();

    /// Parse a CSI sequence
    KeyEvent parseCsiSequence();

    /// Parse an SS3 sequence
    KeyEvent parseSs3Sequence();

    static constexpr int ESC_TIMEOUT_MS = 50; ///< ESC key timeout (milliseconds)
};
} // namespace ventty
