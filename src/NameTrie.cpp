#include "NameTrie.h"

#include <algorithm>
#include <limits>

NameTrie::NameTrie() : trie(1) {}

std::size_t NameTrie::findNode(std::string_view key) const noexcept
{
    if (key.empty()) return noNode;

    std::size_t node = 0;

    for (unsigned char c : key)
    {
        std::size_t child = trie[node].next[c];

        if (child == 0) return noNode;

        node = child;
    }

    return node;
}

bool NameTrie::insert(std::string_view key, ContactId id)
{
    if (key.empty()) return false;

    std::size_t node = 0;

    for (unsigned char c : key)
    {
        if (trie[node].next[c] == 0)
        {
            std::size_t child = trie.size();
            trie[node].next[c] = child;
            trie.emplace_back();
        }

        node = trie[node].next[c];
    }

    auto &ids = trie[node].contactIds;

    if (std::find(ids.begin(), ids.end(), id) != ids.end()) return false;
    ids.push_back(id); // we can skip find(), if we check using NumberTrie before calling NameTrie::insert()
    return true;
}

bool NameTrie::erase(std::string_view key, ContactId id)
{
    std::size_t node = findNode(key);

    if (node == noNode) return false;

    auto &ids = trie[node].contactIds;

    auto it = std::find(ids.begin(), ids.end(), id);

    if (it == ids.end()) return false;

    *it = ids.back();
    ids.pop_back();

    return true;
}

std::vector<ContactId> NameTrie::exactSearch(std::string_view key) const
{
    std::size_t node = findNode(key);

    if (node == noNode) return {};

    return trie[node].contactIds;
}

void NameTrie::collectPrefix(std::size_t node, std::vector<ContactId> &result) const
{
    const auto &ids = trie[node].contactIds;

    result.insert(result.end(), ids.begin(), ids.end());

    for (std::size_t child : trie[node].next)
    {
        if (child != 0) collectPrefix(child, result);
    }
}

std::vector<ContactId> NameTrie::prefixSearch(std::string_view prefix) const
{
    std::size_t node = findNode(prefix);

    if (node == noNode) return {};

    std::vector<ContactId> result;
    collectPrefix(node, result);

    return result;
}

void NameTrie::fuzzySearchDfs(
    std::string_view query,
    std::size_t node,
    std::size_t queryPos,
    std::size_t editDistance,
    std::size_t maxEditDistance,
    std::size_t maxResults,
    MemoTable &visited,
    std::unordered_set<ContactId> &seen,
    std::vector<ContactId> &result) const
{
    if (result.size() >= maxResults || editDistance > maxEditDistance || visited[editDistance][queryPos][node]) return;

    visited[editDistance][queryPos][node] = true;

    if (queryPos == query.size())
    {
        for (ContactId id : trie[node].contactIds)
        {
            if (seen.insert(id).second) result.push_back(id);

            if (result.size() >= maxResults) return;
        }
    }

    // Exact character match.
    if (queryPos < query.size())
    {
        unsigned char c = static_cast<unsigned char>(query[queryPos]);

        std::size_t child = trie[node].next[c];

        if (child != 0)
        {
            fuzzySearchDfs(
                query, child, queryPos + 1,
                editDistance, maxEditDistance, maxResults,
                visited, seen, result);
        }
    }

    for (std::size_t c = 0; c < alphabetSize; ++c)
    {
        std::size_t child = trie[node].next[c];

        if (child == 0) continue;

        // Substitution.
        if (queryPos < query.size() && c != static_cast<unsigned char>(query[queryPos]))
        {
            fuzzySearchDfs(
                query, child, queryPos + 1,
                editDistance + 1,
                maxEditDistance, maxResults,
                visited, seen, result);
        }

        // Insertion: consume a Trie character, not a query character.
        fuzzySearchDfs(
            query, child, queryPos,
            editDistance + 1,
            maxEditDistance, maxResults,
            visited, seen, result);

        if (result.size() >= maxResults) return;
    }

    // Deletion: consume a query character without moving in the Trie.
    if (queryPos < query.size())
    {
        fuzzySearchDfs(
            query, node, queryPos + 1,
            editDistance + 1,
            maxEditDistance, maxResults,
            visited, seen, result);
    }
}

std::vector<ContactId> NameTrie::fuzzySearch(
    std::string_view query,
    std::size_t maxEditDistance,
    std::size_t maxResults) const
{
    if (maxResults == 0) return {};

    constexpr std::size_t maxMemoStates = 10'000'000;

    if (maxEditDistance == std::numeric_limits<std::size_t>::max()) return {};

    const std::size_t editStates = maxEditDistance + 1;
    const std::size_t queryStates = query.size() + 1;

    if (editStates > maxMemoStates / queryStates) return {};

    const std::size_t editQueryStates = editStates * queryStates;

    if (trie.size() > maxMemoStates / editQueryStates) return {};

    MemoTable visited(editStates, std::vector<std::vector<bool>>(queryStates, std::vector<bool>(trie.size(), false)));

    std::unordered_set<ContactId> seen;
    std::vector<ContactId> result;

    fuzzySearchDfs(
        query, 0, 0, 0,
        maxEditDistance, maxResults,
        visited, seen, result);

    return result;
}