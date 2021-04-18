#include <gtest/gtest.h>

#include "ctm/TypeMatrix.hpp"

using ctm::ToCols;
using ctm::ToRows;
using ctm::ToTuple;
using ctm::TypeMatrix;
using ctm::TypeMatrixImpl;

TEST(TypeMatrix, Interface) {  // NOLINT
  static_assert(std::is_same<ToTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<TypeMatrix<3, 2, int>, TypeMatrixImpl<3, 2, std::tuple<int, int, int, int, int, int>>>::value);
  static_assert(
      std::is_same<TypeMatrix<3, 2, int, int, int, int, int, int>, TypeMatrixImpl<3, 2, std::tuple<int, int, int, int, int, int>>>::value);
}

TEST(TypeMatrix, Conversions) {  // NOLINT
  static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);
}
