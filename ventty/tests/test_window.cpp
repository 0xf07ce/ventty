#include <ventty/Window.h>
#include <ventty/Color.h>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testCreate()
{
    Window w(5, 10, 20, 8);
    assert(w.x() == 5);
    assert(w.y() == 10);
    assert(w.width() == 20);
    assert(w.height() == 8);
    assert(w.visible());
    assert(w.isDirty());
}

static void testClear()
{
    Window w(0, 0, 10, 5);
    w.putChar(3, 2, U'X', Colors::RED, Colors::BLACK);
    w.clear();
    auto const & c = w.cellAt(3, 2);
    assert(c.ch == U' ');
}

static void testClearWithBg()
{
    Window w(0, 0, 10, 5);
    w.clear(Colors::BLUE);
    auto const & c = w.cellAt(0, 0);
    assert(c.style.bg == Colors::BLUE);
}

static void testPutChar()
{
    Window w(0, 0, 20, 5);
    w.putChar(5, 2, U'A', Colors::WHITE, Colors::BLACK, Attr::Bold);
    auto const & c = w.cellAt(5, 2);
    assert(c.ch == U'A');
    assert(c.style.fg == Colors::WHITE);
    assert(hasAttr(c.style.attr, Attr::Bold));
}

static void testPutCharFullwidth()
{
    Window w(0, 0, 20, 5);
    w.putChar(3, 1, U'가', Colors::GREEN, Colors::BLACK);

    auto const & c = w.cellAt(3, 1);
    assert(c.ch == U'가');
    assert(c.width == 2);

    auto const & next = w.cellAt(4, 1);
    assert(next.ch == U'\0');
    assert(next.width == 0);
    assert(next.style.fg == Colors::GREEN);
}

static void testDrawText()
{
    Window w(0, 0, 30, 3);
    w.drawText(0, 0, "AB가C", Colors::WHITE, Colors::BLACK);

    assert(w.cellAt(0, 0).ch == U'A');
    assert(w.cellAt(1, 0).ch == U'B');
    assert(w.cellAt(2, 0).ch == U'가');
    assert(w.cellAt(3, 0).ch == U'\0');
    assert(w.cellAt(4, 0).ch == U'C');
}

static void testFill()
{
    Window w(0, 0, 10, 10);
    w.fill(2, 2, 3, 3, U'#', Colors::RED, Colors::BLUE);

    assert(w.cellAt(2, 2).ch == U'#');
    assert(w.cellAt(2, 2).style.fg == Colors::RED);
    assert(w.cellAt(2, 2).style.bg == Colors::BLUE);
    assert(w.cellAt(4, 4).ch == U'#');

    assert(w.cellAt(1, 1).ch == U' ');
    assert(w.cellAt(5, 5).ch == U' ');
}

static void testDrawBox()
{
    Window w(0, 0, 10, 5);
    w.drawBox(Colors::WHITE, Colors::BLACK, false);

    assert(w.cellAt(0, 0).ch == U'┌');
    assert(w.cellAt(9, 0).ch == U'┐');
    assert(w.cellAt(0, 4).ch == U'└');
    assert(w.cellAt(9, 4).ch == U'┘');
    assert(w.cellAt(5, 0).ch == U'─');
    assert(w.cellAt(0, 2).ch == U'│');
}

static void testDrawBoxDouble()
{
    Window w(0, 0, 10, 5);
    w.drawBox(Colors::WHITE, Colors::BLACK, true);

    assert(w.cellAt(0, 0).ch == U'╔');
    assert(w.cellAt(9, 0).ch == U'╗');
}

static void testVisibility()
{
    Window w(0, 0, 10, 5);
    assert(w.visible());
    w.setVisible(false);
    assert(!w.visible());
}

static void testZOrder()
{
    Window w(0, 0, 10, 5);
    assert(w.zOrder() == 0);
    w.setZOrder(5);
    assert(w.zOrder() == 5);
}

static void testPosition()
{
    Window w(10, 20, 5, 5);
    assert(w.x() == 10);
    assert(w.y() == 20);
    w.setPosition(30, 40);
    assert(w.x() == 30);
    assert(w.y() == 40);
}

static void testResize()
{
    Window w(0, 0, 10, 5);
    w.putChar(3, 3, U'X', Colors::WHITE, Colors::BLACK);
    w.resize(20, 10);
    assert(w.width() == 20);
    assert(w.height() == 10);
    assert(w.cellAt(3, 3).ch == U' ');
}

static void testPrint()
{
    Window w(0, 0, 10, 3);
    w.print("Hello\n", Colors::WHITE, Colors::BLACK);
    w.print("World", Colors::WHITE, Colors::BLACK);

    assert(w.cellAt(0, 0).ch == U'H');
    assert(w.cellAt(4, 0).ch == U'o');
    assert(w.cellAt(0, 1).ch == U'W');
}

static void testPrintWrap()
{
    Window w(0, 0, 5, 3);
    w.print("ABCDEFGH", Colors::WHITE, Colors::BLACK);

    assert(w.cellAt(0, 0).ch == U'A');
    assert(w.cellAt(4, 0).ch == U'E');
    assert(w.cellAt(0, 1).ch == U'F');
}

static void testScrollback()
{
    Window w(0, 0, 10, 3);
    w.enableScroll(100);

    w.print("Line1\n", Colors::WHITE, Colors::BLACK);
    w.print("Line2\n", Colors::WHITE, Colors::BLACK);
    w.print("Line3\n", Colors::WHITE, Colors::BLACK);

    assert(w.scrollbackSize() >= 1);
    assert(w.cellAt(0, 0).ch == U'L');
}

static void testOutOfBounds()
{
    Window w(0, 0, 5, 5);
    w.putChar(-1, -1, U'X', Colors::WHITE, Colors::BLACK);
    w.putChar(100, 100, U'X', Colors::WHITE, Colors::BLACK);
    w.drawText(0, -5, "test", Colors::WHITE, Colors::BLACK);
}

static void testDirtyFlag()
{
    Window w(0, 0, 10, 5);
    assert(w.isDirty());
    w.clearDirty();
    assert(!w.isDirty());
    w.putChar(0, 0, U'X', Colors::WHITE, Colors::BLACK);
    assert(w.isDirty());
}

int main()
{
    testCreate();
    testClear();
    testClearWithBg();
    testPutChar();
    testPutCharFullwidth();
    testDrawText();
    testFill();
    testDrawBox();
    testDrawBoxDouble();
    testVisibility();
    testZOrder();
    testPosition();
    testResize();
    testPrint();
    testPrintWrap();
    testScrollback();
    testOutOfBounds();
    testDirtyFlag();

    std::printf("test_window: all tests passed\n");
    return 0;
}
