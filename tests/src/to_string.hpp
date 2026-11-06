#pragma once

#include <sstream>
#include <string>

//! Writes the given object to a string with its stream operator.
template <typename T>
std::string toString(const T& object) {
  std::ostringstream stream;
  stream << object;
  return stream.str();
}
