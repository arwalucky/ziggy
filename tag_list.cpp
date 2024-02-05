#include "tag_list.hpp"
#include <iostream> 


json TagList::tagList = json::array();


TagList::TagList(std::string id, json anchors, std::string timestamp)
{
  tagList.push_back({
      {"id", id},
      {"anchors", anchors},
      {"timestamp", timestamp}
  });
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

json TagList::getAnchorListForTag(std::string id)
{
  for (auto &[key, value] : tagList.items())
  {
    if (value["id"] == id)
    {
      return value["anchors"];
    }
  }
  return json::array();
};

void TagList::updateRange(std::string id, std::string anchorID, std::string distance) 
{
  for (auto &[key, value] : tagList.items())
  {
    if (value["id"] == id)
    {
      for (auto &[key1, value1] : value["anchors"].items())
      {
        if (value1["anchorID"] == anchorID)
        {
          value1["distance"] = distance;
          return;
        }
      }
      
      value["anchors"].push_back({{"anchorID", anchorID}, {"distance", distance}});
      std::cout << "AnchorID: " << anchorID << " Distance: " << distance << std::endl;
    }
  }
};

void TagList::removeAnchor(std::string id, std::string anchorID)
{
  for (auto &[key, value] : tagList.items())
  {
    if (value["id"] == id)
    {

      value["anchors"].erase(value["anchors"].find(anchorID));

      std::cout << value["anchors"] << std::endl;  
      return;
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