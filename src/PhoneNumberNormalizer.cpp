#include "PhoneNumberNormalizer.h"

#include <string>

std::optional<std::string> PhoneNumberNormalizer::normalize(std::string_view input)
{
    std::string digits;
    bool hasLeadingPlus = false;

    for (char c : input)
    {
        if (c >= '0' && c <= '9')
            digits += c;
        else if (c == '+' && digits.empty() && !hasLeadingPlus)
            hasLeadingPlus = true;
        else if (
            c == ' ' || c == '\t' || c == '\r' || c == '\n' ||
            c == '-' || c == '(' || c == ')' ||
            c == '.' || c == '/')
            continue;
        else
            return std::nullopt;
    }

    if (digits.empty())
        return std::nullopt;

    // Normalize an explicitly supplied Indian country code.
    if (hasLeadingPlus &&
        digits.size() >= 2 &&
        digits[0] == '9' &&
        digits[1] == '1')
    {
        digits.erase(0, 2);
    }

    if (digits.empty())
        return std::nullopt;

    return digits;
}