#pragma once

#include <vector>

#include "scripts/json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;

class AnchorList
{
private: 
  static json anchorList;

public:


  /**
   * @brief Construct a new Anchor List object.
   */
  AnchorList(std::string id, float x, float y, std::string timestamp);


  /**
   * @brief Returns the the complete anchor list in JSON.
   *
   * @return The JSON object.
   */
  static json getAnchorList();

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
  ~AnchorList();
};
