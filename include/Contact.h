#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <string_view>

using ContactId = std::uint64_t;

class Contact
{
private:
    ContactId id;
    std::string name;
    std::string phoneNumber;
    std::time_t lastModified;

public:
    Contact(
        ContactId id,
        std::string name,
        std::string phoneNumber,
        std::time_t lastModified = std::time(nullptr));

    ContactId getId() const noexcept;

    const std::string& getName() const noexcept;

    const std::string& getPhoneNumber() const noexcept;

    std::time_t getLastModified() const noexcept;

    void updateDetails(std::string_view newName, std::string_view newPhoneNumber);

    void setLastModified(std::time_t timestamp) noexcept;
};
