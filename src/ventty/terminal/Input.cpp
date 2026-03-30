#include "Input.h"

#include <poll.h>
#include <unistd.h>
#include <cstring>

namespace ventty
{
Input::Input()
{}

Input::~Input()
{}

int Input::readByte(int timeoutMs)
{
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;

    int ret = ::poll(&pfd, 1, timeoutMs);
    if (ret <= 0)
    {
        return -1;
    }

    unsigned char c;
    if (::read(STDIN_FILENO, &c, 1) != 1)
    {
        return -1;
    }
    return c;
}

bool Input::poll(int timeoutMs)
{
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    return (::poll(&pfd, 1, timeoutMs) > 0);
}

KeyEvent Input::read(int timeoutMs)
{
    int c = readByte(timeoutMs);
    if (c < 0)
    {
        return {};
    }

    KeyEvent event;

    // ESC sequence
    if (c == 27)
    {
        return parseEscapeSequence();
    }

    // Control characters
    if (c == 13 || c == 10)
    {
        event.key = KeyEvent::Key::Enter;
        return event;
    }
    if (c == 9)
    {
        event.key = KeyEvent::Key::Tab;
        return event;
    }
    if (c == 127 || c == 8)
    {
        event.key = KeyEvent::Key::Backspace;
        return event;
    }

    // Ctrl+letter (1-26 maps to Ctrl+A to Ctrl+Z)
    if (c >= 1 && c <= 26)
    {
        event.key = KeyEvent::Key::Char;
        event.ch = 'a' + (c - 1);
        event.ctrl = true;
        return event;
    }

    // UTF-8 decoding
    if ((c & 0x80) == 0)
    {
        // ASCII
        event.key = KeyEvent::Key::Char;
        event.ch = c;
        return event;
    }

    // Multi-byte UTF-8
    char32_t codepoint = 0;
    int remaining = 0;

    if ((c & 0xE0) == 0xC0)
    {
        codepoint = c & 0x1F;
        remaining = 1;
    }
    else if ((c & 0xF0) == 0xE0)
    {
        codepoint = c & 0x0F;
        remaining = 2;
    }
    else if ((c & 0xF8) == 0xF0)
    {
        codepoint = c & 0x07;
        remaining = 3;
    }
    else
    {
        event.key = KeyEvent::Key::Char;
        event.ch = c;
        return event;
    }

    for (int i = 0; i < remaining; ++i)
    {
        int next = readByte(ESC_TIMEOUT_MS);
        if (next < 0 || (next & 0xC0) != 0x80)
        {
            event.key = KeyEvent::Key::Char;
            event.ch = c;
            return event;
        }
        codepoint = (codepoint << 6) | (next & 0x3F);
    }

    event.key = KeyEvent::Key::Char;
    event.ch = codepoint;
    return event;
}

KeyEvent Input::parseEscapeSequence()
{
    KeyEvent event;

    int c = readByte(ESC_TIMEOUT_MS);
    if (c < 0)
    {
        event.key = KeyEvent::Key::Escape;
        return event;
    }

    if (c == '[')
    {
        return parseCsiSequence();
    }

    if (c == 'O')
    {
        return parseSs3Sequence();
    }

    // Alt+key
    event.key = KeyEvent::Key::Char;
    event.ch = c;
    event.alt = true;
    return event;
}

KeyEvent Input::parseCsiSequence()
{
    KeyEvent event;

    int params[8] = { 0 };
    int paramCount = 0;
    int c;

    while (true)
    {
        c = readByte(ESC_TIMEOUT_MS);
        if (c < 0)
        {
            event.key = KeyEvent::Key::Escape;
            return event;
        }

        if (c >= '0' && c <= '9')
        {
            if (paramCount < 8)
            {
                params[paramCount] = params[paramCount] * 10 + (c - '0');
            }
        }
        else if (c == ';')
        {
            paramCount++;
        }
        else
        {
            if (params[paramCount] != 0 || paramCount > 0)
            {
                paramCount++;
            }
            break;
        }
    }

    // Parse modifiers (if present in param 2)
    if (paramCount >= 2)
    {
        int mod = params[1] - 1;
        event.shift = (mod & 1) != 0;
        event.alt = (mod & 2) != 0;
        event.ctrl = (mod & 4) != 0;
    }

    switch (c)
    {
    case 'A': event.key = KeyEvent::Key::Up;
        break;
    case 'B': event.key = KeyEvent::Key::Down;
        break;
    case 'C': event.key = KeyEvent::Key::Right;
        break;
    case 'D': event.key = KeyEvent::Key::Left;
        break;
    case 'H': event.key = KeyEvent::Key::Home;
        break;
    case 'F': event.key = KeyEvent::Key::End;
        break;
    case 'Z': event.key = KeyEvent::Key::Tab;
        event.shift = true;
        break;
    case '~':
        switch (params[0])
        {
        case 1: event.key = KeyEvent::Key::Home;
            break;
        case 2: event.key = KeyEvent::Key::Insert;
            break;
        case 3: event.key = KeyEvent::Key::Delete;
            break;
        case 4: event.key = KeyEvent::Key::End;
            break;
        case 5: event.key = KeyEvent::Key::PageUp;
            break;
        case 6: event.key = KeyEvent::Key::PageDown;
            break;
        case 7: event.key = KeyEvent::Key::Home;
            break;
        case 8: event.key = KeyEvent::Key::End;
            break;
        case 11: event.key = KeyEvent::Key::F1;
            break;
        case 12: event.key = KeyEvent::Key::F2;
            break;
        case 13: event.key = KeyEvent::Key::F3;
            break;
        case 14: event.key = KeyEvent::Key::F4;
            break;
        case 15: event.key = KeyEvent::Key::F5;
            break;
        case 17: event.key = KeyEvent::Key::F6;
            break;
        case 18: event.key = KeyEvent::Key::F7;
            break;
        case 19: event.key = KeyEvent::Key::F8;
            break;
        case 20: event.key = KeyEvent::Key::F9;
            break;
        case 21: event.key = KeyEvent::Key::F10;
            break;
        case 23: event.key = KeyEvent::Key::F11;
            break;
        case 24: event.key = KeyEvent::Key::F12;
            break;
        default: event.key = KeyEvent::Key::None;
            break;
        }
        break;
    default:
        event.key = KeyEvent::Key::None;
        break;
    }

    return event;
}

KeyEvent Input::parseSs3Sequence()
{
    KeyEvent event;

    int c = readByte(ESC_TIMEOUT_MS);
    if (c < 0)
    {
        event.key = KeyEvent::Key::Escape;
        return event;
    }

    switch (c)
    {
    case 'P': event.key = KeyEvent::Key::F1;
        break;
    case 'Q': event.key = KeyEvent::Key::F2;
        break;
    case 'R': event.key = KeyEvent::Key::F3;
        break;
    case 'S': event.key = KeyEvent::Key::F4;
        break;
    case 'H': event.key = KeyEvent::Key::Home;
        break;
    case 'F': event.key = KeyEvent::Key::End;
        break;
    case 'A': event.key = KeyEvent::Key::Up;
        break;
    case 'B': event.key = KeyEvent::Key::Down;
        break;
    case 'C': event.key = KeyEvent::Key::Right;
        break;
    case 'D': event.key = KeyEvent::Key::Left;
        break;
    default: event.key = KeyEvent::Key::None;
        break;
    }

    return event;
}
} // namespace ventty
