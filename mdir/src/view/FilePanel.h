#ifndef MDIR_VIEW_FILEPANEL_H
#define MDIR_VIEW_FILEPANEL_H

#include "../fs/FileSystem.h"
#include "../widget/Widget.h"
#include "Theme.h"

#include <functional>
#include <set>
#include <vector>

namespace mdir::view
{
using namespace mdir::widget;

class FilePanel : public Widget
{
public:
    FilePanel();

    // File system
    mdir::fs::FileSystem & fileSystem() { return _fs; }
    mdir::fs::FileSystem const & fileSystem() const { return _fs; }

    // Cursor selection (single file under cursor)
    int selectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int index);
    mdir::fs::FileInfo const * selectedFile() const;

    // Multi-selection (tagged files)
    void toggleTag(int index);
    void tagCurrent();
    void untagCurrent();
    void tagAll();
    void untagAll();
    void invertTags();
    bool isTagged(int index) const;
    int taggedCount() const { return static_cast<int>(_taggedIndices.size()); }
    std::set<int> const & taggedIndices() const { return _taggedIndices; }

    // Get files for operations (tagged files or current file)
    std::vector<mdir::fs::FileInfo const *> getTargetFiles() const;

    // Navigation
    bool enter(); // Enter directory or execute file
    bool goUp();  // Go to parent directory
    void refresh();

    // Scrolling
    int scrollOffset() const { return _scrollOffset; }
    void scrollToSelected();

    // Theme
    Theme const & theme() const { return _theme; }
    void setTheme(Theme const & t) { _theme = t; }

    // Callbacks
    using ExecuteCallback = std::function<void(mdir::fs::FileInfo const &)>;
    void setOnExecute(ExecuteCallback cb) { _onExecute = std::move(cb); }

    // Widget interface
    void draw(Screen & screen) override;
    bool handleKey(KeyEvent const & event) override;

protected:
    void onResize() override;

private:
    void updateItems();
    void ensureVisible(int index);
    int visibleItemCount() const;
    int contentHeight() const;
    Style styleForFile(mdir::fs::FileInfo const & file, bool isTagged) const;
    std::string formatFileEntry(mdir::fs::FileInfo const & file, int width, bool isTagged) const;
    void drawScrollbar(Screen & screen, int totalItems, int visibleItems);

    mdir::fs::FileSystem _fs;
    int _selectedIndex = 0;
    int _scrollOffset = 0;
    std::set<int> _taggedIndices;
    Theme _theme;
    ExecuteCallback _onExecute;
};
} // namespace mdir::view

#endif // MDIR_VIEW_FILEPANEL_H
