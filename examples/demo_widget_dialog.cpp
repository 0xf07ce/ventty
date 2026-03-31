//
// ventty Widget — Dialog & InputDialog demo
//

#include <ventty/ventty.h>
#include <ventty/widget/Label.h>
#include <ventty/widget/Dialog.h>
#include <ventty/widget/InputDialog.h>

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

    Window window(0, 0, term.cols(), term.rows());

    auto rebuild = [&]()
    {
        window.resize(term.cols(), term.rows());
    };
    rebuild();

    Label title("ventty::Dialog Demo");
    title.setStyle({ Colors::WHITE, Colors::BLUE, Attr::Bold });
    title.setAlign(Align::Center);

    Label statusLabel("1: Yes/No dialog  2: OK dialog  3: Input dialog  q: Quit");
    statusLabel.setStyle({ Colors::WHITE, Colors::DARK_GRAY });
    statusLabel.setAlign(Align::Center);

    Label resultLabel("");
    resultLabel.setStyle({ Colors::YELLOW, Colors::BLUE });
    resultLabel.setAlign(Align::Center);

    // Dialogs
    Dialog yesNoDialog;
    yesNoDialog.setTitle("Confirm");
    yesNoDialog.setMessage("Do you want to proceed?");
    yesNoDialog.addButton("Yes", DialogResult::Yes);
    yesNoDialog.addButton("No", DialogResult::No);

    Dialog okDialog;
    okDialog.setTitle("Information");
    okDialog.setMessage("This is an informational dialog.");
    okDialog.addButton("OK", DialogResult::Ok);

    InputDialog inputDialog;
    inputDialog.setTitle("Input");
    inputDialog.setPrompt("Enter your name:");
    inputDialog.setValue("");

    enum class Mode
    {
        Normal,
        YesNo,
        Ok,
        Input,
    };

    Mode mode = Mode::Normal;

    auto layout = [&]()
    {
        int const W = term.cols();
        int const H = term.rows();
        title.setRect(0, 0, W, 1);
        resultLabel.setRect(0, H / 2, W, 1);
        statusLabel.setRect(0, H - 1, W, 1);
        yesNoDialog.autoSize(W, H);
        okDialog.autoSize(W, H);
        inputDialog.autoSize(W, H);
    };
    layout();

    bool running = true;

    term.onKey([&](KeyEvent const & ev)
    {
        if (mode == Mode::YesNo)
        {
            yesNoDialog.handleKey(ev);
            if (yesNoDialog.result() != DialogResult::None)
            {
                std::string r = (yesNoDialog.result() == DialogResult::Yes) ? "Yes" : "No";
                resultLabel.setText("You chose: " + r);
                yesNoDialog.setResult(DialogResult::None);
                mode = Mode::Normal;
            }
            return;
        }

        if (mode == Mode::Ok)
        {
            okDialog.handleKey(ev);
            if (okDialog.result() != DialogResult::None)
            {
                resultLabel.setText("OK dialog closed.");
                okDialog.setResult(DialogResult::None);
                mode = Mode::Normal;
            }
            return;
        }

        if (mode == Mode::Input)
        {
            inputDialog.handleKey(ev);
            if (inputDialog.result() != DialogResult::None)
            {
                if (inputDialog.result() == DialogResult::Ok)
                {
                    resultLabel.setText("Hello, " + inputDialog.value() + "!");
                }
                else
                {
                    resultLabel.setText("Input cancelled.");
                }
                inputDialog.setResult(DialogResult::None);
                mode = Mode::Normal;
            }
            return;
        }

        // Normal mode
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
        {
            running = false;
            return;
        }

        if (ev.key == KeyEvent::Key::Char)
        {
            if (ev.ch == U'1')
            {
                mode = Mode::YesNo;
                yesNoDialog.setSelectedButton(0);
            }
            else if (ev.ch == U'2')
            {
                mode = Mode::Ok;
                okDialog.setSelectedButton(0);
            }
            else if (ev.ch == U'3')
            {
                mode = Mode::Input;
                inputDialog.setValue("");
            }
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
        while (term.pollEvent());

        window.clear(Style { Colors::LIGHT_GRAY, Colors::BLUE });

        title.draw(window);
        resultLabel.draw(window);
        statusLabel.draw(window);

        if (mode == Mode::YesNo)
        {
            yesNoDialog.draw(window);
        }
        else if (mode == Mode::Ok)
        {
            okDialog.draw(window);
        }
        else if (mode == Mode::Input)
        {
            inputDialog.draw(window);
        }

        for (int y = 0; y < window.height() && y < term.rows(); ++y)
        {
            for (int x = 0; x < window.width() && x < term.cols(); ++x)
            {
                auto const & cell = window.cellAt(x, y);
                term.putChar(x, y, cell.ch, cell.style);
            }
        }

        term.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    term.shutdown();
    return 0;
}
