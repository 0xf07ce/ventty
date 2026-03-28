#include <ventty/Screen.h>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testResize()
{
    Screen s;
    s.resize(80, 24);
    assert(s.width() == 80);
    assert(s.height() == 24);
}

static void testClear()
{
    Screen s;
    s.resize(10, 5);
    s.setStyle({Colors::RED, Colors::BLUE});
    s.setCursor(3, 2);
    s.putChar(U'X');
    s.clear();

    assert(s.at(3, 2).ch == U' ');
    assert(s.cursorX() == 0);
    assert(s.cursorY() == 0);
}

static void testClearWithStyle()
{
    Screen s;
    s.resize(10, 5);
    Style st{Colors::GREEN, Colors::MAGENTA};
    s.clear(st);

    assert(s.at(0, 0).style.fg == Colors::GREEN);
    assert(s.at(0, 0).style.bg == Colors::MAGENTA);
}

static void testPutChar()
{
    Screen s;
    s.resize(20, 5);
    s.setStyle({Colors::WHITE, Colors::BLACK, Attr::Bold});
    s.setCursor(5, 2);
    s.putChar(U'A');

    auto const & c = s.at(5, 2);
    assert(c.ch == U'A');
    assert(c.style.fg == Colors::WHITE);
    assert(hasAttr(c.style.attr, Attr::Bold));
    assert(s.cursorX() == 6);
}

static void testPutCharFullwidth()
{
    Screen s;
    s.resize(20, 5);
    s.setStyle({Colors::GREEN, Colors::BLACK});
    s.setCursor(3, 1);
    s.putChar(U'가');

    assert(s.at(3, 1).ch == U'가');
    assert(s.at(3, 1).width == 2);
    assert(s.at(4, 1).ch == 0);
    assert(s.at(4, 1).width == 0);
    assert(s.cursorX() == 5);
}

static void testPutString()
{
    Screen s;
    s.resize(30, 3);
    s.setStyle({Colors::WHITE, Colors::BLACK});
    s.setCursor(0, 0);
    s.putString("AB가C");

    assert(s.at(0, 0).ch == U'A');
    assert(s.at(1, 0).ch == U'B');
    assert(s.at(2, 0).ch == U'가');
    assert(s.at(3, 0).ch == 0);
    assert(s.at(4, 0).ch == U'C');
}

static void testPutStringAt()
{
    Screen s;
    s.resize(20, 5);
    s.setStyle({Colors::WHITE, Colors::BLACK});
    s.putStringAt(5, 2, "Hello");

    assert(s.at(5, 2).ch == U'H');
    assert(s.at(9, 2).ch == U'o');
}

static void testDrawBox()
{
    Screen s;
    s.resize(20, 10);
    s.setStyle({Colors::WHITE, Colors::BLACK});
    s.drawBox(0, 0, 10, 5);

    assert(s.at(0, 0).ch == U'┌');
    assert(s.at(9, 0).ch == U'┐');
    assert(s.at(0, 4).ch == U'└');
    assert(s.at(9, 4).ch == U'┘');
    assert(s.at(5, 0).ch == U'─');
    assert(s.at(0, 2).ch == U'│');
}

static void testDrawBoxDouble()
{
    Screen s;
    s.resize(20, 10);
    s.setStyle({Colors::WHITE, Colors::BLACK});
    s.drawBoxDouble(0, 0, 10, 5);

    assert(s.at(0, 0).ch == U'╔');
    assert(s.at(9, 0).ch == U'╗');
    assert(s.at(0, 4).ch == U'╚');
    assert(s.at(9, 4).ch == U'╝');
}

static void testFillRect()
{
    Screen s;
    s.resize(10, 10);
    s.setStyle({Colors::RED, Colors::BLUE});
    s.fillRect(2, 2, 3, 3, U'#');

    assert(s.at(2, 2).ch == U'#');
    assert(s.at(4, 4).ch == U'#');
    assert(s.at(1, 1).ch == U' ');
}

static void testOutOfBounds()
{
    Screen s;
    s.resize(5, 5);
    // Should not crash
    auto const & c = s.at(-1, -1);
    (void)c;
    auto const & d = s.at(100, 100);
    (void)d;
}

static void testSetCursor()
{
    Screen s;
    s.resize(10, 5);
    s.setCursor(3, 2);
    assert(s.cursorX() == 3);
    assert(s.cursorY() == 2);

    // Clamping
    s.setCursor(100, 100);
    assert(s.cursorX() == 9);
    assert(s.cursorY() == 4);
}

int main()
{
    testResize();
    testClear();
    testClearWithStyle();
    testPutChar();
    testPutCharFullwidth();
    testPutString();
    testPutStringAt();
    testDrawBox();
    testDrawBoxDouble();
    testFillRect();
    testOutOfBounds();
    testSetCursor();

    std::printf("test_screen: all tests passed\n");
    return 0;
}
