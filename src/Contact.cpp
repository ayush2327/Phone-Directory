
#include "Contact.h"

#include <utility>

Contact::Contact(
    ContactId id,
    std::string name,
    std::string phoneNumber,
    std::time_t lastModified)
    : id(id),
      name(std::move(name)),
      phoneNumber(std::move(phoneNumber)),
      lastModified(lastModified) {}

ContactId Contact::getId() const noexcept
{
    return id;
}

const std::string &Contact::getName() const noexcept
{
    return name;
}

const std::string &Contact::getPhoneNumber() const noexcept
{
    return phoneNumber;
}

std::time_t Contact::getLastModified() const noexcept
{
    return lastModified;
}

void Contact::updateDetails(
    std::string_view newName,
    std::string_view newPhoneNumber)
{
    name = newName;
    phoneNumber = newPhoneNumber;
    lastModified = std::time(nullptr);
}

void Contact::setLastModified(std::time_t timestamp) noexcept
{
    lastModified = timestamp;
}
