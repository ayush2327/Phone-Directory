#include "NumberTrie.h"

NumberTrie::NumberTrie() : trie(1) {}

bool NumberTrie::isValidKey(std::string_view key) noexcept
{
    if (key.empty())
        return false;

    for (char c : key)
    {
        if (c < '0' || c > '9')
            return false;
    }

    return true;
}

std::size_t NumberTrie::findNode(std::string_view key) const noexcept
{
    if (!isValidKey(key))
        return noNode;

    std::size_t node = 0;

    for (char c : key)
    {
        std::size_t digit = static_cast<std::size_t>(c - '0');

        std::size_t child = trie[node].next[digit];

        if (child == 0)
            return noNode;

        node = child;
    }

    return node;
}

void NumberTrie::insert(std::string_view key, ContactId id)
{
    if (!isValidKey(key))
        return;

    std::size_t node = 0;

    for (char c : key)
    {
        std::size_t digit = static_cast<std::size_t>(c - '0');

        if (trie[node].next[digit] == 0)
        {
            std::size_t child = trie.size();
            trie[node].next[digit] = child;
            trie.emplace_back();
        }

        node = trie[node].next[digit];
    }

    if (trie[node].terminal && trie[node].contactId != id)
        return;

    trie[node].contactId = id;
    trie[node].terminal = true;
}

bool NumberTrie::erase(std::string_view key, ContactId id)
{
    if (!isValidKey(key))
        return false;

    std::size_t node = findNode(key);

    if (node == noNode)
        return false;

    if (!trie[node].terminal || trie[node].contactId != id)
        return false;

    trie[node].terminal = false;
    trie[node].contactId = ContactId{};

    return true;
}

std::vector<ContactId> NumberTrie::exactSearch(std::string_view key) const
{
    if (!isValidKey(key))
        return {};

    std::size_t node = findNode(key);

    if (node == noNode || !trie[node].terminal)
        return {};

    return {trie[node].contactId};
}

void NumberTrie::collectPrefix(std::size_t node, std::vector<ContactId> &result) const
{
    if (trie[node].terminal)
        result.push_back(trie[node].contactId);

    for (std::size_t digit = 0; digit < alphabetSize; ++digit)
    {
        std::size_t child = trie[node].next[digit];

        if (child != 0)
            collectPrefix(child, result);
    }
}

std::vector<ContactId> NumberTrie::prefixSearch(std::string_view prefix) const
{
    if (!isValidKey(prefix))
        return {};

    std::size_t node = findNode(prefix);

    if (node == noNode)
        return {};

    std::vector<ContactId> result;
    collectPrefix(node, result);

    return result;
}