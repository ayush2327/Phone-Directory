#include "SearchHistory.h"

#include <algorithm>

void SearchHistory::record(SearchKind kind, std::string_view query)
{
    if (query.empty())
        return;

    entries.push_back({kind, std::string(query), std::time(nullptr)});

    if (entries.size() > capacity)
        entries.pop_front();
}

std::vector<SearchRecord> SearchHistory::getRecent(std::size_t count) const
{
    std::vector<SearchRecord> result;

    count = std::min(count, entries.size());
    result.reserve(count);

    auto it = entries.rbegin();

    while (result.size() < count)
    {
        result.push_back(*it);
        ++it;
    }

    return result;
}

void SearchHistory::clear() noexcept
{
    entries.clear();
}

std::size_t SearchHistory::size() const noexcept
{
    return entries.size();
}

std::string_view searchKindName(SearchKind kind) noexcept
{
    switch (kind)
    {
    case SearchKind::Name:
        return "Name";

    case SearchKind::PhoneNumber:
        return "Phone number";
    }

    return "Unknown";
}