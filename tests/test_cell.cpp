#include <ventty/core/Cell.hpp>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testDefaults()
{
    Cell c;
    assert(c.ch == U' ');
    assert(c.style.fg == Colors::LIGHT_GRAY);
    assert(c.style.bg == Colors::BLACK);
    assert(c.style.attr == Attr::None);
    assert(c.width == 1);
}

static void testEquality()
{
    Cell a;
    Cell b;
    assert(a == b);

    a.ch = U'X';
    assert(a != b);

    b.ch = U'X';
    assert(a == b);
}

static void testStyleInCell()
{
    Cell c;
    c.style = Style{Colors::RED, Colors::BLUE, Attr::Bold | Attr::Underline};
    assert(c.style.fg == Colors::RED);
    assert(hasAttr(c.style.attr, Attr::Bold));
    assert(hasAttr(c.style.attr, Attr::Underline));
    assert(!hasAttr(c.style.attr, Attr::Blink));
}

static void testWidth()
{
    Cell c;
    assert(c.width == 1);
    c.width = 2;
    assert(c.width == 2);

    Cell d;
    assert(c != d);  // width differs (2 vs 1)
}

int main()
{
    testDefaults();
    testEquality();
    testStyleInCell();
    testWidth();

    std::printf("test_cell: all tests passed\n");
    return 0;
}
