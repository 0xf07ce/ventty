#pragma once

#include "Terminal.h"

#include <optional>
#include <string>

namespace ventty
{

/// Standalone input reader for blocking/timeout key reading.
/// Useful for widget-based applications that need synchronous input.
class Input
{
public:
    Input();
    ~Input();

    /// Read a key event (blocking with timeout). Returns event with Key::None on timeout.
    KeyEvent read(int timeoutMs = -1);

    /// Check if input is available
    bool poll(int timeoutMs = 0);

private:
    int readByte(int timeoutMs);
    KeyEvent parseEscapeSequence();
    KeyEvent parseCsiSequence();
    KeyEvent parseSs3Sequence();

    static constexpr int ESC_TIMEOUT_MS = 50;
};

} // namespace ventty
