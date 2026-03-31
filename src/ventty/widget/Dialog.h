// Copyright (c) 2026 ventty-studio
// SPDX-License-Identifier: MIT

#ifndef VENTTY_WIDGET_DIALOG_H
#define VENTTY_WIDGET_DIALOG_H

#include "Widget.h"

#include <functional>
#include <string>
#include <vector>

namespace ventty
{
/// Dialog result
enum class DialogResult
{
    None,   ///< No result
    Ok,     ///< OK
    Cancel, ///< Cancel
    Yes,    ///< Yes
    No,     ///< No
};

/// Message dialog widget
class Dialog : public Widget
{
public:
    /// Constructor
    Dialog();

    /// Set title
    void setTitle(std::string title);

    /// Return title
    std::string const & title() const;

    /// Set message
    void setMessage(std::string message);

    /// Return message
    std::string const & message() const;

    /// Add button (label and result value)
    void addButton(std::string label, DialogResult result);

    /// Remove all buttons
    void clearButtons();

    /// Return dialog result
    DialogResult result() const;

    /// Set result value
    void setResult(DialogResult r);

    /// Return selected button index
    int selectedButton() const;

    /// Set selected button index
    void setSelectedButton(int idx);

    /// Draw dialog to window
    void draw(Window & window) override;

    /// Handle key event
    bool handleKey(KeyEvent const & event) override;

    /// Auto-size to fit content
    void autoSize(int screenWidth, int screenHeight);

private:
    std::string _title;                                         ///< Dialog title
    std::string _message;                                       ///< Dialog message
    std::vector<std::pair<std::string, DialogResult>> _buttons; ///< Button list (label, result pairs)
    int _selectedButton = 0;                                    ///< Selected button index
    DialogResult _result = DialogResult::None;                  ///< Dialog result
};
} // namespace ventty

#endif // VENTTY_WIDGET_DIALOG_H
