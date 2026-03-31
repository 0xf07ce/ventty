//
// ventty Widget — Label demo
//

#include <ventty/ventty.h>
#include <ventty/widget/Label.h>

#include <chrono>
#include <string>
#include <thread>

using namespace ventty;

int main()
{
    Terminal term;
    if (!term.init())
    {
        return 1;
    }

    Window screen(0, 0, term.cols(), term.rows());

    auto rebuild = [&]()
    {
        int const W = term.cols();
        int const H = term.rows();
        screen.resize(W, H);
        screen.clear(Style{ Colors::LIGHT_GRAY, Colors::BLUE });
    };

    rebuild();

    // Title label
    Label title("ventty::Label Demo");
    title.setStyle({ Colors::WHITE, Colors::BLUE, Attr::Bold });
    title.setAlign(Align::Center);

    // Left-aligned label
    Label leftLabel("Left aligned");
    leftLabel.setStyle({ Colors::YELLOW, Colors::BLUE });
    leftLabel.setAlign(Align::Left);

    // Center-aligned label
    Label centerLabel("Center aligned");
    centerLabel.setStyle({ Colors::LIGHT_GREEN, Colors::BLUE });
    centerLabel.setAlign(Align::Center);

    // Right-aligned label
    Label rightLabel("Right aligned");
    rightLabel.setStyle({ Colors::LIGHT_CYAN, Colors::BLUE });
    rightLabel.setAlign(Align::Right);

    // Korean label
    Label koreanLabel("한글 라벨 테스트");
    koreanLabel.setStyle({ Colors::WHITE, Colors::RED });
    koreanLabel.setAlign(Align::Center);

    // Counter label
    Label counterLabel("Counter: 0");
    counterLabel.setStyle({ Colors::BLACK, Colors::CYAN });
    counterLabel.setAlign(Align::Center);

    // Status label
    Label statusLabel("Press 'q' or ESC to quit");
    statusLabel.setStyle({ Colors::WHITE, Colors::DARK_GRAY });
    statusLabel.setAlign(Align::Center);

    auto layout = [&]()
    {
        int const W = term.cols();
        int const H = term.rows();
        title.setRect(0, 1, W, 1);
        leftLabel.setRect(2, 4, W - 4, 1);
        centerLabel.setRect(2, 6, W - 4, 1);
        rightLabel.setRect(2, 8, W - 4, 1);
        koreanLabel.setRect(2, 10, W - 4, 1);
        counterLabel.setRect(2, 12, W - 4, 1);
        statusLabel.setRect(0, H - 1, W, 1);
    };

    layout();

    int counter = 0;
    bool running = true;

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
        {
            running = false;
        }
    });

    term.onResize([&](ResizeEvent const &)
    {
        rebuild();
        layout();
        term.forceRedraw();
    });

    auto lastTick = std::chrono::steady_clock::now();

    while (term.isRunning() && running)
    {
        while (term.pollEvent());

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastTick).count();

        if (elapsed > 500)
        {
            lastTick = now;
            ++counter;
            counterLabel.setText("Counter: " + std::to_string(counter));
        }

        screen.clear(Style{ Colors::LIGHT_GRAY, Colors::BLUE });
        title.draw(screen);
        leftLabel.draw(screen);
        centerLabel.draw(screen);
        rightLabel.draw(screen);
        koreanLabel.draw(screen);
        counterLabel.draw(screen);
        statusLabel.draw(screen);

        // Blit screen to terminal
        for (int y = 0; y < screen.height() && y < term.rows(); ++y)
        {
            for (int x = 0; x < screen.width() && x < term.cols(); ++x)
            {
                auto const & cell = screen.cellAt(x, y);
                term.putChar(x, y, cell.ch, cell.style);
            }
        }

        term.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    term.shutdown();
    return 0;
}
