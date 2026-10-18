#include "PhoneDirectory.h"

#include <cassert>
#include <iostream>

int main()
{
    PhoneDirectory directory;

    auto first = directory.addContact("Ayush Verma", "09876543210");

    auto second = directory.addContact("Ayush Kumar", "9876543211");

    assert(first.has_value());
    assert(second.has_value());
    assert(directory.size() == 2);

    // Duplicate phone numbers must be rejected.
    auto duplicate = directory.addContact("Another Person", "09876543210");

    assert(!duplicate.has_value());
    assert(directory.size() == 2);

    // Exact and prefix searches.
    auto exact = directory.searchNameExact("Ayush Verma");
    assert(exact.size() == 1);
    assert(exact[0] == *first);

    auto prefix = directory.searchNamePrefix("Ay");
    assert(prefix.size() == 2);

    auto byNumber = directory.searchNumberExact("09876543210");
    assert(byNumber.size() == 1);
    assert(byNumber[0] == *first);

    // Update a contact and verify both indexes change.
    assert(directory.updateContact(*first, "Ayush Sharma", "9123456780"));

    assert(directory.searchNameExact("Ayush Verma").empty());
    assert(directory.searchNumberExact("09876543210").empty());
    assert(directory.searchNameExact("Ayush Sharma").size() == 1);
    assert(directory.searchNumberExact("9123456780").size() == 1);

    // Deleting the updated contact must remove both index entries.
    assert(directory.deleteContact(*first));
    assert(directory.searchNameExact("Ayush Sharma").empty());
    assert(directory.searchNumberExact("9123456780").empty());
    assert(directory.size() == 1);

    std::cout << "All PhoneDirectory tests passed.\n";
}
