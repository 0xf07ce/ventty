//
// Interactive demo: 24-bit color gradients
// Press ESC or 'q' to quit
//

#include <ventty/ventty.h>

#include <cmath>
#include <chrono>
#include <thread>

using namespace ventty;

int main()
{
    Terminal term;
    if (!term.init())
        return 1;

    int frame = 0;

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
            term.quit();
    });

    while (term.isRunning())
    {
        while (term.pollEvent())
            ;

        term.clear(Colors::BLACK);
        int cols = term.cols();
        int rows = term.rows();

        // RGB gradient — animated hue shift
        for (int y = 0; y < rows - 2; ++y)
        {
            for (int x = 0; x < cols; ++x)
            {
                float fx = static_cast<float>(x) / static_cast<float>(cols);
                float fy = static_cast<float>(y) / static_cast<float>(rows - 2);
                float phase = static_cast<float>(frame) * 0.02f;

                auto r = static_cast<uint8_t>(128 + 127 * std::sin(fx * 6.28f + phase));
                auto g = static_cast<uint8_t>(128 + 127 * std::sin(fy * 6.28f + phase + 2.09f));
                auto b = static_cast<uint8_t>(128 + 127 * std::sin((fx + fy) * 3.14f + phase + 4.18f));

                term.putChar(x, y, U'▓', Color{r, g, b}, Colors::BLACK);
            }
        }

        // Status bar
        term.fill(0, rows - 1, cols, 1, U' ', Colors::BLACK, Colors::BLUE);
        term.drawText(1, rows - 1, "24-bit True Color Demo | Press 'q' to quit",
                     Colors::WHITE, Colors::BLUE, Attr::Bold);

        // xterm256 palette row
        int y256 = rows - 2;
        term.fill(0, y256, cols, 1, U' ', Colors::BLACK, Colors::BLACK);
        for (int i = 0; i < 256 && i < cols; ++i)
            term.putChar(i, y256, U'█', Palette::xterm256(static_cast<uint8_t>(i)), Colors::BLACK);

        term.render();
        frame++;
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    term.shutdown();
    return 0;
}
