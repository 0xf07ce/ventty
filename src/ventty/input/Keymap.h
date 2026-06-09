// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#pragma once

#include <ventty/input/KeyChord.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace ventty
{
/// A trie of KeyChord sequences mapped to opaque string tokens.
///
/// The token's meaning is the application's concern — the keymap only resolves
/// "which sequence was typed". `match()` distinguishes a completed binding from
/// a still-growing prefix so the input engine can wait for more keys without a
/// timer.
class Keymap
{
public:
    enum class Status
    {
        NoMatch, ///< The sequence matches no binding and is not a live prefix.
        Pending, ///< A valid prefix of one or more longer bindings — wait for more.
        Matched, ///< A complete binding; `token` is set.
    };

    struct Result
    {
        Status status = Status::NoMatch;
        std::string token;
    };

    /// Bind a chord sequence to a token. A later bind to the same sequence wins.
    void bind(std::vector<KeyChord> const & seq, std::string token);

    /// Match a (possibly partial) chord sequence against the trie.
    Result match(std::vector<KeyChord> const & seq) const;

    bool empty() const { return _root.children.empty(); }

private:
    struct Node
    {
        bool hasToken = false;
        std::string token;
        std::unordered_map<KeyChord, Node, KeyChordHash> children;
    };

    Node _root;
};
} // namespace ventty
