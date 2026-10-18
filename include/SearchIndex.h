#pragma once

#include "Contact.h"

#include <string_view>
#include <vector>

class SearchIndex
{
public:
    virtual void insert(std::string_view key, ContactId id) = 0;

    virtual bool erase(std::string_view key, ContactId id) = 0;

    virtual std::vector<ContactId> exactSearch(std::string_view key) const = 0;

    virtual std::vector<ContactId> prefixSearch(std::string_view prefix) const = 0;

    virtual ~SearchIndex() = default;
};
