#pragma once

#include "SearchIndex.h"

#include <array>
#include <cstddef>
#include <limits>
#include <string_view>
#include <vector>

class NumberTrie final : public SearchIndex
{
private:
    static constexpr std::size_t alphabetSize = 10;

    static constexpr std::size_t noNode = std::numeric_limits<std::size_t>::max();

    struct Node
    {
        std::array<std::size_t, alphabetSize> next{};
        ContactId contactId{};
        bool terminal = false;
    };

    std::vector<Node> trie;

    static bool isValidKey(std::string_view key) noexcept;

    std::size_t findNode(std::string_view key) const noexcept;

    void collectPrefix(std::size_t node, std::vector<ContactId> &result) const;

public:
    NumberTrie();

    void insert(std::string_view key, ContactId id) override;

    bool erase(std::string_view key, ContactId id) override;

    std::vector<ContactId> exactSearch(std::string_view key) const override;

    std::vector<ContactId> prefixSearch(std::string_view prefix) const override;
};