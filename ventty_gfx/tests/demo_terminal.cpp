//
// ventty::gfx Terminal — interactive SDL3 demo
//

#include <ventty_gfx/ventty_gfx.h>

#include <chrono>
#include <cstdio>
#include <string>
#include <thread>

using namespace ventty;

int main()
{
    gfx::GfxTerminal term;
    if (!term.init(80, 25, "ventty::gfx Demo", "assets/fonts/IyagiGGC.ttf", 16.0f))
    {
        std::fprintf(stderr, "Failed to initialize terminal\n");
        return 1;
    }

    Style const cyan { Colors::CYAN, Colors::BLACK };
    Style const yellowBold { Colors::YELLOW, Colors::BLACK, Attr::Bold };
    Style const green { Colors::GREEN, Colors::BLACK };
    Style const gray { Colors::LIGHT_GRAY, Colors::BLACK };
    Style const white { Colors::WHITE, Colors::BLACK };
    Style const whiteBold { Colors::WHITE, Colors::BLACK, Attr::Bold };
    Style const magenta { Colors::MAGENTA, Colors::BLACK };

    auto * infoWin = term.createWindow(2, 1, 40, 10);
    infoWin->drawBox(cyan);
    infoWin->drawText(2, 0, " gfx SDL3 Terminal ", yellowBold);
    infoWin->drawText(2, 2, "Arrow keys: move box", gray);
    infoWin->drawText(2, 3, "한글 테스트: 안녕하세요!", green);
    infoWin->drawText(2, 4, "Press 'q' or ESC to quit", gray);

    auto * colorWin = term.createWindow(2, 12, 40, 6);
    colorWin->drawBox(magenta);
    colorWin->drawText(2, 0, " 24-bit Color ", whiteBold);
    for (int x = 1; x < 39; ++x)
    {
        float t = static_cast<float>(x - 1) / 37.0f;
        Color c {
            static_cast<uint8_t>(t * 255),
            static_cast<uint8_t>((1.0f - t) * 128),
            static_cast<uint8_t>(128 + t * 127)
        };
        Style cs { c, Colors::BLACK };
        colorWin->putChar(x, 2, U'█', cs);
        colorWin->putChar(x, 3, U'▓', cs);
        colorWin->putChar(x, 4, U'░', cs);
    }

    int spinIdx = 0;
    float progress = 0.0f;
    int boxX = 50;
    int boxY = 5;

    auto * moveBox = term.createWindow(boxX, boxY, 10, 5);
    moveBox->drawBox(Style { Colors::YELLOW, Colors::BLACK });
    moveBox->drawText(1, 2, "Move me", white);
    moveBox->setZOrder(10);

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
        {
            term.quit();
            return;
        }

        bool moved = false;
        if (ev.key == KeyEvent::Key::Up)
        {
            boxY--;
            moved = true;
        }
        if (ev.key == KeyEvent::Key::Down)
        {
            boxY++;
            moved = true;
        }
        if (ev.key == KeyEvent::Key::Left)
        {
            boxX--;
            moved = true;
        }
        if (ev.key == KeyEvent::Key::Right)
        {
            boxX++;
            moved = true;
        }

        if (moved)
        {
            if (boxX < 0) boxX = 0;
            if (boxY < 0) boxY = 0;
            if (boxX + moveBox->width() > term.cols())
                boxX = term.cols() - moveBox->width();
            if (boxY + moveBox->height() > term.rows())
                boxY = term.rows() - moveBox->height();
            moveBox->setPosition(boxX, boxY);
        }
    });

    auto lastTick = std::chrono::steady_clock::now();

    while (term.isRunning())
    {
        while (term.pollEvent());

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastTick).count();

        if (elapsed > 80)
        {
            lastTick = now;
            spinIdx = (spinIdx + 1) % static_cast<int>(ascii::SPINNER_BRAILLE.size());
            progress += 0.005f;
            if (progress > 1.0f)
                progress = 0.0f;
        }

        auto const & frame = ascii::SPINNER_BRAILLE[static_cast<size_t>(spinIdx)];
        infoWin->drawText(2, 6, std::string(frame) + " Working...", cyan);

        auto bar = ascii::progressBar(36, progress);
        infoWin->drawText(2, 8, bar, green);

        std::string status = " ventty::gfx | " +
            std::to_string(term.cols()) + "x" + std::to_string(term.rows()) +
            " | Box@" + std::to_string(boxX) + "," + std::to_string(boxY) + " ";
        Style statusStyle { Colors::WHITE, Colors::CYAN, Attr::Bold };
        term.fill(0, term.rows() - 1, term.cols(), 1, U' ', Colors::BLACK, Colors::CYAN);
        term.drawText(0, term.rows() - 1, status, statusStyle);

        term.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    term.shutdown();
    return 0;
}
