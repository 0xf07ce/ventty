// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include <ventty/input/InputEngine.h>
#include <ventty/input/KeyChord.h>
#include <ventty/input/Keymap.h>
#include <ventty/input/KeymapFile.h>

#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

using namespace ventty;

namespace
{
using Key = KeyEvent::Key;

/// Parse one key token, asserting it parses.
KeyChord K(std::string_view s)
{
    auto const k = KeyChord::parse(s);
    assert(k.has_value());
    return *k;
}

std::vector<KeyChord> S(std::string_view s)
{
    return KeyChord::parseSequence(s);
}

void expectMatched(Keymap const & km, std::string_view seq, std::string const & token)
{
    auto const r = km.match(S(seq));
    assert(r.status == Keymap::Status::Matched);
    assert(r.token == token);
}
} // namespace

static void testKeyChordParse()
{
    // Plain chars.
    assert(K("j").key == Key::Char && K("j").ch == U'j' && !K("j").ctrl);
    assert(K("G").ch == U'G');

    // Ctrl/alt + letter (ventty delivers ctrl+letter lowercased).
    assert(K("<C-w>").key == Key::Char && K("<C-w>").ch == U'w' && K("<C-w>").ctrl);
    assert(K("<C-W>").ch == U'w' && K("<C-W>").ctrl); // case-folded
    assert(K("<A-x>").alt && K("<A-x>").ch == U'x');

    // Named keys.
    assert(K("<CR>").key == Key::Enter);
    assert(K("<Esc>").key == Key::Escape);
    assert(K("<Tab>").key == Key::Tab);
    assert(K("<C-Up>").key == Key::Up && K("<C-Up>").ctrl);
    assert(K("<S-Tab>").key == Key::Tab && K("<S-Tab>").shift);

    // Char aliases.
    assert(K("<Space>").ch == U' ');
    assert(K("<lt>").ch == U'<');
    assert(K("<gt>").ch == U'>');

    // parse() is a single key; a multi-key string fails.
    assert(!KeyChord::parse("dd").has_value());

    // Shift folds into letter case, so <S-g> == G.
    assert(K("<S-g>").ch == U'G' && !K("<S-g>").shift);
}

static void testParseSequence()
{
    auto dd = S("dd");
    assert(dd.size() == 2 && dd[0].ch == U'd' && dd[1].ch == U'd');

    auto cw = S("<C-w>l");
    assert(cw.size() == 2);
    assert(cw[0].ctrl && cw[0].ch == U'w');
    assert(cw[1].ch == U'l' && !cw[1].ctrl);

    auto ww = S("<C-w><C-w>");
    assert(ww.size() == 2 && ww[0].ctrl && ww[1].ctrl);

    // A bare '<' with no closing '>' degrades to a literal key.
    auto lt = S("<");
    assert(lt.size() == 1 && lt[0].ch == U'<');
}

static void testToStringRoundtrip()
{
    char const * cases[] = { "j", "G", "<C-w>", "<CR>", "<Esc>", "<Tab>",
                             "<C-Up>", "<S-Tab>", "<Space>", "<A-x>", "<F5>" };
    for (auto const * c : cases)
        assert(K(c).toString() == c);
}

static void testFrom()
{
    KeyEvent e;
    e.key = Key::Char;
    e.ch = U'g';
    assert(KeyChord::from(e) == K("g"));

    KeyEvent c;
    c.key = Key::Char;
    c.ch = U'e';
    c.ctrl = true;
    assert(KeyChord::from(c) == K("<C-e>"));

    KeyEvent t;
    t.key = Key::Tab;
    t.shift = true;
    assert(KeyChord::from(t) == K("<S-Tab>"));
}

static void testKeymap()
{
    Keymap km;
    km.bind(S("dd"), "remove");
    km.bind(S("gg"), "first");
    km.bind(S("<C-w>l"), "focus-right");

    assert(km.match(S("d")).status == Keymap::Status::Pending);
    expectMatched(km, "dd", "remove");
    assert(km.match(S("dx")).status == Keymap::Status::NoMatch);
    assert(km.match(S("z")).status == Keymap::Status::NoMatch);
    expectMatched(km, "<C-w>l", "focus-right");
    assert(km.match(S("<C-w>")).status == Keymap::Status::Pending);

    // A single-key leaf matches immediately (the default-preset shape).
    Keymap flat;
    flat.bind(S("d"), "del");
    expectMatched(flat, "d", "del");
}

static InputEngine makeViEngine()
{
    Keymap normal;
    normal.bind(S("j"), "cursor-down");
    normal.bind(S("k"), "cursor-up");
    normal.bind(S("dd"), "remove");
    normal.bind(S("gg"), "first");
    normal.bind(S("G"), "last");
    normal.bind(S("x"), "stop");
    normal.bind(S("<C-w>l"), "focus-right");
    normal.bind(S("v"), "enter-visual");

    Keymap visual;
    visual.bind(S("j"), "cursor-down");
    visual.bind(S("d"), "remove");

    std::unordered_map<std::string, Keymap> maps;
    maps["normal"] = std::move(normal);
    maps["visual"] = std::move(visual);

    InputEngine eng;
    eng.configure({ "normal", "visual" }, std::move(maps), /*counts=*/true);
    return eng;
}

static void testEngineCountsAndChords()
{
    InputEngine eng = makeViEngine();

    // 3j -> cursor-down, count 3.
    assert(eng.feed(K("3")).kind == InputEngine::ResultKind::None);
    auto r = eng.feed(K("j"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "cursor-down" && r.count == 3);

    // 5dd -> remove, count 5 (chord with a count prefix).
    assert(eng.feed(K("5")).kind == InputEngine::ResultKind::None);
    assert(eng.feed(K("d")).kind == InputEngine::ResultKind::None); // pending
    r = eng.feed(K("d"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "remove" && r.count == 5);

    // gg with no count -> first, count 0.
    assert(eng.feed(K("g")).kind == InputEngine::ResultKind::None);
    r = eng.feed(K("g"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "first" && r.count == 0);

    // <C-w>l -> focus-right.
    assert(eng.feed(K("<C-w>")).kind == InputEngine::ResultKind::None);
    r = eng.feed(K("l"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "focus-right");
}

static void testZeroRuleAndMultiDigit()
{
    InputEngine eng = makeViEngine();

    // 10j: '1' starts count, '0' extends it to 10, 'j' fires.
    assert(eng.feed(K("1")).kind == InputEngine::ResultKind::None);
    assert(eng.feed(K("0")).kind == InputEngine::ResultKind::None);
    auto r = eng.feed(K("j"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.count == 10);

    // A leading '0' with no count is NOT a count digit; unbound here -> passthrough.
    r = eng.feed(K("0"));
    assert(r.kind == InputEngine::ResultKind::Passthrough);
}

static void testDeadBranchRetry()
{
    InputEngine eng = makeViEngine();
    // `d` then `x`: `dx` is unbound, but `x` is — the dead branch is discarded
    // and `x` fires fresh (no double-trigger).
    assert(eng.feed(K("d")).kind == InputEngine::ResultKind::None);
    auto r = eng.feed(K("x"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "stop");
}

static void testModesAndEsc()
{
    InputEngine eng = makeViEngine();

    auto r = eng.feed(K("v"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "enter-visual");
    eng.setMode("visual");
    assert(eng.mode() == "visual");

    r = eng.feed(K("d"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "remove"); // visual map

    // Esc pops visual -> normal.
    assert(eng.feedEsc());
    assert(eng.mode() == "normal");

    // Esc cancels a pending count first, and reports "consumed".
    assert(eng.feed(K("7")).kind == InputEngine::ResultKind::None);
    assert(eng.feedEsc());
    assert(!eng.hasPending());

    // Esc with nothing pending in the initial mode is not consumed.
    assert(!eng.feedEsc());
}

static void testCountsOffDefaultPreset()
{
    Keymap normal;
    normal.bind(S("d"), "remove"); // single-key delete (default preset)
    normal.bind(S("1"), "mode-1"); // digit is a real binding when counts are off

    std::unordered_map<std::string, Keymap> maps;
    maps["normal"] = std::move(normal);

    InputEngine eng;
    eng.configure({ "normal" }, std::move(maps), /*counts=*/false);

    auto r = eng.feed(K("d"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "remove"); // immediate

    r = eng.feed(K("1"));
    assert(r.kind == InputEngine::ResultKind::Emit && r.token == "mode-1");
}

static void testKeymapFile()
{
    std::string const text =
        "# vi.keys\n"
        "modes  = normal, visual\n"
        "counts = on\n"
        "\n"
        "map normal j      cursor-down\n"
        "map normal dd     remove\n"
        "map normal <C-w>l focus-right\n"
        "map normal gg     first      # trailing comment\n"
        "map visual d      remove\n"
        "motion normal x   some-motion\n"; // reserved -> warning, ignored

    auto const cfg = parseKeymap(text);
    assert(cfg.ok());
    assert((cfg.modes == std::vector<std::string>{ "normal", "visual" }));
    assert(cfg.counts);

    expectMatched(cfg.keymaps.at("normal"), "j", "cursor-down");
    expectMatched(cfg.keymaps.at("normal"), "dd", "remove");
    expectMatched(cfg.keymaps.at("normal"), "<C-w>l", "focus-right");
    expectMatched(cfg.keymaps.at("normal"), "gg", "first");
    expectMatched(cfg.keymaps.at("visual"), "d", "remove");

    // The reserved `motion` directive produced a warning and bound nothing.
    assert(cfg.keymaps.at("normal").match(S("x")).status == Keymap::Status::NoMatch);
    assert(!cfg.warnings.empty());

    // Validator flags unknown action tokens.
    bool sawUnknown = false;
    auto const cfg2 = parseKeymap("modes = normal\nmap normal q bogus-action\n",
                                  [&](std::string const & t) {
                                      bool const known = (t == "quit");
                                      if (!known)
                                          sawUnknown = true;
                                      return known;
                                  });
    assert(sawUnknown);
    assert(!cfg2.warnings.empty());
}

int main()
{
    testKeyChordParse();
    testParseSequence();
    testToStringRoundtrip();
    testFrom();
    testKeymap();
    testEngineCountsAndChords();
    testZeroRuleAndMultiDigit();
    testDeadBranchRetry();
    testModesAndEsc();
    testCountsOffDefaultPreset();
    testKeymapFile();

    std::printf("test_input: all tests passed\n");
    return 0;
}
