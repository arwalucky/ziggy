#include "tag_list.hpp"
#include <iostream> 


json TagList::tagList = json::array();


TagList::TagList(std::string id, json x)
{
  tagList.push_back({
      {"id", id},
      {"anchors", x}
  });
  std::cout << "Added tag" << tagList.dump() << std::endl;
}


bool TagList::isInList(std::string id)
{
  for (auto &[key, value] : tagList.items())
  {
    if (value["id"] == id)
    {
      return true;
    }
  }
  return false;
};


void TagList::addAnchor(std::string id, std::string anchorID) 
{
  for (auto &[key, value] : tagList.items())
  {
    if (value["id"] == id)
    {
      for (auto &[key, value] : value["anchors"].items())
      {
        if (value == anchorID)
        {
          std::cout << "found anchor" << value << std::endl;
          return;
        }
      }
      value["anchors"].push_back(anchorID);
      std::cout << "Added anchor" << value << std::endl;
    }
  }
};

json TagList::getTagList()
{
  return tagList;
};


size_t TagList::size() const
{
  return this->tagList.size();
}

bool TagList::empty() const
{
  return !(this->tagList.size());
}

void TagList::clear()
{
  this->tagList.clear();
}

TagList::~TagList()
{
}