#pragma once

#include <vector>
#include "anchor.hpp"

#include "scripts/json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;

/**
 * @brief Anchor management vector
 *
 * Extends the default std::vector.
 */
class AnchorList
{
private:
  json anchorList;

public:
  /**
   * @brief Construct a new Anchor List object.
   */
  AnchorList();

  json getAnchorList();

  /**
   * @brief Appends the given Anchor to the end of the JSON.
   *
   * @param id 16-bit ID, 4 hex-digit-presentation.
   * @param x Position of the anchor in meters.
   * @param y Position of the anchor in meters.
   * @param timestamp Time of the first addition in milliseconds.
   */
  void push_back(const std::string id, const float x, const float y, const unsigned int timestamp);

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
   * @brief Returns a reference to the Anchor at specified location pos, with bounds checking.
   *
   * @param pos position of the Anchor to return.
   * @return Reference to the requested Anchor.
   */
  Anchor at(const size_t pos) const;

  /**
   * @brief Erases all elements from the container. After this call, size() returns zero.
   */
  void clear();

  /**
   * @brief Destroy the Anchor List object.
   */
  ~AnchorList();
};
