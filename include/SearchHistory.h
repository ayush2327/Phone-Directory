#pragma once

#include <cstddef>
#include <ctime>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

enum class SearchKind
{
    Name,
    PhoneNumber
};

struct SearchRecord
{
    SearchKind kind;
    std::string query;
    std::time_t timestamp;
};

class SearchHistory
{
private:
    static constexpr std::size_t capacity = 100;

    std::deque<SearchRecord> entries;

public:
    void record(SearchKind kind, std::string_view query);

    std::vector<SearchRecord> getRecent(std::size_t count = capacity) const;

    void clear() noexcept;

    std::size_t size() const noexcept;
};

std::string_view searchKindName(SearchKind kind) noexcept;