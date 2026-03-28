#include <ventty/AsciiArt.h>
#include <ventty/Utf8.h>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testBoxChars()
{
    assert(ascii::SINGLE_BOX.tl == U'┌');
    assert(ascii::SINGLE_BOX.tr == U'┐');
    assert(ascii::DOUBLE_BOX.tl == U'╔');
    assert(ascii::ROUND_BOX.tl == U'╭');
    assert(ascii::HEAVY_BOX.tl == U'┏');
}

static void testSpinnerFrames()
{
    assert(ascii::SPINNER_BRAILLE.size() == 10);
    assert(ascii::SPINNER_LINE.size() == 4);
    assert(ascii::SPINNER_BLOCK.size() == 4);
    assert(ascii::SPINNER_MOON.size() == 8);
    assert(ascii::SPINNER_DOTS.size() == 4);

    for (auto const & f : ascii::SPINNER_BRAILLE)
        assert(!f.empty());
}

static void testProgressBar()
{
    auto bar0 = ascii::progressBar(20, 0.0f);
    auto bar50 = ascii::progressBar(20, 0.5f);
    auto bar100 = ascii::progressBar(20, 1.0f);

    assert(utf8::stringWidth(bar0) == 20);
    assert(utf8::stringWidth(bar50) == 20);
    assert(utf8::stringWidth(bar100) == 20);
}

static void testProgressBarStyles()
{
    auto block  = ascii::progressBar(30, 0.7f, ascii::PROGRESS_BLOCK);
    auto smooth = ascii::progressBar(30, 0.7f, ascii::PROGRESS_SMOOTH);
    auto hash   = ascii::progressBar(30, 0.7f, ascii::PROGRESS_HASH);

    assert(utf8::stringWidth(block) == 30);
    assert(utf8::stringWidth(smooth) == 30);
    assert(utf8::stringWidth(hash) == 30);

    assert(block != smooth);
    assert(block != hash);
}

static void testProgressBarEdgeCases()
{
    auto barNeg = ascii::progressBar(10, -0.5f);
    auto bar0   = ascii::progressBar(10, 0.0f);
    assert(barNeg == bar0);

    auto barOver = ascii::progressBar(10, 1.5f);
    auto bar1    = ascii::progressBar(10, 1.0f);
    assert(barOver == bar1);
}

static void testVBarHBar()
{
    assert(ascii::VBAR.size() == 9);
    assert(ascii::HBAR.size() == 9);
    assert(ascii::VBAR[0] == U' ');
    assert(ascii::VBAR[8] == U'█');
    assert(ascii::HBAR[0] == U' ');
    assert(ascii::HBAR[8] == U'█');
}

static void testShadeChars()
{
    assert(ascii::SHADE_LIGHT == U'░');
    assert(ascii::SHADE_MEDIUM == U'▒');
    assert(ascii::SHADE_DARK == U'▓');
    assert(ascii::SHADE_FULL == U'█');
}

static void testBraillePlot()
{
    bool empty[2][4] = {};
    assert(ascii::BraillePlot::fromDots(empty) == U'\u2800');

    auto cp = ascii::BraillePlot::setDot(U'\u2800', 0, 0);
    assert(cp == U'\u2801');

    cp = ascii::BraillePlot::setDot(U'\u2800', 1, 0);
    assert(cp == U'\u2808');
}

static void testHrConstants()
{
    assert(ascii::HR_THIN == U'─');
    assert(ascii::HR_THICK == U'━');
    assert(ascii::HR_DOUBLE == U'═');
}

int main()
{
    testBoxChars();
    testSpinnerFrames();
    testProgressBar();
    testProgressBarStyles();
    testProgressBarEdgeCases();
    testVBarHBar();
    testShadeChars();
    testBraillePlot();
    testHrConstants();

    std::printf("test_ascii_art: all tests passed\n");
    return 0;
}
