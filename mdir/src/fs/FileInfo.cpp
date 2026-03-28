#include "FileInfo.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_set>

namespace mdir::fs
{
namespace
{
std::unordered_set<std::string> const ARCHIVE_EXTENSIONS = {
    ".tar", ".gz", ".bz2", ".xz", ".zip", ".rar", ".7z",
    ".tgz", ".tbz2", ".txz", ".lzh", ".arj", ".cab"
};

std::unordered_set<std::string> const IMAGE_EXTENSIONS = {
    ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".ico", ".svg",
    ".webp", ".tiff", ".tif"
};

std::unordered_set<std::string> const DOCUMENT_EXTENSIONS = {
    ".txt", ".md", ".pdf", ".doc", ".docx", ".odt",
    ".rtf", ".tex", ".html", ".htm", ".xml", ".json"
};
} // namespace

std::string FileInfo::extension() const
{
    auto const ext = path.extension().string();
    if (ext.empty())
    {
        return "";
    }
    // Remove leading dot and convert to uppercase
    std::string result = ext.substr(1);
    std::ranges::transform(result, result.begin(), ::toupper);
    return result;
}

std::string FileInfo::sizeString() const
{
    if (type == FileType::Directory)
    {
        return "[ SubDir ]";
    }

    if (name == "..")
    {
        return "[ Up-Dir ]";
    }

    // Format with comma separators
    std::string s = std::to_string(size);
    int insertPosition = static_cast<int>(s.length()) - 3;
    while (insertPosition > 0)
    {
        s.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return s;
}

std::string FileInfo::dateString() const
{
    auto const sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        modTime - std::filesystem::file_time_type::clock::now() +
        std::chrono::system_clock::now()
    );
    std::time_t const tt = std::chrono::system_clock::to_time_t(sctp);
    std::tm const * tm = std::localtime(&tt);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << (tm->tm_year % 100) << "-"
        << std::setw(2) << (tm->tm_mon + 1) << "-"
        << std::setw(2) << tm->tm_mday;
    return oss.str();
}

std::string FileInfo::timeString() const
{
    auto const sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        modTime - std::filesystem::file_time_type::clock::now() +
        std::chrono::system_clock::now()
    );
    std::time_t const tt = std::chrono::system_clock::to_time_t(sctp);
    std::tm const * tm = std::localtime(&tt);

    int hour = tm->tm_hour;
    char const * ampm = (hour >= 12) ? "p" : "a";
    if (hour == 0)
    {
        hour = 12;
    }
    else if (hour > 12)
    {
        hour -= 12;
    }

    std::ostringstream oss;
    oss << std::setfill(' ') << std::setw(2) << hour << ":"
        << std::setfill('0') << std::setw(2) << tm->tm_min
        << ampm;
    return oss.str();
}

FileType FileInfo::detectType(std::filesystem::path const & p,
    std::filesystem::perms perms)
{
    std::string ext = p.extension().string();
    std::ranges::transform(ext, ext.begin(), ::tolower);

    if (ARCHIVE_EXTENSIONS.contains(ext))
    {
        return FileType::Archive;
    }
    if (IMAGE_EXTENSIONS.contains(ext))
    {
        return FileType::Image;
    }
    if (DOCUMENT_EXTENSIONS.contains(ext))
    {
        return FileType::Document;
    }

    // Check for executable
    if ((perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none)
    {
        return FileType::Executable;
    }

    return FileType::Regular;
}
} // namespace mdir::fs
