#include "PhoneDirectory.h"
#include "PhoneNumberNormalizer.h"

#include <algorithm>
#include <ctime>
#include <limits>
#include <utility>

bool PhoneDirectory::restoreContact(const Contact &snapshot)
{
    const ContactId id = snapshot.getId();

    if (snapshot.getName().empty())
        return false;

    auto normalized = PhoneNumberNormalizer::normalize(snapshot.getPhoneNumber());

    if (!normalized)
        return false;

    std::string number = std::move(*normalized);

    auto numberMatches = numberIndex.exactSearch(number);

    if (!numberMatches.empty() && numberMatches.front() != id)
        return false;

    auto it = contacts.find(id);

    if (it == contacts.end())
    {
        auto [contactIt, inserted] = contacts.emplace(id, Contact(id, snapshot.getName(), number, snapshot.getLastModified()));

        if (!inserted)
            return false;

        const Contact &contact = contactIt->second;

        if (!nameIndex.insert(contact.getName(), id))
        {
            contacts.erase(contactIt);
            return false;
        }

        if (!numberIndex.insert(contact.getPhoneNumber(), id))
        {
            nameIndex.erase(contact.getName(), id);
            contacts.erase(contactIt);
            return false;
        }
    }
    else
    {
        Contact &current = it->second;
        Contact previous = current;

        if (!nameIndex.erase(previous.getName(), id))
            return false;

        if (!numberIndex.erase(previous.getPhoneNumber(), id))
        {
            nameIndex.insert(previous.getName(), id);
            return false;
        }

        current.updateDetails(snapshot.getName(), number);
        current.setLastModified(snapshot.getLastModified());

        bool nameInserted = nameIndex.insert(current.getName(), id);

        bool numberInserted = nameInserted && numberIndex.insert(current.getPhoneNumber(), id);

        if (!nameInserted || !numberInserted)
        {
            if (nameInserted)
                nameIndex.erase(current.getName(), id);

            current.updateDetails(previous.getName(), previous.getPhoneNumber());
            current.setLastModified(previous.getLastModified());

            nameIndex.insert(previous.getName(), id);
            numberIndex.insert(previous.getPhoneNumber(), id);

            return false;
        }
    }

    if (nextContactId != 0 && id >= nextContactId)
    {
        if (id == std::numeric_limits<ContactId>::max())
            nextContactId = 0;
        else
            nextContactId = id + 1;
    }

    return true;
}

std::optional<ContactId> PhoneDirectory::addContact(std::string name, std::string phoneNumber)
{
    if (name.empty())
        return std::nullopt;

    auto normalized = PhoneNumberNormalizer::normalize(phoneNumber);

    if (!normalized)
        return std::nullopt;

    phoneNumber = std::move(*normalized);

    if (nextContactId == 0)
        return std::nullopt;

    if (!numberIndex.exactSearch(phoneNumber).empty())
        return std::nullopt;

    ContactId id = nextContactId;

    auto [it, inserted] = contacts.emplace(id, Contact(id, std::move(name), std::move(phoneNumber)));

    if (!inserted)
        return std::nullopt;

    const Contact &contact = it->second;

    if (!nameIndex.insert(contact.getName(), id))
    {
        contacts.erase(it);
        return std::nullopt;
    }

    if (!numberIndex.insert(contact.getPhoneNumber(), id))
    {
        nameIndex.erase(contact.getName(), id);
        contacts.erase(it);
        return std::nullopt;
    }

    if (nextContactId == std::numeric_limits<ContactId>::max())
        nextContactId = 0;
    else
        ++nextContactId;

    return id;
}

bool PhoneDirectory::deleteContact(ContactId id)
{
    auto it = contacts.find(id);

    if (it == contacts.end())
        return false;

    const std::string name = it->second.getName();
    const std::string number = it->second.getPhoneNumber();

    if (!nameIndex.erase(name, id))
        return false;

    if (!numberIndex.erase(number, id))
    {
        nameIndex.insert(name, id);
        return false;
    }

    contacts.erase(it);
    return true;
}

bool PhoneDirectory::updateContact(ContactId id, std::string_view newNameView, std::string_view newPhoneNumberView)
{
    auto it = contacts.find(id);

    if (it == contacts.end())
        return false;

    if (newNameView.empty())
        return false;

    auto normalized = PhoneNumberNormalizer::normalize(newPhoneNumberView);

    if (!normalized)
        return false;

    std::string newName(newNameView);
    std::string newPhoneNumber = std::move(*normalized);

    auto existing = numberIndex.exactSearch(newPhoneNumber);

    if (!existing.empty() && existing.front() != id)
        return false;

    Contact &contact = it->second;

    const std::string oldName = contact.getName();
    const std::string oldPhoneNumber = contact.getPhoneNumber();
    const std::time_t oldModified = contact.getLastModified();

    if (!nameIndex.erase(oldName, id))
        return false;

    if (!numberIndex.erase(oldPhoneNumber, id))
    {
        nameIndex.insert(oldName, id);
        return false;
    }

    contact.updateDetails(newName, newPhoneNumber);

    bool nameInserted = nameIndex.insert(contact.getName(), id);
    bool numberInserted = false;

    if (nameInserted)
        numberInserted = numberIndex.insert(contact.getPhoneNumber(), id);

    if (nameInserted && numberInserted)
        return true;

    if (nameInserted)
        nameIndex.erase(contact.getName(), id);

    contact.updateDetails(oldName, oldPhoneNumber);
    contact.setLastModified(oldModified);

    nameIndex.insert(oldName, id);
    numberIndex.insert(oldPhoneNumber, id);

    return false;
}

const Contact *PhoneDirectory::getContact(ContactId id) const noexcept
{
    auto it = contacts.find(id);

    if (it == contacts.end())
        return nullptr;

    return &it->second;
}

std::vector<ContactId> PhoneDirectory::searchNameExact(std::string_view name) const
{
    return nameIndex.exactSearch(name);
}

std::vector<ContactId> PhoneDirectory::searchNamePrefix(std::string_view prefix) const
{
    return nameIndex.prefixSearch(prefix);
}

std::vector<ContactId> PhoneDirectory::fuzzySearchName(std::string_view query, std::size_t maxEditDistance, std::size_t maxResults) const
{
    return nameIndex.fuzzySearch(query, maxEditDistance, maxResults);
}

std::vector<ContactId> PhoneDirectory::searchNumberExact(std::string_view number) const
{
    auto normalized = PhoneNumberNormalizer::normalize(number);

    if (!normalized)
        return {};

    return numberIndex.exactSearch(*normalized);
}

std::vector<ContactId> PhoneDirectory::searchNumberPrefix(std::string_view prefix) const
{
    auto normalized = PhoneNumberNormalizer::normalize(prefix);

    if (!normalized)
        return {};

    return numberIndex.prefixSearch(*normalized);
}

std::vector<Contact> PhoneDirectory::getAllContacts() const
{
    std::vector<Contact> result;
    result.reserve(contacts.size());

    for (const auto &[id, contact] : contacts)
        result.push_back(contact);

    return result;
}

std::size_t PhoneDirectory::size() const noexcept
{
    return contacts.size();
}