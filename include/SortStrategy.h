#pragma once

#include "Contact.h"

#include <vector>

class SortStrategy
{
public:
    virtual void sort(std::vector<Contact> &contacts) const = 0;
    virtual ~SortStrategy() = default;
};

class SortByName final : public SortStrategy
{
public:
    void sort(std::vector<Contact> &contacts) const override;
};

class SortByNumber final : public SortStrategy
{
public:
    void sort(std::vector<Contact> &contacts) const override;
};

class SortByLastModified final : public SortStrategy
{
public:
    void sort(std::vector<Contact> &contacts) const override;
};