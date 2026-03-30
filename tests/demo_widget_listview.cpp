//
// ventty Widget — ListView demo
//

#include <ventty/ventty.h>
#include <ventty/widget/Label.h>
#include <ventty/widget/ListView.h>

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
        screen.resize(term.cols(), term.rows());
        screen.clear(Style{Colors::LIGHT_GRAY, Colors::BLUE});
    };
    rebuild();

    // Title
    Label title("ventty::ListView Demo");
    title.setStyle({Colors::WHITE, Colors::BLUE, Attr::Bold});
    title.setAlign(Align::Center);

    // Left list — fruits
    ListView fruitList;
    fruitList.setFocused(true);
    fruitList.setNormalStyle({Colors::LIGHT_GRAY, Colors::BLACK});
    fruitList.setSelectedStyle({Colors::WHITE, Colors::RED});
    std::vector<ListItem> fruits = {
        {"Apple",       {Colors::LIGHT_GREEN, Colors::BLACK}},
        {"Banana",      {Colors::YELLOW, Colors::BLACK}},
        {"Cherry",      {Colors::LIGHT_RED, Colors::BLACK}},
        {"Dragonfruit", {Colors::LIGHT_MAGENTA, Colors::BLACK}},
        {"Elderberry",  {Colors::LIGHT_CYAN, Colors::BLACK}},
        {"Fig",         {Colors::BROWN, Colors::BLACK}},
        {"Grape",       {Colors::MAGENTA, Colors::BLACK}},
        {"Honeydew",    {Colors::GREEN, Colors::BLACK}},
        {"Kiwi",        {Colors::LIGHT_GREEN, Colors::BLACK}},
        {"Lemon",       {Colors::YELLOW, Colors::BLACK}},
        {"Mango",       {Colors::LIGHT_RED, Colors::BLACK}},
        {"Nectarine",   {Colors::LIGHT_MAGENTA, Colors::BLACK}},
        {"Orange",      {Colors::BROWN, Colors::BLACK}},
        {"Papaya",      {Colors::YELLOW, Colors::BLACK}},
        {"Quince",      {Colors::LIGHT_CYAN, Colors::BLACK}},
    };
    fruitList.setItems(std::move(fruits));

    // Right list — Korean items
    ListView koreanList;
    koreanList.setNormalStyle({Colors::LIGHT_GRAY, Colors::BLACK});
    koreanList.setSelectedStyle({Colors::WHITE, Colors::CYAN});
    std::vector<ListItem> korean = {
        {"사과",   {Colors::LIGHT_RED, Colors::BLACK}},
        {"바나나", {Colors::YELLOW, Colors::BLACK}},
        {"체리",   {Colors::RED, Colors::BLACK}},
        {"포도",   {Colors::MAGENTA, Colors::BLACK}},
        {"수박",   {Colors::GREEN, Colors::BLACK}},
        {"딸기",   {Colors::LIGHT_RED, Colors::BLACK}},
        {"복숭아", {Colors::LIGHT_MAGENTA, Colors::BLACK}},
        {"참외",   {Colors::YELLOW, Colors::BLACK}},
        {"감",     {Colors::BROWN, Colors::BLACK}},
        {"배",     {Colors::LIGHT_GRAY, Colors::BLACK}},
    };
    koreanList.setItems(std::move(korean));

    // Selection info label
    Label infoLabel("Tab: switch focus | Arrows: navigate | Enter: activate | q: quit");
    infoLabel.setStyle({Colors::WHITE, Colors::DARK_GRAY});
    infoLabel.setAlign(Align::Center);

    Label selLabel("");
    selLabel.setStyle({Colors::YELLOW, Colors::BLUE});
    selLabel.setAlign(Align::Center);

    bool leftFocused = true;

    fruitList.setOnSelectionChanged([&](int idx)
    {
        if (auto const * item = fruitList.selectedItem())
        {
            selLabel.setText("Selected: " + item->text);
        }
    });

    fruitList.setOnActivate([&](int idx)
    {
        if (auto const * item = fruitList.selectedItem())
        {
            selLabel.setText("Activated: " + item->text + "!");
        }
    });

    koreanList.setOnSelectionChanged([&](int idx)
    {
        if (auto const * item = koreanList.selectedItem())
        {
            selLabel.setText("Selected: " + item->text);
        }
    });

    koreanList.setOnActivate([&](int idx)
    {
        if (auto const * item = koreanList.selectedItem())
        {
            selLabel.setText("Activated: " + item->text + "!");
        }
    });

    auto layout = [&]()
    {
        int const W = term.cols();
        int const H = term.rows();
        int const halfW = W / 2 - 2;
        int const listH = H - 7;

        title.setRect(0, 0, W, 1);
        fruitList.setRect(1, 2, halfW, listH);
        koreanList.setRect(W / 2 + 1, 2, halfW, listH);
        selLabel.setRect(0, H - 3, W, 1);
        infoLabel.setRect(0, H - 1, W, 1);
    };
    layout();

    bool running = true;

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
        {
            running = false;
            return;
        }

        if (ev.key == KeyEvent::Key::Tab)
        {
            leftFocused = !leftFocused;
            fruitList.setFocused(leftFocused);
            koreanList.setFocused(!leftFocused);
            return;
        }

        if (leftFocused)
        {
            fruitList.handleKey(ev);
        }
        else
        {
            koreanList.handleKey(ev);
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

        // Draw box borders around lists
        int const W = term.cols();
        int const halfW = W / 2 - 2;
        int const listH = term.rows() - 7;
        Style const boxStyle{Colors::CYAN, Colors::BLUE};
        screen.drawBox(0, 1, halfW + 2, listH + 2, boxStyle);
        screen.drawBox(W / 2, 1, halfW + 2, listH + 2, boxStyle);

        title.draw(screen);
        fruitList.draw(screen);
        koreanList.draw(screen);
        selLabel.draw(screen);
        infoLabel.draw(screen);

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
