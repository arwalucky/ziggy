#ifndef TAG_LIST_HPP
#define TAG_LIST_HPP

#include <vector>

#include "scripts/json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;

class TagList
{
protected:
  std::string id;
  float x;
  float y;
  unsigned int timestamp;

public:
  static json tagList;
  /**
   * @brief Construct a new Anchor List object.
   */
  TagList(std::string id, json x);

  static bool isInList(std::string id);

  static  void addAnchor(std::string id, std::string anchorID);
  /**
   * @brief Returns the the complete anchor list in JSON.
   *
   * @return The JSON object.
   */
  static json getTagList();

  /**
   * @brief Returns the number of anchors in the vector.
   *
   * @return The number of anchors in the vector.
   */
  size_t size() const;

  /**
   * @brief Checks if the vector has no anchors.
   *
   * @return true if the vector is empty.
   * @return false if the vector is not empty.
   */
  bool empty() const;

  /**
   * @brief Erases all elements from the container. After this call, size() returns zero.
   */
  void clear();

  /**
   * @brief Destroy the Anchor List object.
   */
  ~TagList();
};

#endif /* AA6EEA53_B42B_48F5_BB2D_6FDBB016607E */
