#include "SortStrategy.h"

#include <algorithm>

void SortByName::sort(std::vector<Contact> &contacts) const
{
    std::sort(contacts.begin(), contacts.end(),
              [](const Contact &a, const Contact &b)
              {
                  if (a.getName() != b.getName())
                      return a.getName() < b.getName();
                  return a.getPhoneNumber() < b.getPhoneNumber();
              });
}

void SortByNumber::sort(std::vector<Contact> &contacts) const
{
    std::sort(contacts.begin(), contacts.end(),
              [](const Contact &a, const Contact &b)
              {
                  if (a.getPhoneNumber() != b.getPhoneNumber())
                      return a.getPhoneNumber() < b.getPhoneNumber();
                  return a.getName() < b.getName();
              });
}

void SortByLastModified::sort(std::vector<Contact> &contacts) const
{
    std::sort(contacts.begin(), contacts.end(),
              [](const Contact &a, const Contact &b)
              {
                  if (a.getLastModified() != b.getLastModified())
                      return a.getLastModified() > b.getLastModified();
                  return a.getName() < b.getName();
              });
}