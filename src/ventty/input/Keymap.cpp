// Copyright (c) 2026 Leon J. Lee
// SPDX-License-Identifier: MIT

#include <ventty/input/Keymap.h>

namespace ventty
{
void Keymap::bind(std::vector<KeyChord> const & seq, std::string token)
{
    Node * node = &_root;
    for (auto const & chord : seq)
        node = &node->children[chord];
    node->hasToken = true;
    node->token = std::move(token);
}

Keymap::Result Keymap::match(std::vector<KeyChord> const & seq) const
{
    Node const * node = &_root;
    for (auto const & chord : seq)
    {
        auto const it = node->children.find(chord);
        if (it == node->children.end())
            return { Status::NoMatch, {} };
        node = &it->second;
    }

    // The whole input was consumed along the trie. If more keys could extend
    // this path, wait for them (this is what makes `d` a prefix of `dd` rather
    // than an immediate match). Presets avoid binding both a prefix and its
    // extension, so a node with children is treated as purely pending.
    if (!node->children.empty())
        return { Status::Pending, {} };
    if (node->hasToken)
        return { Status::Matched, node->token };
    return { Status::NoMatch, {} };
}
} // namespace ventty
