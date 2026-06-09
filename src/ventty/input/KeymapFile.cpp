// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include <ventty/input/KeymapFile.h>

#include <cctype>

namespace ventty
{
namespace
{
std::string_view trim(std::string_view s)
{
    std::size_t b = 0;
    std::size_t e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b])))
        ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1])))
        --e;
    return s.substr(b, e - b);
}

/// Drop a trailing comment: a '#' at the start of the line or following
/// whitespace begins a comment.
std::string_view stripComment(std::string_view line)
{
    for (std::size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == '#' && (i == 0 || std::isspace(static_cast<unsigned char>(line[i - 1]))))
            return line.substr(0, i);
    }
    return line;
}

std::vector<std::string> splitWhitespace(std::string_view s)
{
    std::vector<std::string> out;
    std::size_t i = 0;
    while (i < s.size())
    {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
            ++i;
        std::size_t const start = i;
        while (i < s.size() && !std::isspace(static_cast<unsigned char>(s[i])))
            ++i;
        if (i > start)
            out.emplace_back(s.substr(start, i - start));
    }
    return out;
}

std::vector<std::string> splitComma(std::string_view s)
{
    std::vector<std::string> out;
    std::size_t start = 0;
    for (std::size_t i = 0; i <= s.size(); ++i)
    {
        if (i == s.size() || s[i] == ',')
        {
            std::string_view const t = trim(s.substr(start, i - start));
            if (!t.empty())
                out.emplace_back(t);
            start = i + 1;
        }
    }
    return out;
}
} // namespace

KeymapConfig parseKeymap(std::string_view text, TokenValidator validator)
{
    KeymapConfig cfg;

    std::size_t pos = 0;
    int lineNo = 0;
    while (pos <= text.size())
    {
        std::size_t const nl = text.find('\n', pos);
        std::string_view const raw =
            (nl == std::string_view::npos) ? text.substr(pos) : text.substr(pos, nl - pos);
        pos = (nl == std::string_view::npos) ? text.size() + 1 : nl + 1;
        ++lineNo;

        std::string_view const line = trim(stripComment(raw));
        if (line.empty())
            continue;

        std::vector<std::string> const fields = splitWhitespace(line);
        if (fields.empty())
            continue;
        std::string const & verb = fields[0];
        std::size_t const eq = line.find('=');

        auto warn = [&](std::string const & msg) {
            cfg.warnings.push_back("line " + std::to_string(lineNo) + ": " + msg);
        };

        if (verb == "modes" || verb == "mode")
        {
            if (eq == std::string_view::npos)
            {
                warn("expected 'modes = <name>, ...'");
                continue;
            }
            cfg.modes = splitComma(trim(line.substr(eq + 1)));
            for (auto const & m : cfg.modes)
                cfg.keymaps[m]; // ensure an entry exists even if it gets no binds
            continue;
        }

        if (verb == "counts" || verb == "count")
        {
            if (eq == std::string_view::npos)
            {
                warn("expected 'counts = on|off'");
                continue;
            }
            std::string_view const v = trim(line.substr(eq + 1));
            cfg.counts = (v == "on" || v == "true" || v == "1" || v == "yes");
            continue;
        }

        if (verb == "map" || verb == "motion" || verb == "op")
        {
            if (fields.size() < 4)
            {
                warn(verb + ": expected <mode> <lhs> <token>");
                continue;
            }
            std::string const & mode = fields[1];
            std::string const & lhs = fields[2];
            std::string const & token = fields[3];

            if (cfg.keymaps.find(mode) == cfg.keymaps.end())
            {
                warn("unknown mode '" + mode + "' (declare it in 'modes =' first)");
                continue;
            }
            if (verb != "map")
            {
                warn(verb + " is reserved for a future operator+motion tier; ignored");
                continue;
            }

            std::vector<KeyChord> const seq = KeyChord::parseSequence(lhs);
            if (seq.empty())
            {
                warn("could not parse key '" + lhs + "'");
                continue;
            }
            if (validator && !validator(token))
                warn("unknown action '" + token + "'");

            cfg.keymaps[mode].bind(seq, token);
            continue;
        }

        warn("unknown directive '" + verb + "'");
    }

    return cfg;
}
} // namespace ventty
