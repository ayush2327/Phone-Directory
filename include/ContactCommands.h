#pragma once

#include "Command.h"
#include "PhoneDirectory.h"

#include <optional>
#include <string>

class AddContactCommand final : public Command
{
private:
    PhoneDirectory &directory;
    std::string name;
    std::string phoneNumber;
    std::optional<Contact> snapshot;

public:
    AddContactCommand(PhoneDirectory &directory, std::string name, std::string phoneNumber);

    bool execute() override;
    bool undo() override;
};

class DeleteContactCommand final : public Command
{
private:
    PhoneDirectory &directory;
    ContactId id;
    std::optional<Contact> snapshot;

public:
    DeleteContactCommand(PhoneDirectory &directory, ContactId id);

    bool execute() override;
    bool undo() override;
};

class UpdateContactCommand final : public Command
{
private:
    PhoneDirectory &directory;
    ContactId id;
    std::string newName;
    std::string newPhoneNumber;
    std::optional<Contact> previous;

public:
    UpdateContactCommand(PhoneDirectory &directory, ContactId id, std::string newName, std::string newPhoneNumber);

    bool execute() override;
    bool undo() override;
};