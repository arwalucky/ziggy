#include "anchorList.hpp"

json AnchorList::anchorList = json::array();


AnchorList::AnchorList(std::string id, float x, float y, unsigned int timestamp)
{
  anchorList.push_back({
      {"id", id},
      {"x", x},
      {"y", y},
      {"timestamp", timestamp}
  });
}

json AnchorList::getAnchorList()
{
  return anchorList;
};


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