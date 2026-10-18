#include "PhoneNumberNormalizer.h"

#include <cassert>
#include <iostream>

int main()
{
    auto a = PhoneNumberNormalizer::normalize("98765 43210");
    assert(a && *a == "9876543210");

    auto b = PhoneNumberNormalizer::normalize("(98765)-43210");
    assert(b && *b == "9876543210");

    auto c = PhoneNumberNormalizer::normalize("+91 98765-43210");
    assert(c && *c == "9876543210");

    auto d = PhoneNumberNormalizer::normalize("09876543210");
    assert(d && *d == "09876543210");

    auto e = PhoneNumberNormalizer::normalize("98A76543210");
    assert(!e);

    auto f = PhoneNumberNormalizer::normalize("");
    assert(!f);

    auto g = PhoneNumberNormalizer::normalize("+91");
    assert(!g);

    std::cout << "All phone-number normalization tests passed.\n";
}