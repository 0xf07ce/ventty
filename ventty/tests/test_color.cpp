#include <ventty/Color.h>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testColorConstruct()
{
    Color c{100, 200, 50};
    assert(c.r == 100);
    assert(c.g == 200);
    assert(c.b == 50);
}

static void testColorDefault()
{
    Color c;
    assert(c.r == 0);
    assert(c.g == 0);
    assert(c.b == 0);
}

static void testColorEquality()
{
    Color a{10, 20, 30};
    Color b{10, 20, 30};
    Color c{10, 20, 31};
    assert(a == b);
    assert(a != c);
}

static void testPalette()
{
    assert(Colors::BLACK == (Color{0, 0, 0}));
    assert(Colors::WHITE == (Color{255, 255, 255}));
    assert(Palette::CGA_16[0] == Colors::BLACK);
    assert(Palette::CGA_16[15] == Colors::WHITE);
}

static void testXterm256()
{
    for (int i = 0; i < 16; ++i)
        assert(Palette::xterm256(static_cast<uint8_t>(i)) == Palette::CGA_16[i]);

    assert(Palette::xterm256(16) == (Color{0, 0, 0}));
    assert(Palette::xterm256(196) == (Color{255, 0, 0}));
    assert(Palette::xterm256(232) == (Color{8, 8, 8}));
    assert(Palette::xterm256(255) == (Color{238, 238, 238}));
}

static void testLerpColor()
{
    Color a{0, 0, 0};
    Color b{255, 255, 255};

    assert(lerpColor(a, b, 0.0f) == a);
    assert(lerpColor(a, b, 1.0f) == b);

    auto mid = lerpColor(a, b, 0.5f);
    assert(mid.r >= 126 && mid.r <= 128);
}

static void testFromDosIndex()
{
    assert(Color::fromDosIndex(0) == Colors::BLACK);
    assert(Color::fromDosIndex(15) == Colors::WHITE);
    assert(Color::fromDosIndex(4) == Colors::RED);
    assert(Color::fromDosIndex(99) == Colors::BLACK);
}

static void testFromHex()
{
    assert(Color::fromHex("#FF8040") == (Color{0xFF, 0x80, 0x40}));
    assert(Color::fromHex("FF8040") == (Color{0xFF, 0x80, 0x40}));
    assert(Color::fromHex("") == Colors::BLACK);
    assert(Color::fromHex("abc") == Colors::BLACK);
}

static void testToHex()
{
    Color c{0xFF, 0x80, 0x40};
    assert(c.toHex() == "#FF8040");
    assert(Colors::BLACK.toHex() == "#000000");
}

static void testStyle()
{
    Style s;
    assert(s.fg == Colors::LIGHT_GRAY);
    assert(s.bg == Colors::BLACK);
    assert(s.attr == Attr::None);

    Style s2{Colors::RED, Colors::BLUE, Attr::Bold};
    assert(s2.fg == Colors::RED);
    assert(s2.bg == Colors::BLUE);
    assert(s2.attr == Attr::Bold);
    assert(s != s2);
}

static void testAttr()
{
    assert(hasAttr(Attr::Bold, Attr::Bold));
    assert(!hasAttr(Attr::Bold, Attr::Italic));

    Attr combined = Attr::Bold | Attr::Underline;
    assert(hasAttr(combined, Attr::Bold));
    assert(hasAttr(combined, Attr::Underline));
    assert(!hasAttr(combined, Attr::Blink));

    assert(static_cast<uint8_t>(Attr::Strike) == 64);
}

int main()
{
    testColorConstruct();
    testColorDefault();
    testColorEquality();
    testPalette();
    testXterm256();
    testLerpColor();
    testFromDosIndex();
    testFromHex();
    testToHex();
    testStyle();
    testAttr();

    std::printf("test_color: all tests passed\n");
    return 0;
}
