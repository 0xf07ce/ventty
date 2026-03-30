//
// ventty Widget — Menu & MenuBar demo
//

#include <ventty/ventty.h>
#include <ventty/widget/Label.h>
#include <ventty/widget/Menu.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>

using namespace ventty;
using Key = KeyEvent::Key;

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
        screen.resize(term.cols(), term.rows());
    };
    rebuild();

    Label title("ventty::Menu Demo");
    title.setStyle({Colors::WHITE, Colors::BLUE, Attr::Bold});
    title.setAlign(Align::Center);

    Label actionLabel("(no action yet)");
    actionLabel.setStyle({Colors::YELLOW, Colors::BLUE});
    actionLabel.setAlign(Align::Center);

    Label statusLabel("F12 or Alt+F/E/H: open menu | Arrows: navigate | Enter: select | q: quit");
    statusLabel.setStyle({Colors::WHITE, Colors::DARK_GRAY});
    statusLabel.setAlign(Align::Center);

    // File menu
    auto fileMenu = std::make_shared<Menu>();
    fileMenu->addItem({"New",    "Ctrl+N", Key::None, false, true,
        [&]() { actionLabel.setText("Action: New"); }});
    fileMenu->addItem({"Open",   "Ctrl+O", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Open"); }});
    fileMenu->addItem({"Save",   "Ctrl+S", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Save"); }});
    fileMenu->addSeparator();
    fileMenu->addItem({"Quit",   "Ctrl+Q", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Quit (not really)"); }});

    // Edit menu
    auto editMenu = std::make_shared<Menu>();
    editMenu->addItem({"Undo",   "Ctrl+Z", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Undo"); }});
    editMenu->addItem({"Redo",   "Ctrl+Y", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Redo"); }});
    editMenu->addSeparator();
    editMenu->addItem({"Cut",    "Ctrl+X", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Cut"); }});
    editMenu->addItem({"Copy",   "Ctrl+C", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Copy"); }});
    editMenu->addItem({"Paste",  "Ctrl+V", Key::None, false, true,
        [&]() { actionLabel.setText("Action: Paste"); }});
    editMenu->addSeparator();
    editMenu->addItem({"Disabled Item", "", Key::None, false, false, nullptr});

    // Help menu
    auto helpMenu = std::make_shared<Menu>();
    helpMenu->addItem({"About",   "", Key::None, false, true,
        [&]() { actionLabel.setText("ventty widget framework"); }});
    helpMenu->addItem({"Version", "", Key::None, false, true,
        [&]() { actionLabel.setText("Version 0.1.0"); }});

    MenuBar menuBar;
    menuBar.addMenu("File", fileMenu);
    menuBar.addMenu("Edit", editMenu);
    menuBar.addMenu("Help", helpMenu);

    auto layout = [&]()
    {
        int const W = term.cols();
        int const H = term.rows();
        menuBar.setRect(0, 0, W, 1);
        title.setRect(0, 3, W, 1);
        actionLabel.setRect(0, H / 2, W, 1);
        statusLabel.setRect(0, H - 1, W, 1);
    };
    layout();

    bool running = true;

    term.onKey([&](KeyEvent const & ev)
    {
        // MenuBar gets first crack at input
        if (menuBar.handleKey(ev))
        {
            return;
        }

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

    while (term.isRunning() && running)
    {
        while (term.pollEvent())
            ;

        screen.clear(Style{Colors::LIGHT_GRAY, Colors::BLUE});

        title.draw(screen);
        actionLabel.draw(screen);
        statusLabel.draw(screen);
        menuBar.draw(screen);

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
