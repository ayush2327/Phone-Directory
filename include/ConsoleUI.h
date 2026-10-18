#pragma once

#include "PhoneDirectory.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

class ConsoleUI
{
private:
    PhoneDirectory &directory;

    static bool parseId(const std::string &text, ContactId &id);

    void printContact(const Contact &contact, bool showId = false) const;
    void printContactMatches(const std::vector<ContactId> &ids, bool showId) const;
    std::vector<ContactId> searchContacts();
    std::optional<ContactId> selectContact(const std::vector<ContactId> &candidates, std::string_view action) const;

    void addContact();
    void updateContact();
    void deleteContact();
    void listContacts() const;
    void printHelp() const;

public:
    explicit ConsoleUI(PhoneDirectory &directory);

    void run();
};
