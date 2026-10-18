#include "ConsoleUI.h"
#include "SortStrategy.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

ConsoleUI::ConsoleUI(PhoneDirectory &directory) : directory(directory) {}

bool ConsoleUI::parseId(const std::string &text, ContactId &id)
{
    if (text.empty())
        return false;

    for (unsigned char c : text)
    {
        if (c < '0' || c > '9')
            return false;
    }

    try
    {
        std::size_t pos = 0;
        unsigned long long value = std::stoull(text, &pos);

        if (pos != text.size() ||
            value > std::numeric_limits<ContactId>::max())
        {
            return false;
        }

        id = static_cast<ContactId>(value);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void ConsoleUI::printContact(const Contact &contact, bool showId) const
{
    if (showId)
        std::cout << "ID: " << contact.getId() << " |";

    std::cout << contact.getName() << " | " << contact.getPhoneNumber()
              << " | Date Modified - ";

    std::time_t timestamp = contact.getLastModified();
    const std::tm *date = std::localtime(&timestamp);

    if (date != nullptr)
        std::cout << std::put_time(date, "%d/%m/%Y");
    else
        std::cout << "Unknown";

    std::cout << '\n';
}

void ConsoleUI::printContactMatches(const std::vector<ContactId> &ids, bool showId) const
{
    for (const ContactId &id : ids)
    {
        if (const Contact *contact = directory.getContact(id))
            printContact(*contact, showId);
    }
}

void ConsoleUI::addContact()
{
    std::string name, number;

    std::cout << "Enter name: ";
    if (!std::getline(std::cin, name))
        return;

    std::cout << "Enter phone number: ";
    if (!std::getline(std::cin, number))
        return;

    auto id = directory.addContact(std::move(name), std::move(number));

    if (id)
        std::cout << "Contact added successfully.\n";
    else
        std::cout << "Could not add contact. Check the name and phone number; "
                     "the number may already exist.\n";
}

std::vector<ContactId> ConsoleUI::searchContacts()
{
    std::string searchType;
    std::string query;

    std::cout << "Search by (1) Name or (2) Number? ";
    if (!std::getline(std::cin, searchType))
        return {};

    if (searchType != "1" && searchType != "2")
    {
        std::cout << "Invalid search type. Enter 1 for name or 2 for number.\n";
        return {};
    }

    if (searchType == "1")
        std::cout << "Enter name: ";
    else
        std::cout << "Enter phone number or prefix: ";

    if (!std::getline(std::cin, query))
        return {};

    std::vector<ContactId> ids;
    std::unordered_map<ContactId, bool> seen;

    auto appendUnique = [&](const std::vector<ContactId> &matches)
    {
        for (ContactId id : matches)
        {
            auto [it, inserted] = seen.emplace(id, true);
            if (inserted)
                ids.push_back(id);
        }
    };

    if (searchType == "1")
    {
        auto exactMatches = directory.searchNameExact(query);

        if (!exactMatches.empty())
        {
            appendUnique(exactMatches);
        }
        else
        {
            appendUnique(directory.searchNamePrefix(query));
            appendUnique(directory.fuzzySearchName(query));
        }
    }
    else
    {
        appendUnique(directory.searchNumberExact(query));
        appendUnique(directory.searchNumberPrefix(query));
    }

    history.record(searchType == "1" ? SearchKind::Name : SearchKind::PhoneNumber, query);

    return ids;
}

std::optional<ContactId> ConsoleUI::selectContact(const std::vector<ContactId> &candidates, std::string_view action) const
{
    std::string text;
    ContactId id;

    std::cout << "Enter contact ID to " << action
              << " (leave blank to cancel): ";

    if (!std::getline(std::cin, text) || text.empty())
        return std::nullopt;

    if (!parseId(text, id))
    {
        std::cout << "Invalid contact ID.\n";
        return std::nullopt;
    }

    if (std::find(candidates.begin(), candidates.end(), id) == candidates.end())
    {
        std::cout << "That ID is not among the search results.\n";
        return std::nullopt;
    }

    if (directory.getContact(id) == nullptr)
    {
        std::cout << "Contact no longer exists. Please search again.\n";
        return std::nullopt;
    }

    return id;
}

void ConsoleUI::updateContact()
{
    std::vector<ContactId> candidates = searchContacts();

    if (candidates.empty())
    {
        std::cout << "No matching contacts found.\n";
        return;
    }

    std::cout << "Matching contacts (IDs for selection):\n";
    printContactMatches(candidates, true);

    auto selectedId = selectContact(candidates, "update");
    if (!selectedId)
        return;

    std::string name, number;

    std::cout << "Enter new name: ";
    if (!std::getline(std::cin, name))
        return;

    std::cout << "Enter new phone number: ";
    if (!std::getline(std::cin, number))
        return;

    if (directory.updateContact(*selectedId, name, number))
        std::cout << "Contact updated successfully.\n";
    else
        std::cout << "Update failed. Check the name and number; "
                     "the number may already be in use.\n";
}

void ConsoleUI::deleteContact()
{
    std::vector<ContactId> candidates = searchContacts();

    if (candidates.empty())
    {
        std::cout << "No matching contacts found.\n";
        return;
    }

    std::cout << "Matching contacts (IDs for selection):\n";
    printContactMatches(candidates, true);

    auto selectedId = selectContact(candidates, "delete");
    if (!selectedId)
        return;

    if (directory.deleteContact(*selectedId))
        std::cout << "Contact deleted successfully.\n";
    else
        std::cout << "Contact not found or could not be deleted.\n";
}

void ConsoleUI::sortContacts()
{
    auto contacts = directory.getAllContacts();

    if (contacts.empty())
    {
        std::cout << "Directory is empty.\n";
        return;
    }

    std::string type;

    std::cout << "Sort by (name / number / modified): ";

    if (!std::getline(std::cin, type))
        return;

    if (type == "name")
    {
        SortByName strategy;
        strategy.sort(contacts);
    }
    else if (type == "number")
    {
        SortByNumber strategy;
        strategy.sort(contacts);
    }
    else if (type == "modified")
    {
        SortByLastModified strategy;
        strategy.sort(contacts);
    }
    else
    {
        std::cout << "Invalid sort type.\n";
        return;
    }

    for (const Contact &contact : contacts)
        printContact(contact);
}

void ConsoleUI::listContacts() const
{
    auto contacts = directory.getAllContacts();

    if (contacts.empty())
    {
        std::cout << "Directory is empty.\n";
        return;
    }

    for (const Contact &contact : contacts)
        printContact(contact);
}

void ConsoleUI::printHistory() const
{
    const auto records = history.getRecent();

    if (records.empty())
    {
        std::cout << "Search history is empty.\n";
        return;
    }

    for (const SearchRecord &record : records)
    {
        std::cout << searchKindName(record.kind) << " search: " << record.query << " | ";

        std::time_t timestamp = record.timestamp;
        const std::tm *date = std::localtime(&timestamp);

        if (date != nullptr)
            std::cout << std::put_time(date, "%d/%m/%Y %H:%M:%S");
        else
            std::cout << "Unknown time";

        std::cout << '\n';
    }
}

void ConsoleUI::printHelp() const
{
    std::cout
        << "\nCommands:\n"
        << "add     - Add a contact\n"
        << "search  - Search contacts by name or phone number\n"
        << "update  - Search for and update a contact\n"
        << "delete  - Search for and delete a contact\n"
        << "sort    - Sort contacts by name, number, or modification time\n"
        << "list    - List all contacts\n"
        << "history - Show recent searches\n"
        << "help    - Show commands\n"
        << "exit    - Exit\n\n";
}

void ConsoleUI::run()
{
    std::cout << "Welcome to the Phone Directory!\n";
    printHelp();

    std::string command;

    while (true)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, command))
            break;

        if (command == "add")
            addContact();
        else if (command == "search")
        {
            std::vector<ContactId> matches = searchContacts();
            if (matches.empty())
            {
                std::cout << "No matching contacts found.\n";
            }
            else
            {
                std::cout << "Matching contacts:\n";
                printContactMatches(matches, false);
            }
        }
        else if (command == "update")
            updateContact();
        else if (command == "delete")
            deleteContact();
        else if (command == "sort")
            sortContacts();
        else if (command == "list")
            listContacts();
        else if (command == "history")
            printHistory();
        else if (command == "help")
            printHelp();
        else if (command == "exit")
            break;
        else
            std::cout << "Unknown command. Type 'help'.\n";
    }
}
