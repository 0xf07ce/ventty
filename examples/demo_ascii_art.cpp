//
// Interactive demo: ASCII art presets — spinners, progress bars, box drawing
//

#include <ventty/ventty.h>

#include <chrono>
#include <cmath>
#include <string>
#include <thread>

using namespace ventty;

int main()
{
    Terminal term;
    if (!term.init())
        return 1;

    Style const cyan { Colors::CYAN, Colors::BLACK };
    Style const yellowBold { Colors::YELLOW, Colors::BLACK, Attr::Bold };
    Style const whiteBold { Colors::WHITE, Colors::BLACK, Attr::Bold };
    Style const white { Colors::WHITE, Colors::BLACK };
    Style const green { Colors::GREEN, Colors::BLACK };
    Style const magenta { Colors::MAGENTA, Colors::BLACK };
    Style const yellow { Colors::YELLOW, Colors::BLACK };
    Style const red { Colors::LIGHT_RED, Colors::BLACK };
    Style const gray { Colors::LIGHT_GRAY, Colors::BLACK };

    // Main panel
    auto * panel = term.createWindow(2, 1, 60, 30);
    panel->drawBox(cyan);
    panel->drawText(2, 0, " ASCII Art Presets ", yellowBold);

    int frame = 0;
    float progress = 0.0f;

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
            term.quit();
    });

    while (term.isRunning())
    {
        while (term.pollEvent());

        // Clear interior
        panel->fill(1, 1, 58, 28, U' ', white);

        // -- Spinners --
        panel->drawText(2, 2, "Spinners:", whiteBold);

        int si = frame % static_cast<int>(SPINNER_BRAILLE.size());
        panel->drawText(4, 4,
            std::string("Braille: ") + std::string(SPINNER_BRAILLE[static_cast<size_t>(si)]) + " Loading...",
            cyan);

        si = frame % static_cast<int>(SPINNER_LINE.size());
        panel->drawText(4, 5,
            std::string("Line:    ") + std::string(SPINNER_LINE[static_cast<size_t>(si)]) + " Working...",
            green);

        si = frame % static_cast<int>(SPINNER_BLOCK.size());
        panel->drawText(4, 6,
            std::string("Block:   ") + std::string(SPINNER_BLOCK[static_cast<size_t>(si)]) + " Building...",
            magenta);

        si = frame % static_cast<int>(SPINNER_DOTS.size());
        panel->drawText(4, 7,
            std::string("Dots:    ") + std::string(SPINNER_DOTS[static_cast<size_t>(si)]),
            yellow);

        // -- Progress Bars --
        panel->drawText(2, 9, "Progress Bars:", whiteBold);

        auto bar1 = progressBar(40, progress, PROGRESS_BLOCK);
        panel->drawText(4, 11, "Block:  " + bar1, green);

        auto bar2 = progressBar(40, progress, PROGRESS_SMOOTH);
        panel->drawText(4, 12, "Smooth: " + bar2, cyan);

        auto bar3 = progressBar(40, progress, PROGRESS_HASH);
        panel->drawText(4, 13, "Hash:   " + bar3, yellow);

        // -- Box Drawing Styles --
        panel->drawText(2, 15, "Box Styles:", whiteBold);

        auto drawMiniBox = [&](int ox, int oy, BoxChars const & bc, std::string_view label, Style const & st)
        {
            panel->putChar(ox, oy, bc.tl, st);
            for (int i = 1; i < 10; ++i)
                panel->putChar(ox + i, oy, bc.h, st);
            panel->putChar(ox + 10, oy, bc.tr, st);
            for (int j = 1; j < 3; ++j)
            {
                panel->putChar(ox, oy + j, bc.v, st);
                panel->putChar(ox + 10, oy + j, bc.v, st);
            }
            panel->putChar(ox, oy + 3, bc.bl, st);
            for (int i = 1; i < 10; ++i)
                panel->putChar(ox + i, oy + 3, bc.h, st);
            panel->putChar(ox + 10, oy + 3, bc.br, st);
            panel->drawText(ox + 2, oy + 1, label, st);
        };

        drawMiniBox(4, 17, SINGLE_BOX, "Single", white);
        drawMiniBox(18, 17, DOUBLE_BOX, "Double", yellow);
        drawMiniBox(32, 17, ROUND_BOX, "Round", cyan);
        drawMiniBox(46, 17, HEAVY_BOX, "Heavy", red);

        // -- Shade/Block characters --
        panel->drawText(2, 22, "Shades & Bars:", whiteBold);

        panel->putChar(4, 24, SHADE_LIGHT, white);
        panel->putChar(5, 24, SHADE_MEDIUM, white);
        panel->putChar(6, 24, SHADE_DARK, white);
        panel->putChar(7, 24, SHADE_FULL, white);
        panel->drawText(9, 24, "Shades", gray);

        for (int i = 0; i < 9; ++i)
            panel->putChar(4 + i, 26, VBAR[static_cast<size_t>(i)], green);
        panel->drawText(14, 26, "VBar 1/8 increments", gray);

        for (int i = 0; i < 9; ++i)
            panel->putChar(4 + i, 27, HBAR[static_cast<size_t>(i)], cyan);
        panel->drawText(14, 27, "HBar 1/8 increments", gray);

        // Status bar
        term.fill(0, term.rows() - 1, term.cols(), 1, U' ', Colors::BLACK, Colors::MAGENTA);
        term.drawText(1, term.rows() - 1,
            "ASCII Art Demo | Press 'q' to quit",
            Colors::WHITE, Colors::MAGENTA, Attr::Bold);

        term.render();

        frame++;
        progress += 0.003f;
        if (progress > 1.0f)
            progress = 0.0f;

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }

    term.shutdown();
    return 0;
}
