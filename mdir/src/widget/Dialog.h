#ifndef MDIR_WIDGET_DIALOG_H
#define MDIR_WIDGET_DIALOG_H

#include "Widget.h"

#include <functional>
#include <string>
#include <vector>

namespace mdir::widget
{

enum class DialogResult
{
    None,
    Ok,
    Cancel,
    Yes,
    No,
};

class Dialog : public Widget
{
public:
    Dialog();

    void setTitle(std::string title) { _title = std::move(title); }
    std::string const & title() const { return _title; }

    void setMessage(std::string message) { _message = std::move(message); }
    std::string const & message() const { return _message; }

    void addButton(std::string label, DialogResult result);
    void clearButtons() { _buttons.clear(); }

    DialogResult result() const { return _result; }
    void setResult(DialogResult r) { _result = r; }

    int selectedButton() const { return _selectedButton; }
    void setSelectedButton(int idx);

    void draw(Screen & screen) override;
    bool handleKey(KeyEvent const & event) override;

    // Calculate size based on content
    void autoSize(int screenWidth, int screenHeight);

private:
    std::string _title;
    std::string _message;
    std::vector<std::pair<std::string, DialogResult>> _buttons;
    int _selectedButton = 0;
    DialogResult _result = DialogResult::None;
};

} // namespace mdir::widget

#endif // MDIR_WIDGET_DIALOG_H
