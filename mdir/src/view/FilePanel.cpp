#include "FilePanel.h"

#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <sstream>

namespace mdir::view
{

FilePanel::FilePanel()
{
}

void FilePanel::setSelectedIndex(int index)
{
    int const count = static_cast<int>(_fs.entries().size());
    int const newIndex = std::clamp(index, 0, std::max(0, count - 1));

    if (newIndex != _selectedIndex)
    {
        _selectedIndex = newIndex;
        ensureVisible(_selectedIndex);
    }
}

mdir::fs::FileInfo const * FilePanel::selectedFile() const
{
    auto const & entries = _fs.entries();
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(entries.size()))
    {
        return &entries[_selectedIndex];
    }
    return nullptr;
}

void FilePanel::toggleTag(int index)
{
    auto const & entries = _fs.entries();
    if (index < 0 || index >= static_cast<int>(entries.size()))
    {
        return;
    }

    // Don't allow tagging ".."
    if (entries[index].isParentDir())
    {
        return;
    }

    if (_taggedIndices.count(index))
    {
        _taggedIndices.erase(index);
    }
    else
    {
        _taggedIndices.insert(index);
    }
}

void FilePanel::tagCurrent()
{
    auto const & entries = _fs.entries();
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(entries.size()))
    {
        if (!entries[_selectedIndex].isParentDir())
        {
            _taggedIndices.insert(_selectedIndex);
        }
    }
}

void FilePanel::untagCurrent()
{
    _taggedIndices.erase(_selectedIndex);
}

void FilePanel::tagAll()
{
    auto const & entries = _fs.entries();
    _taggedIndices.clear();
    for (size_t i = 0; i < entries.size(); ++i)
    {
        if (!entries[i].isParentDir())
        {
            _taggedIndices.insert(static_cast<int>(i));
        }
    }
}

void FilePanel::untagAll()
{
    _taggedIndices.clear();
}

void FilePanel::invertTags()
{
    auto const & entries = _fs.entries();
    std::set<int> newTags;
    for (size_t i = 0; i < entries.size(); ++i)
    {
        if (!entries[i].isParentDir() && !_taggedIndices.count(static_cast<int>(i)))
        {
            newTags.insert(static_cast<int>(i));
        }
    }
    _taggedIndices = std::move(newTags);
}

bool FilePanel::isTagged(int index) const
{
    return _taggedIndices.count(index) > 0;
}

std::vector<mdir::fs::FileInfo const *> FilePanel::getTargetFiles() const
{
    std::vector<mdir::fs::FileInfo const *> result;
    auto const & entries = _fs.entries();

    if (_taggedIndices.empty())
    {
        // No tags, use current file
        if (auto const * file = selectedFile())
        {
            if (!file->isParentDir())
            {
                result.push_back(file);
            }
        }
    }
    else
    {
        // Use tagged files
        for (int idx : _taggedIndices)
        {
            if (idx >= 0 && idx < static_cast<int>(entries.size()))
            {
                result.push_back(&entries[idx]);
            }
        }
    }

    return result;
}

bool FilePanel::enter()
{
    auto const * file = selectedFile();
    if (!file)
    {
        return false;
    }

    if (file->isDirectory() || file->isParentDir())
    {
        if (_fs.changeDirectory(file->path))
        {
            _selectedIndex = 0;
            _scrollOffset = 0;
            _taggedIndices.clear();
            return true;
        }
        return false;
    }

    // Execute file
    if (_onExecute)
    {
        _onExecute(*file);
        return true;
    }

    return false;
}

bool FilePanel::goUp()
{
    if (_fs.goUp())
    {
        _selectedIndex = 0;
        _scrollOffset = 0;
        _taggedIndices.clear();
        return true;
    }
    return false;
}

void FilePanel::refresh()
{
    _fs.refresh();
    _selectedIndex = std::clamp(_selectedIndex, 0,
        static_cast<int>(_fs.entries().size()) - 1);
    _taggedIndices.clear();
    ensureVisible(_selectedIndex);
}

int FilePanel::visibleItemCount() const
{
    return contentHeight();
}

int FilePanel::contentHeight() const
{
    // Height minus borders (top and bottom)
    return std::max(0, _rect.height - 2);
}

void FilePanel::ensureVisible(int index)
{
    if (index < _scrollOffset)
    {
        _scrollOffset = index;
    }
    else if (index >= _scrollOffset + visibleItemCount())
    {
        _scrollOffset = index - visibleItemCount() + 1;
    }

    int const maxOffset = std::max(0,
        static_cast<int>(_fs.entries().size()) - visibleItemCount());
    _scrollOffset = std::clamp(_scrollOffset, 0, maxOffset);
}

void FilePanel::scrollToSelected()
{
    ensureVisible(_selectedIndex);
}

void FilePanel::onResize()
{
    ensureVisible(_selectedIndex);
}

Style FilePanel::styleForFile(mdir::fs::FileInfo const & file, bool isTagged) const
{
    Style style{_theme.fileListFg, _theme.fileListBg};

    if (file.isParentDir())
    {
        style.fg = _theme.parentDirFg;
    }
    else if (file.isDirectory())
    {
        style.fg = _theme.directoryFg;
    }
    else if (file.isHidden)
    {
        style.fg = _theme.hiddenFg;
    }
    else if (file.isSymlink)
    {
        style.fg = _theme.symlinkFg;
    }
    else
    {
        switch (file.type)
        {
            case mdir::fs::FileType::Executable:
                style.fg = _theme.executableFg;
                break;
            case mdir::fs::FileType::Archive:
                style.fg = _theme.archiveFg;
                break;
            default:
                break;
        }
    }

    // Tagged files get yellow background
    if (isTagged)
    {
        style.bg = Colors::BROWN;
    }

    return style;
}

std::string FilePanel::formatFileEntry(mdir::fs::FileInfo const & file, int width, bool isTagged) const
{
    // MDIR format:
    // [Tag][Name 12 chars][Ext 3 chars] [Size/Type 10 chars] [Date 8 chars] [Time 6 chars]

    std::ostringstream oss;

    // Tag marker
    oss << (isTagged ? "*" : " ");

    // Name
    std::string name = file.name;
    std::string ext = file.extension();

    if (file.isDirectory() || file.isParentDir())
    {
        int const maxNameLen = 12;
        if (name.size() > static_cast<size_t>(maxNameLen))
        {
            name = name.substr(0, maxNameLen);
        }
        oss << std::left << std::setw(13) << name;
    }
    else
    {
        std::string baseName = file.name;
        size_t const dotPos = baseName.rfind('.');
        if (dotPos != std::string::npos && dotPos > 0)
        {
            baseName = baseName.substr(0, dotPos);
        }

        int const maxNameLen = 9;
        if (baseName.size() > static_cast<size_t>(maxNameLen))
        {
            baseName = baseName.substr(0, maxNameLen);
        }

        oss << std::left << std::setw(9) << baseName;

        if (ext.size() > 3)
        {
            ext = ext.substr(0, 3);
        }
        oss << " " << std::left << std::setw(3) << ext;
    }

    // Size or type
    std::string sizeStr = file.sizeString();
    oss << " " << std::right << std::setw(10) << sizeStr;

    // Date
    oss << " " << file.dateString();

    // Time
    oss << " " << file.timeString();

    std::string result = oss.str();

    // Pad or truncate to fit width
    if (static_cast<int>(result.size()) < width)
    {
        result.append(width - result.size(), ' ');
    }
    else if (static_cast<int>(result.size()) > width)
    {
        result = result.substr(0, width);
    }

    return result;
}

void FilePanel::draw(Screen & screen)
{
    if (!_visible || _rect.width <= 0 || _rect.height <= 0)
    {
        return;
    }

    // Draw top separator line (═══)
    screen.setStyle({_theme.panelSeparatorFg, _theme.panelSeparatorBg});
    screen.setCursor(_rect.x, _rect.y);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U'═');
    }

    // Draw bottom separator line (═══)
    screen.setCursor(_rect.x, _rect.y + _rect.height - 1);
    for (int i = 0; i < _rect.width; ++i)
    {
        screen.putChar(U'═');
    }

    // Content area (between separator lines, minus 1 col for scrollbar)
    int const contentX = _rect.x;
    int const contentY = _rect.y + 1;
    int const contentW = _rect.width - 1;  // 1 column for scrollbar
    int const contentH = contentHeight();

    auto const & entries = _fs.entries();
    int const visible = visibleItemCount();
    int const totalItems = static_cast<int>(entries.size());

    for (int i = 0; i < visible; ++i)
    {
        int const itemIndex = _scrollOffset + i;
        int const y = contentY + i;

        if (itemIndex < totalItems)
        {
            mdir::fs::FileInfo const & file = entries[itemIndex];
            bool const tagged = isTagged(itemIndex);
            Style style = styleForFile(file, tagged);

            // Selection highlight
            if (itemIndex == _selectedIndex)
            {
                if (_focused)
                {
                    style.bg = _theme.selectionBg;
                    style.fg = _theme.selectionFg;
                }
                else
                {
                    style.bg = Colors::DARK_GRAY;
                }
            }

            screen.setStyle(style);
            screen.setCursor(contentX, y);

            std::string line = formatFileEntry(file, contentW, tagged);
            screen.putString(line);
        }
        else
        {
            // Empty row
            screen.setStyle({_theme.fileListFg, _theme.fileListBg});
            screen.setCursor(contentX, y);
            for (int j = 0; j < contentW; ++j)
            {
                screen.putChar(U' ');
            }
        }
    }

    // Draw scrollbar on right edge
    drawScrollbar(screen, totalItems, visible);
}

void FilePanel::drawScrollbar(Screen & screen, int totalItems, int visibleItems)
{
    int const sbX = _rect.x + _rect.width - 1;
    int const sbY = _rect.y + 1;
    int const sbH = contentHeight();

    if (sbH <= 0)
    {
        return;
    }

    if (totalItems <= visibleItems)
    {
        // No scrolling needed — fill with empty scrollbar track
        screen.setStyle({_theme.scrollBarFg, _theme.scrollBarBg});
        for (int i = 0; i < sbH; ++i)
        {
            screen.setCursor(sbX, sbY + i);
            screen.putChar(U'░');
        }
        return;
    }

    // Calculate thumb position and size
    int thumbH = std::max(1, sbH * visibleItems / totalItems);
    int thumbY = sbH * _scrollOffset / totalItems;
    thumbY = std::min(thumbY, sbH - thumbH);

    for (int i = 0; i < sbH; ++i)
    {
        screen.setCursor(sbX, sbY + i);
        if (i >= thumbY && i < thumbY + thumbH)
        {
            screen.setStyle({_theme.scrollBarFg, _theme.scrollBarBg});
            screen.putChar(U'█');
        }
        else
        {
            screen.setStyle({_theme.scrollBarFg, _theme.scrollBarBg});
            screen.putChar(U'░');
        }
    }
}

bool FilePanel::handleKey(KeyEvent const & event)
{
    auto const & entries = _fs.entries();
    if (entries.empty())
    {
        return false;
    }

    switch (event.key)
    {
        case Key::Up:
            setSelectedIndex(_selectedIndex - 1);
            return true;

        case Key::Down:
            setSelectedIndex(_selectedIndex + 1);
            return true;

        case Key::PageUp:
            setSelectedIndex(_selectedIndex - visibleItemCount());
            return true;

        case Key::PageDown:
            setSelectedIndex(_selectedIndex + visibleItemCount());
            return true;

        case Key::Home:
            setSelectedIndex(0);
            return true;

        case Key::End:
            setSelectedIndex(static_cast<int>(entries.size()) - 1);
            return true;

        case Key::Enter:
            return enter();

        case Key::Backspace:
        case Key::Left:
            return goUp();

        case Key::Right:
        {
            auto const * file = selectedFile();
            if (file && (file->isDirectory() || file->isParentDir()))
            {
                return enter();
            }
            return false;
        }

        case Key::Insert:
            toggleTag(_selectedIndex);
            setSelectedIndex(_selectedIndex + 1);
            return true;

        case Key::Char:
            if (event.ch == U' ')
            {
                toggleTag(_selectedIndex);
                setSelectedIndex(_selectedIndex + 1);
                return true;
            }
            // Ctrl+H - toggle hidden files
            if (event.ctrl && (event.ch == 'h' || event.ch == 'H'))
            {
                _fs.setShowHidden(!_fs.showHidden());
                return true;
            }
            // Ctrl+A - select all
            if (event.ctrl && (event.ch == 'a' || event.ch == 'A'))
            {
                tagAll();
                return true;
            }
            // Ctrl+D - deselect all
            if (event.ctrl && (event.ch == 'd' || event.ch == 'D'))
            {
                untagAll();
                return true;
            }
            // * - invert selection
            if (event.ch == '*')
            {
                invertTags();
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

} // namespace mdir::view
