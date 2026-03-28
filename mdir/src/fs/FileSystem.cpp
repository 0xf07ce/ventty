#include "FileSystem.h"

#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

namespace mdir::fs
{
FileSystem::FileSystem()
{
    _currentPath = std::filesystem::current_path();
    readDirectory();
}

FileSystem::~FileSystem()
{}

bool FileSystem::changeDirectory(std::filesystem::path const & path)
{
    std::error_code ec;
    auto const canonical = std::filesystem::canonical(path, ec);
    if (ec)
    {
        return false;
    }

    if (!std::filesystem::is_directory(canonical, ec) || ec)
    {
        return false;
    }

    _currentPath = canonical;
    readDirectory();
    return true;
}

bool FileSystem::goUp()
{
    if (_currentPath.has_parent_path() && _currentPath != _currentPath.root_path())
    {
        return changeDirectory(_currentPath.parent_path());
    }
    return false;
}

void FileSystem::refresh()
{
    readDirectory();
}

void FileSystem::setShowHidden(bool show)
{
    if (_showHidden != show)
    {
        _showHidden = show;
        readDirectory();
    }
}

void FileSystem::setSortOptions(SortOptions const & opts)
{
    _sortOptions = opts;
    sortEntries();
}

std::uintmax_t FileSystem::freeSpace() const
{
    std::error_code ec;
    auto const info = std::filesystem::space(_currentPath, ec);
    if (ec)
    {
        return 0;
    }
    return info.available;
}

std::uintmax_t FileSystem::totalSpace() const
{
    std::error_code ec;
    auto const info = std::filesystem::space(_currentPath, ec);
    if (ec)
    {
        return 0;
    }
    return info.capacity;
}

void FileSystem::readDirectory()
{
    _entries.clear();
    _fileCount = 0;
    _dirCount = 0;
    _totalSize = 0;

    // Add parent directory entry if not at root
    if (_currentPath != _currentPath.root_path())
    {
        FileInfo parent;
        parent.name = "..";
        parent.path = _currentPath.parent_path();
        parent.type = FileType::Directory;

        std::error_code ec;
        auto const status = std::filesystem::status(parent.path, ec);
        if (!ec)
        {
            parent.modTime = std::filesystem::last_write_time(parent.path, ec);
        }

        _entries.push_back(std::move(parent));
    }

    std::error_code ec;
    for (auto const & entry : std::filesystem::directory_iterator(_currentPath, ec))
    {
        if (ec)
        {
            continue;
        }

        FileInfo info;
        info.name = entry.path().filename().string();
        info.path = entry.path();
        info.isHidden = !info.name.empty() && info.name[0] == '.';

        if (!_showHidden && info.isHidden)
        {
            continue;
        }

        std::error_code ec2;
        auto const status = entry.status(ec2);
        if (ec2)
        {
            continue;
        }

        info.permissions = status.permissions();
        info.isSymlink = entry.is_symlink();

        if (entry.is_directory())
        {
            info.type = FileType::Directory;
            info.size = 0;
            ++_dirCount;
        }
        else if (entry.is_regular_file())
        {
            info.size = entry.file_size(ec2);
            if (ec2)
            {
                info.size = 0;
            }
            info.type = FileInfo::detectType(info.path, info.permissions);
            info.isExecutable = (info.type == FileType::Executable);
            _totalSize += info.size;
            ++_fileCount;
        }
        else if (entry.is_symlink())
        {
            info.type = FileType::Symlink;
            info.isSymlink = true;
        }
        else
        {
            info.type = FileType::Unknown;
        }

        info.modTime = std::filesystem::last_write_time(entry.path(), ec2);

        _entries.push_back(std::move(info));
    }

    sortEntries();
}

void FileSystem::sortEntries()
{
    // Parent directory always first
    auto const parentIt = std::ranges::find_if(_entries, [](FileInfo const & f) { return f.name == ".."; });

    auto const sortBegin = (parentIt != _entries.end()) ? parentIt + 1 : _entries.begin();

    auto comparator = [this](FileInfo const & a, FileInfo const & b) -> bool
    {
        // Directories first (if enabled)
        if (_sortOptions.dirsFirst)
        {
            if (a.isDirectory() != b.isDirectory())
            {
                return a.isDirectory();
            }
        }

        int cmp = 0;
        switch (_sortOptions.sortBy)
        {
        case SortBy::Name:
            {
                std::string nameA = a.name;
                std::string nameB = b.name;
                if (!_sortOptions.caseSensitive)
                {
                    std::ranges::transform(nameA, nameA.begin(), ::tolower);
                    std::ranges::transform(nameB, nameB.begin(), ::tolower);
                }
                cmp = nameA.compare(nameB);
                break;
            }
        case SortBy::Extension:
            {
                std::string extA = a.extension();
                std::string extB = b.extension();
                if (!_sortOptions.caseSensitive)
                {
                    std::ranges::transform(extA, extA.begin(), ::tolower);
                    std::ranges::transform(extB, extB.begin(), ::tolower);
                }
                cmp = extA.compare(extB);
                if (cmp == 0)
                {
                    cmp = a.name.compare(b.name);
                }
                break;
            }
        case SortBy::Size:
            if (a.size < b.size) cmp = -1;
            else if (a.size > b.size) cmp = 1;
            else cmp = 0;
            break;
        case SortBy::Date:
            if (a.modTime < b.modTime) cmp = -1;
            else if (a.modTime > b.modTime) cmp = 1;
            else cmp = 0;
            break;
        }

        if (_sortOptions.reverse)
        {
            cmp = -cmp;
        }

        return cmp < 0;
    };

    std::sort(sortBegin, _entries.end(), comparator);
}

bool FileSystem::execute(std::filesystem::path const & file)
{
    if (!std::filesystem::exists(file))
    {
        return false;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        return false;
    }

    if (pid == 0)
    {
        // Child process
        std::string const pathStr = file.string();

        // Check if it's a script or binary
        execl(pathStr.c_str(), pathStr.c_str(), nullptr);

        // If execl fails, try with shell
        execl("/bin/sh", "sh", "-c", pathStr.c_str(), nullptr);

        std::exit(127);
    }

    // Parent process - wait for child
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
} // namespace mdir::fs
