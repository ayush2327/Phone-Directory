#pragma once

#include "Contact.h"
#include "NameTrie.h"
#include "NumberTrie.h"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class PhoneDirectory
{
private:
    std::unordered_map<ContactId, Contact> contacts;

    NameTrie nameIndex;
    NumberTrie numberIndex;

    ContactId nextContactId = 1;

public:
    std::optional<ContactId> addContact(std::string name, std::string phoneNumber);

    bool updateContact(ContactId id, std::string_view newName, std::string_view newPhoneNumber);

    bool deleteContact(ContactId id);

    bool restoreContact(const Contact& snapshot);

    const Contact *getContact(ContactId id) const noexcept;

    std::vector<ContactId> searchNameExact(std::string_view name) const;

    std::vector<ContactId> searchNamePrefix(std::string_view prefix) const;

    std::vector<ContactId> fuzzySearchName(
        std::string_view query,
        std::size_t maxEditDistance = 3,
        std::size_t maxResults = 100) const;

    std::vector<ContactId> searchNumberExact(std::string_view number) const;

    std::vector<ContactId> searchNumberPrefix(std::string_view prefix) const;

    std::vector<Contact> getAllContacts() const;

    std::size_t size() const noexcept;
};
