#pragma once

#include <optional>
#include <string>
#include <string_view>

class PhoneNumberNormalizer
{
public:
    static std::optional<std::string> normalize(std::string_view input);
};
