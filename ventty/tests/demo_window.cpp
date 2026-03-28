//
// Interactive demo: multiple overlapping windows
//

#include <ventty/ventty.h>

#include <chrono>
#include <thread>
#include <string>

using namespace ventty;

int main()
{
    AnsiTerminal term;
    if (!term.init())
        return 1;

    Color const darkBg1{20, 20, 40};
    Color const darkBg2{40, 20, 20};
    Color const darkBg3{20, 40, 20};

    // Background pattern
    for (int y = 0; y < term.rows(); ++y)
        for (int x = 0; x < term.cols(); ++x)
        {
            char32_t cp = ((x + y) % 2 == 0) ? U'·' : U' ';
            term.putChar(x, y, cp, Colors::DARK_GRAY, Colors::BLACK);
        }

    // Window 1: Info panel (single border)
    auto * w1 = term.createWindow(3, 2, 30, 10);
    w1->drawBox(Colors::CYAN, Colors::BLACK, false);
    w1->fill(1, 1, 28, 8, U' ', Colors::WHITE, darkBg1);
    w1->drawText(2, 0, " Window 1 ", Colors::YELLOW, Colors::BLACK, Attr::Bold);
    w1->drawText(2, 2, "Single border box", Colors::WHITE, darkBg1);
    w1->drawText(2, 3, "Tab: cycle focus", Colors::LIGHT_GRAY, darkBg1);
    w1->drawText(2, 4, "Arrows: move", Colors::LIGHT_GRAY, darkBg1);
    w1->setZOrder(1);

    // Window 2: Double border
    auto * w2 = term.createWindow(20, 6, 30, 10);
    w2->drawBox(Colors::MAGENTA, Colors::BLACK, true);
    w2->fill(1, 1, 28, 8, U' ', Colors::WHITE, darkBg2);
    w2->drawText(2, 0, " Window 2 ", Colors::YELLOW, Colors::BLACK, Attr::Bold);
    w2->drawText(2, 2, "Double border box", Colors::WHITE, darkBg2);
    w2->drawText(2, 4, "가나다라마바사", Colors::GREEN, darkBg2);
    w2->drawText(2, 5, "Overlapping OK!", Colors::CYAN, darkBg2);
    w2->setZOrder(2);

    // Window 3: Small floating
    auto * w3 = term.createWindow(40, 3, 25, 8);
    w3->drawBox(Colors::GREEN, Colors::BLACK, false);
    w3->fill(1, 1, 23, 6, U' ', Colors::WHITE, darkBg3);
    w3->drawText(2, 0, " Window 3 ", Colors::YELLOW, Colors::BLACK, Attr::Bold);
    w3->drawText(2, 2, "Show/Hide: 1,2,3", Colors::WHITE, darkBg3);
    w3->setZOrder(3);

    Window * windows[] = {w1, w2, w3};
    int focused = 0;
    int const WIN_COUNT = 3;

    Color const focusColors[] = {Colors::CYAN, Colors::MAGENTA, Colors::GREEN};

    auto highlight = [&]()
    {
        for (int i = 0; i < WIN_COUNT; ++i)
        {
            bool dbl = (i == 1);
            Color fc = (i == focused) ? Colors::WHITE : focusColors[i];
            windows[i]->drawBox(fc, Colors::BLACK, dbl);
        }
        for (int i = 0; i < WIN_COUNT; ++i)
            windows[i]->setZOrder(i == focused ? 10 : i);
    };

    highlight();

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
        {
            term.quit();
            return;
        }

        if (ev.key == KeyEvent::Key::Tab)
        {
            focused = (focused + 1) % WIN_COUNT;
            highlight();
            return;
        }

        if (ev.key == KeyEvent::Key::Char && ev.ch >= U'1' && ev.ch <= U'3')
        {
            int idx = static_cast<int>(ev.ch - U'1');
            windows[idx]->setVisible(!windows[idx]->visible());
            return;
        }

        auto * w = windows[focused];
        int wx = w->x();
        int wy = w->y();

        if (ev.key == KeyEvent::Key::Up)    wy--;
        if (ev.key == KeyEvent::Key::Down)  wy++;
        if (ev.key == KeyEvent::Key::Left)  wx--;
        if (ev.key == KeyEvent::Key::Right) wx++;

        w->setPosition(wx, wy);
    });

    term.onResize([&](ResizeEvent const &)
    {
        term.forceRedraw();
        for (int y = 0; y < term.rows(); ++y)
            for (int x = 0; x < term.cols(); ++x)
            {
                char32_t cp = ((x + y) % 2 == 0) ? U'·' : U' ';
                term.putChar(x, y, cp, Colors::DARK_GRAY, Colors::BLACK);
            }
    });

    // Status bar
    term.fill(0, term.rows() - 1, term.cols(), 1, U' ', Colors::BLACK, Colors::CYAN);
    term.drawText(1, term.rows() - 1,
                 "Tab: focus | Arrows: move | 1-3: toggle | q: quit",
                 Colors::WHITE, Colors::CYAN, Attr::Bold);

    while (term.isRunning())
    {
        while (term.pollEvent())
            ;
        term.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    term.shutdown();
    return 0;
}
