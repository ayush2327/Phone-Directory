#pragma once

#include "SearchIndex.h"

#include <array>
#include <cstddef>
#include <limits>
#include <set>
#include <string_view>
#include <unordered_set>
#include <vector>

class NameTrie final : public SearchIndex
{
private:
    static constexpr std::size_t alphabetSize = 256;
    static constexpr std::size_t noNode = std::numeric_limits<std::size_t>::max();

    struct Node
    {
        std::array<std::size_t, alphabetSize> next{};
        std::vector<ContactId> contactIds;
    };

    using MemoTable = std::vector<std::vector<std::vector<bool>>>;

    std::vector<Node> trie;

    std::size_t findNode(std::string_view key) const noexcept;

    void collectPrefix(std::size_t node, std::vector<ContactId> &result) const;

    void fuzzySearchDfs(
        std::string_view query,
        std::size_t node,
        std::size_t queryPos,
        std::size_t editDistance,
        std::size_t maxEditDistance,
        std::size_t maxResults,
        MemoTable &visited,
        std::unordered_set<ContactId> &seen,
        std::vector<ContactId> &result) const;

public:
    NameTrie();

    bool insert(std::string_view key, ContactId id) override;

    bool erase(std::string_view key, ContactId id) override;

    std::vector<ContactId> exactSearch(std::string_view key) const override;

    std::vector<ContactId> prefixSearch(std::string_view prefix) const override;

    std::vector<ContactId> fuzzySearch(
        std::string_view query,
        std::size_t maxEditDistance = 3,
        std::size_t maxResults = 100) const;
};
