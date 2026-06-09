// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include <ventty/input/InputEngine.h>

#include <string>

namespace ventty
{
void InputEngine::configure(std::vector<std::string> modes,
                            std::unordered_map<std::string, Keymap> keymaps,
                            bool counts)
{
    _modes = std::move(modes);
    _keymaps = std::move(keymaps);
    _counts = counts;
    _initialMode = _modes.empty() ? std::string("normal") : _modes.front();
    _mode = _initialMode;
    clearPending();
}

Keymap const * InputEngine::activeKeymap() const
{
    auto const it = _keymaps.find(_mode);
    return it == _keymaps.end() ? nullptr : &it->second;
}

bool InputEngine::isCountDigit(KeyChord const & chord) const
{
    return chord.key == KeyEvent::Key::Char && !chord.ctrl && !chord.alt
           && chord.ch >= U'0' && chord.ch <= U'9';
}

void InputEngine::clearPending()
{
    _pending.clear();
    _count = 0;
    _hasCount = false;
}

InputEngine::Result InputEngine::feed(KeyChord const & chord)
{
    // Count prefix — only at the start of a sequence. The 0-rule: a leading '0'
    // (no count accumulated yet) is a normal key, not a count, so it stays
    // bindable (e.g. to "first item").
    if (_counts && _pending.empty() && isCountDigit(chord))
    {
        int const digit = static_cast<int>(chord.ch - U'0');
        if (!(digit == 0 && !_hasCount))
        {
            _count = _count * 10 + digit;
            _hasCount = true;
            return {};
        }
    }

    Keymap const * const km = activeKeymap();
    if (km == nullptr)
    {
        clearPending();
        return { ResultKind::Passthrough, {}, 0 };
    }

    _pending.push_back(chord);
    Keymap::Result const r = km->match(_pending);

    if (r.status == Keymap::Status::Matched)
    {
        int const count = _hasCount ? _count : 0;
        clearPending();
        return { ResultKind::Emit, r.token, count };
    }
    if (r.status == Keymap::Status::Pending)
        return {};

    // NoMatch: discard the dead branch. If we were mid-sequence, retry the
    // just-pressed key as a fresh start so a still-bound key isn't swallowed
    // (e.g. `d` then `x`, where `dx` is unbound but `x` is bound).
    bool const wasMidSequence = _pending.size() > 1;
    clearPending();
    if (wasMidSequence)
    {
        _pending.push_back(chord);
        Keymap::Result const r2 = km->match(_pending);
        if (r2.status == Keymap::Status::Matched)
        {
            clearPending();
            return { ResultKind::Emit, r2.token, 0 };
        }
        if (r2.status == Keymap::Status::Pending)
            return {};
        clearPending();
    }
    return { ResultKind::Passthrough, {}, 0 };
}

bool InputEngine::feedEsc()
{
    if (_hasCount || !_pending.empty())
    {
        clearPending();
        return true;
    }
    if (_mode != _initialMode)
    {
        _mode = _initialMode;
        return true;
    }
    return false;
}

void InputEngine::setMode(std::string const & mode)
{
    for (auto const & m : _modes)
    {
        if (m == mode)
        {
            _mode = mode;
            clearPending();
            return;
        }
    }
    // Unknown mode: ignore.
}

std::string InputEngine::pendingDisplay() const
{
    std::string out;
    if (_hasCount)
        out += std::to_string(_count);
    for (auto const & chord : _pending)
        out += chord.toString();
    return out;
}

void InputEngine::reset()
{
    clearPending();
    _mode = _initialMode;
}
} // namespace ventty
