// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include <ventty/input/KeyChord.h>

#include <ventty/core/Utf8.h>

#include <cctype>

namespace ventty
{
namespace
{
using Key = KeyEvent::Key;

/// Case-insensitive ASCII comparison.
bool ieq(std::string_view a, std::string_view b)
{
    if (a.size() != b.size())
        return false;
    for (std::size_t i = 0; i < a.size(); ++i)
    {
        if (std::tolower(static_cast<unsigned char>(a[i]))
            != std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    }
    return true;
}

struct NamedKey
{
    char const * name;
    Key key;
};

// vim-notation base name -> named key. First spelling is the canonical one
// emitted by toString().
NamedKey const NAMED[] = {
    { "CR", Key::Enter },        { "Enter", Key::Enter },     { "Return", Key::Enter },
    { "Esc", Key::Escape },      { "Escape", Key::Escape },
    { "Tab", Key::Tab },
    { "BS", Key::Backspace },    { "Backspace", Key::Backspace },
    { "Del", Key::Delete },      { "Delete", Key::Delete },
    { "Ins", Key::Insert },      { "Insert", Key::Insert },
    { "Up", Key::Up },           { "Down", Key::Down },
    { "Left", Key::Left },       { "Right", Key::Right },
    { "Home", Key::Home },       { "End", Key::End },
    { "PageUp", Key::PageUp },   { "PgUp", Key::PageUp },
    { "PageDown", Key::PageDown },{ "PgDn", Key::PageDown },
    { "F1", Key::F1 },   { "F2", Key::F2 },   { "F3", Key::F3 },   { "F4", Key::F4 },
    { "F5", Key::F5 },   { "F6", Key::F6 },   { "F7", Key::F7 },   { "F8", Key::F8 },
    { "F9", Key::F9 },   { "F10", Key::F10 }, { "F11", Key::F11 }, { "F12", Key::F12 },
};

/// Finish a Char chord, folding Shift into letter case and lowercasing
/// ctrl+letter to match what ventty's input layer delivers.
KeyChord makeChar(KeyChord k, char32_t cp)
{
    k.key = Key::Char;
    if (k.shift && cp >= U'a' && cp <= U'z')
        cp = cp - U'a' + U'A';
    k.shift = false; // never carried on a Char chord
    if (k.ctrl && cp >= U'A' && cp <= U'Z')
        cp = cp - U'A' + U'a';
    k.ch = cp;
    return k;
}
} // namespace

KeyChord KeyChord::from(KeyEvent const & ev)
{
    KeyChord k;
    k.key = ev.key;
    k.ctrl = ev.ctrl;
    k.alt = ev.alt;
    if (ev.key == Key::Char)
    {
        k.ch = ev.ch;
        k.shift = false; // case already encodes Shift
    }
    else
    {
        k.ch = 0;
        k.shift = ev.shift;
    }
    return k;
}

std::optional<KeyChord> KeyChord::parse(std::string_view token)
{
    if (token.empty())
        return std::nullopt;

    if (token.front() == '<' && token.back() == '>' && token.size() >= 3)
    {
        std::string_view inner = token.substr(1, token.size() - 2);
        KeyChord k;

        // Consume modifier prefixes (C- A- M- S-), in any order.
        while (inner.size() >= 2 && inner[1] == '-')
        {
            char const m = static_cast<char>(std::tolower(static_cast<unsigned char>(inner[0])));
            if (m == 'c')
                k.ctrl = true;
            else if (m == 'a' || m == 'm')
                k.alt = true;
            else if (m == 's')
                k.shift = true;
            else
                break;
            inner = inner.substr(2);
        }
        if (inner.empty())
            return std::nullopt;

        for (auto const & nk : NAMED)
        {
            if (ieq(inner, nk.name))
            {
                k.key = nk.key;
                return k; // named keys carry ctrl/alt/shift as parsed
            }
        }

        // Named aliases that resolve to a printable character.
        if (ieq(inner, "Space"))
            return makeChar(k, U' ');
        if (ieq(inner, "lt"))
            return makeChar(k, U'<');
        if (ieq(inner, "gt"))
            return makeChar(k, U'>');
        if (ieq(inner, "bar"))
            return makeChar(k, U'|');
        if (ieq(inner, "bslash"))
            return makeChar(k, U'\\');

        // Otherwise a single codepoint (e.g. <C-w>).
        std::size_t pos = 0;
        char32_t const cp = decode(inner, pos);
        if (pos != inner.size())
            return std::nullopt; // more than one codepoint inside <...>
        return makeChar(k, cp);
    }

    // Bare token: exactly one codepoint.
    std::size_t pos = 0;
    char32_t const cp = decode(token, pos);
    if (pos != token.size())
        return std::nullopt;
    return makeChar(KeyChord{}, cp);
}

std::vector<KeyChord> KeyChord::parseSequence(std::string_view seq)
{
    std::vector<KeyChord> out;
    std::size_t pos = 0;
    while (pos < seq.size())
    {
        if (seq[pos] == '<')
        {
            std::size_t const close = seq.find('>', pos + 1);
            if (close != std::string_view::npos)
            {
                std::string_view const tok = seq.substr(pos, close - pos + 1);
                if (auto const k = parse(tok))
                {
                    out.push_back(*k);
                    pos = close + 1;
                    continue;
                }
            }
            // Not a valid <...> token — treat '<' as a literal key.
            out.push_back(KeyChord{ Key::Char, U'<', false, false, false });
            ++pos;
        }
        else
        {
            std::size_t const start = pos;
            decode(seq, pos);
            if (pos == start)
                ++pos; // safety against a non-advancing decode
            auto const k = parse(seq.substr(start, pos - start));
            if (!k)
                return {};
            out.push_back(*k);
        }
    }
    return out;
}

std::string KeyChord::toString() const
{
    std::string base;
    bool bracket = false;

    switch (key)
    {
    case Key::Char:
        if (ch == U' ')
        {
            base = "Space";
            bracket = true;
        }
        else
        {
            encode(ch, base);
        }
        break;
    case Key::Up: base = "Up"; bracket = true; break;
    case Key::Down: base = "Down"; bracket = true; break;
    case Key::Left: base = "Left"; bracket = true; break;
    case Key::Right: base = "Right"; bracket = true; break;
    case Key::Home: base = "Home"; bracket = true; break;
    case Key::End: base = "End"; bracket = true; break;
    case Key::PageUp: base = "PageUp"; bracket = true; break;
    case Key::PageDown: base = "PageDown"; bracket = true; break;
    case Key::Insert: base = "Ins"; bracket = true; break;
    case Key::Delete: base = "Del"; bracket = true; break;
    case Key::Backspace: base = "BS"; bracket = true; break;
    case Key::Tab: base = "Tab"; bracket = true; break;
    case Key::Enter: base = "CR"; bracket = true; break;
    case Key::Escape: base = "Esc"; bracket = true; break;
    case Key::F1: base = "F1"; bracket = true; break;
    case Key::F2: base = "F2"; bracket = true; break;
    case Key::F3: base = "F3"; bracket = true; break;
    case Key::F4: base = "F4"; bracket = true; break;
    case Key::F5: base = "F5"; bracket = true; break;
    case Key::F6: base = "F6"; bracket = true; break;
    case Key::F7: base = "F7"; bracket = true; break;
    case Key::F8: base = "F8"; bracket = true; break;
    case Key::F9: base = "F9"; bracket = true; break;
    case Key::F10: base = "F10"; bracket = true; break;
    case Key::F11: base = "F11"; bracket = true; break;
    case Key::F12: base = "F12"; bracket = true; break;
    case Key::None: base = ""; break;
    }

    if (ctrl || alt || shift)
        bracket = true;
    if (!bracket)
        return base;

    std::string mods;
    if (ctrl)
        mods += "C-";
    if (alt)
        mods += "A-";
    if (shift)
        mods += "S-";
    return "<" + mods + base + ">";
}

bool KeyChord::operator==(KeyChord const & o) const
{
    return key == o.key && ch == o.ch && ctrl == o.ctrl && alt == o.alt && shift == o.shift;
}

std::size_t KeyChordHash::operator()(KeyChord const & k) const
{
    std::size_t h = static_cast<std::size_t>(k.key);
    h = h * 1000003u + static_cast<std::size_t>(k.ch);
    h = (h << 1) ^ (k.ctrl ? 1u : 0u);
    h = (h << 1) ^ (k.alt ? 1u : 0u);
    h = (h << 1) ^ (k.shift ? 1u : 0u);
    return h;
}
} // namespace ventty
