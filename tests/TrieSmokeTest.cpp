
#include "NameTrie.h"
#include "NumberTrie.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int main()
{
    NameTrie nameTrie;
    NumberTrie numberTrie;

    nameTrie.insert("Ayush Verma", 1);
    nameTrie.insert("Ayush Kumar", 2);
    nameTrie.insert("Riya Sen", 3);

    numberTrie.insert("9876543210", 1);
    numberTrie.insert("9876543211", 2);
    numberTrie.insert("9123456780", 3);

    // Exact name search
    auto exactName = nameTrie.exactSearch("Ayush Verma");
    assert(exactName.size() == 1);
    assert(exactName[0] == 1);

    // Name prefix search
    auto namePrefix = nameTrie.prefixSearch("Ay");
    assert(namePrefix.size() == 2);
    assert(std::find(namePrefix.begin(), namePrefix.end(), 1) != namePrefix.end());
    assert(std::find(namePrefix.begin(), namePrefix.end(), 2) != namePrefix.end());

    // Exact phone-number search
    auto exactNumber = numberTrie.exactSearch("9876543211");
    assert(exactNumber.size() == 1);
    assert(exactNumber[0] == 2);

    // Number prefix search
    auto numberPrefix = numberTrie.prefixSearch("98765");
    assert(numberPrefix.size() == 2);

    // Duplicate name-to-ID insertion should not duplicate the ID
    nameTrie.insert("Ayush Verma", 1);
    exactName = nameTrie.exactSearch("Ayush Verma");
    assert(exactName.size() == 1);

    // Conflicting number insertion should not overwrite the existing ID
    numberTrie.insert("9876543210", 99);
    exactNumber = numberTrie.exactSearch("9876543210");
    assert(exactNumber.size() == 1);
    assert(exactNumber[0] == 1);

    // Fuzzy search: one-character typo
    auto fuzzy = nameTrie.fuzzySearch("Ayush Vermx", 1, 100);
    assert(fuzzy.size() == 1);
    assert(fuzzy[0] == 1);

    // Delete a name-index entry
    assert(nameTrie.erase("Ayush Verma", 1));
    assert(nameTrie.exactSearch("Ayush Verma").empty());

    // Delete a number-index entry
    assert(numberTrie.erase("9876543211", 2));
    assert(numberTrie.exactSearch("9876543211").empty());

    assert(!numberTrie.insert("9876543210", 99));
    
    std::cout << "All Trie smoke tests passed.\n";
    return 0;
}
