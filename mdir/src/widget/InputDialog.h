#ifndef MDIR_WIDGET_INPUTDIALOG_H
#define MDIR_WIDGET_INPUTDIALOG_H

#include "Dialog.h"

#include <string>

namespace mdir::widget
{

class InputDialog : public Widget
{
public:
    InputDialog();

    void setTitle(std::string title) { _title = std::move(title); }
    std::string const & title() const { return _title; }

    void setPrompt(std::string prompt) { _prompt = std::move(prompt); }
    std::string const & prompt() const { return _prompt; }

    void setValue(std::string value);
    std::string const & value() const { return _value; }

    DialogResult result() const { return _result; }
    void setResult(DialogResult r) { _result = r; }

    void draw(Screen & screen) override;
    bool handleKey(KeyEvent const & event) override;

    void autoSize(int screenWidth, int screenHeight);

private:
    void insertChar(char32_t ch);
    void deleteChar();
    void backspace();
    void moveCursorLeft();
    void moveCursorRight();

    std::string _title;
    std::string _prompt;
    std::string _value;
    int _cursorPos = 0;
    int _scrollOffset = 0;
    DialogResult _result = DialogResult::None;
};

} // namespace mdir::widget

#endif // MDIR_WIDGET_INPUTDIALOG_H
