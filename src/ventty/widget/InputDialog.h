// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#ifndef VENTTY_WIDGET_INPUTDIALOG_H
#define VENTTY_WIDGET_INPUTDIALOG_H

#include "Dialog.h"

#include <string>

namespace ventty
{
/// Text input dialog widget
class InputDialog : public Widget
{
public:
    /// Constructor
    InputDialog();

    /// Set the title
    void setTitle(std::string title);

    /// Return the title
    std::string const & title() const;

    /// Set the prompt text
    void setPrompt(std::string prompt);

    /// Return the prompt text
    std::string const & prompt() const;

    /// Set the input value
    void setValue(std::string value);

    /// Return the input value
    std::string const & value() const;

    /// Return the dialog result
    DialogResult result() const;

    /// Set the result
    void setResult(DialogResult r);

    /// Draw the input dialog to the window
    void draw(Window & window) override;

    /// Handle key event
    bool handleKey(KeyEvent const & event) override;

    /// Set the IME composing text
    void setComposingText(std::string text);

    /// Auto-resize to fit content
    void autoSize(int screenWidth, int screenHeight);

private:
    /// Insert a character at the cursor position
    void insertChar(char32_t ch);

    /// Delete the character at the cursor position
    void deleteChar();

    /// Delete the character before the cursor
    void backspace();

    /// Move the cursor left
    void moveCursorLeft();

    /// Move the cursor right
    void moveCursorRight();

    std::string _title;                        ///< Dialog title
    std::string _prompt;                       ///< Prompt text
    std::string _value;                        ///< Current input value
    std::string _composingText;                ///< IME composing text
    int _cursorPos = 0;                        ///< Cursor position
    int _scrollOffset = 0;                     ///< Input field scroll offset
    DialogResult _result = DialogResult::None; ///< Dialog result
};
} // namespace ventty

#endif // VENTTY_WIDGET_INPUTDIALOG_H
