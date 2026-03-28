#ifndef MDIR_FS_FILESYSTEM_H
#define MDIR_FS_FILESYSTEM_H

#include "FileInfo.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace mdir::fs
{
enum class SortBy
{
    Name,
    Extension,
    Size,
    Date,
};

struct SortOptions
{
    SortBy sortBy = SortBy::Name;
    bool reverse = false;
    bool dirsFirst = true;
    bool caseSensitive = false;
};

class FileSystem
{
public:
    FileSystem();
    ~FileSystem();

    // Current directory
    std::filesystem::path const & currentPath() const { return _currentPath; }
    bool changeDirectory(std::filesystem::path const & path);
    bool goUp();

    // Read directory
    std::vector<FileInfo> const & entries() const { return _entries; }
    void refresh();

    // Options
    bool showHidden() const { return _showHidden; }
    void setShowHidden(bool show);

    SortOptions const & sortOptions() const { return _sortOptions; }
    void setSortOptions(SortOptions const & opts);

    // Disk space
    std::uintmax_t freeSpace() const;
    std::uintmax_t totalSpace() const;

    // File operations
    bool execute(std::filesystem::path const & file);

    // Statistics
    int fileCount() const { return _fileCount; }
    int dirCount() const { return _dirCount; }
    std::uintmax_t totalSize() const { return _totalSize; }

private:
    void readDirectory();
    void sortEntries();

    std::filesystem::path _currentPath;
    std::vector<FileInfo> _entries;
    bool _showHidden = false;
    SortOptions _sortOptions;

    int _fileCount = 0;
    int _dirCount = 0;
    std::uintmax_t _totalSize = 0;
};
} // namespace mdir::fs

#endif // MDIR_FS_FILESYSTEM_H
