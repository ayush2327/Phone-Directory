#include "ContactCommands.h"

#include <utility>

AddContactCommand::AddContactCommand(PhoneDirectory &directory, std::string name, std::string phoneNumber)
    : directory(directory),
      name(std::move(name)),
      phoneNumber(std::move(phoneNumber)) {}

bool AddContactCommand::execute()
{
    if (snapshot)
        return directory.restoreContact(*snapshot);

    auto id = directory.addContact(name, phoneNumber);

    if (!id)
        return false;

    const Contact *contact = directory.getContact(*id);

    if (contact == nullptr)
    {
        directory.deleteContact(*id);
        return false;
    }

    snapshot = *contact;
    return true;
}

bool AddContactCommand::undo()
{
    if (!snapshot)
        return false;

    return directory.deleteContact(snapshot->getId());
}

DeleteContactCommand::DeleteContactCommand(PhoneDirectory &directory, ContactId id)
    : directory(directory), id(id) {}

bool DeleteContactCommand::execute()
{
    if (!snapshot)
    {
        const Contact *contact = directory.getContact(id);

        if (contact == nullptr)
            return false;

        snapshot = *contact;
    }

    return directory.deleteContact(id);
}

bool DeleteContactCommand::undo()
{
    if (!snapshot)
        return false;

    return directory.restoreContact(*snapshot);
}

UpdateContactCommand::UpdateContactCommand(PhoneDirectory &directory, ContactId id, std::string newName, std::string newPhoneNumber)
    : directory(directory),
      id(id),
      newName(std::move(newName)),
      newPhoneNumber(std::move(newPhoneNumber)) {}

bool UpdateContactCommand::execute()
{
    if (!previous)
    {
        const Contact *contact = directory.getContact(id);

        if (contact == nullptr)
            return false;

        previous = *contact;
    }

    return directory.updateContact(id, newName, newPhoneNumber);
}

bool UpdateContactCommand::undo()
{
    if (!previous)
        return false;

    return directory.restoreContact(*previous);
}