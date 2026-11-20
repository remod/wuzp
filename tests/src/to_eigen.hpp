#pragma once

#include <Eigen/Core>

#include "wuzp/core/traits.hpp"

//! The values of quantities or of a tensor, with any value matrix backend, as an Eigen matrix of the same fixed size, e.g. to
//! compare them with the results of Eigen's geometry module or of kindr.
template <typename ObjectT_>
auto toEigen(const ObjectT_& object) {
  constexpr int rows = wuzp::Rows<ObjectT_>::value;
  constexpr int cols = wuzp::Cols<ObjectT_>::value;
  const auto& values = object.getValues();
  Eigen::Matrix<typename ObjectT_::ValueT, rows, cols> matrix;
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      matrix(row, col) = values.get(row, col);
    }
  }
  return matrix;
}
