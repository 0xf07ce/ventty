# Quick Start

```cpp
#include <ventty/ventty.h>

int main()
{
    ventty::Terminal term;
    if (!term.init()) return 1;

    term.onKey([&](ventty::KeyEvent const & ev)
    {
        if (ev.key == ventty::KeyEvent::Key::Escape)
            term.quit();
    });

    auto * win = term.createWindow(2, 1, 40, 10);
    ventty::Style style{ventty::Color(0, 255, 128), ventty::Color(0, 0, 0)};
    win->drawText(1, 1, "Hello, ventty!", style);

    while (term.isRunning())
    {
        while (term.pollEvent()) ;
        term.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    term.shutdown();
    return 0;
}
```
