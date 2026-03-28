#ifndef MDIR_FS_FILEINFO_H
#define MDIR_FS_FILEINFO_H

#include <chrono>
#include <filesystem>
#include <string>

namespace mdir::fs
{
enum class FileType
{
    Unknown,
    Regular,
    Directory,
    Symlink,
    Executable,
    Archive,
    Image,
    Document,
};

struct FileInfo
{
    std::string name;
    std::filesystem::path path;
    FileType type = FileType::Unknown;
    std::uintmax_t size = 0;
    std::filesystem::file_time_type modTime;
    std::filesystem::perms permissions;
    bool isHidden = false;
    bool isExecutable = false;
    bool isSymlink = false;

    // For display
    std::string extension() const;
    std::string sizeString() const;
    std::string dateString() const;
    std::string timeString() const;

    bool isDirectory() const { return type == FileType::Directory; }
    bool isParentDir() const { return name == ".."; }

    static FileType detectType(std::filesystem::path const & path,
        std::filesystem::perms perms);
};
} // namespace mdir::fs

#endif // MDIR_FS_FILEINFO_H
