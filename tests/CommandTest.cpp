#include "CommandManager.h"
#include "ContactCommands.h"

#include <cassert>
#include <iostream>
#include <memory>

int main()
{
    PhoneDirectory directory;
    CommandManager manager;

    // Add, undo, and redo.
    assert(manager.execute(std::make_unique<AddContactCommand>(directory, "Ayush Verma", "09876543210")));

    auto ids = directory.searchNameExact("Ayush Verma");
    assert(ids.size() == 1);

    ContactId id = ids[0];

    assert(manager.undo());
    assert(directory.getContact(id) == nullptr);

    assert(manager.redo());
    assert(directory.getContact(id) != nullptr);

    // Update, undo, and redo.
    assert(manager.execute(std::make_unique<UpdateContactCommand>(directory, id, "Ayush Sharma", "9123456780")));

    assert(directory.searchNameExact("Ayush Sharma").size() == 1);

    assert(manager.undo());
    assert(directory.searchNameExact("Ayush Sharma").empty());
    assert(directory.searchNameExact("Ayush Verma").size() == 1);
    assert(directory.searchNumberExact("09876543210").size() == 1);

    assert(manager.redo());
    assert(directory.searchNameExact("Ayush Sharma").size() == 1);
    assert(directory.searchNumberExact("9123456780").size() == 1);

    // Delete, undo, and redo.
    assert(manager.execute(std::make_unique<DeleteContactCommand>(directory, id)));

    assert(directory.getContact(id) == nullptr);

    assert(manager.undo());
    assert(directory.getContact(id) != nullptr);
    assert(directory.searchNameExact("Ayush Sharma").size() == 1);

    assert(manager.redo());
    assert(directory.getContact(id) == nullptr);

    // Undo after redo must restore the same ID.
    assert(manager.undo());
    assert(directory.getContact(id) != nullptr);

    std::cout << "All Command tests passed.\n";
}