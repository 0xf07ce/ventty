#include <ventty/art/AsciiArt.h>
#include <ventty/core/Utf8.h>

#include <cassert>
#include <cstdio>

using namespace ventty;

static void testBoxChars()
{
    assert(SINGLE_BOX.tl == U'┌');
    assert(SINGLE_BOX.tr == U'┐');
    assert(DOUBLE_BOX.tl == U'╔');
    assert(ROUND_BOX.tl == U'╭');
    assert(HEAVY_BOX.tl == U'┏');
}

static void testSpinnerFrames()
{
    assert(SPINNER_BRAILLE.size() == 10);
    assert(SPINNER_LINE.size() == 4);
    assert(SPINNER_BLOCK.size() == 4);
    assert(SPINNER_MOON.size() == 8);
    assert(SPINNER_DOTS.size() == 4);

    for (auto const & f : SPINNER_BRAILLE)
        assert(!f.empty());
}

static void testProgressBar()
{
    auto bar0 = progressBar(20, 0.0f);
    auto bar50 = progressBar(20, 0.5f);
    auto bar100 = progressBar(20, 1.0f);

    assert(stringWidth(bar0) == 20);
    assert(stringWidth(bar50) == 20);
    assert(stringWidth(bar100) == 20);
}

static void testProgressBarStyles()
{
    auto block  = progressBar(30, 0.7f, PROGRESS_BLOCK);
    auto smooth = progressBar(30, 0.7f, PROGRESS_SMOOTH);
    auto hash   = progressBar(30, 0.7f, PROGRESS_HASH);

    assert(stringWidth(block) == 30);
    assert(stringWidth(smooth) == 30);
    assert(stringWidth(hash) == 30);

    assert(block != smooth);
    assert(block != hash);
}

static void testProgressBarEdgeCases()
{
    auto barNeg = progressBar(10, -0.5f);
    auto bar0   = progressBar(10, 0.0f);
    assert(barNeg == bar0);

    auto barOver = progressBar(10, 1.5f);
    auto bar1    = progressBar(10, 1.0f);
    assert(barOver == bar1);
}

static void testVBarHBar()
{
    assert(VBAR.size() == 9);
    assert(HBAR.size() == 9);
    assert(VBAR[0] == U' ');
    assert(VBAR[8] == U'█');
    assert(HBAR[0] == U' ');
    assert(HBAR[8] == U'█');
}

static void testShadeChars()
{
    assert(SHADE_LIGHT == U'░');
    assert(SHADE_MEDIUM == U'▒');
    assert(SHADE_DARK == U'▓');
    assert(SHADE_FULL == U'█');
}

static void testBraillePlot()
{
    bool empty[2][4] = {};
    assert(BraillePlot::fromDots(empty) == U'\u2800');

    auto cp = BraillePlot::setDot(U'\u2800', 0, 0);
    assert(cp == U'\u2801');

    cp = BraillePlot::setDot(U'\u2800', 1, 0);
    assert(cp == U'\u2808');
}

static void testHrConstants()
{
    assert(HR_THIN == U'─');
    assert(HR_THICK == U'━');
    assert(HR_DOUBLE == U'═');
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
