#include "anchorList.hpp"

AnchorList::AnchorList()
{
}

json AnchorList::getAnchorList()
{
  return this->anchorList;
};

void AnchorList::push_back(const std::string id, const float x, const float y, const unsigned int timestamp)
{
  this->anchorList.push_back({
      {"id", id},
      {"x", x},
      {"y", y},
      {"timestamp", timestamp},
  });
}

size_t AnchorList::size() const
{
  return this->anchorList.size();
}

bool AnchorList::empty() const
{
  return !(this->anchorList.size());
}

void AnchorList::clear()
{
  this->anchorList.clear();
}

AnchorList::~AnchorList()
{
}