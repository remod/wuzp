#include <gtest/gtest.h>

#include "ctm/TypeMatrix.hpp"

using ctm::HomogeneousTuple;
using ctm::Product;
using ctm::Sum;
using ctm::ToCols;
using ctm::ToRows;
using ctm::Transpose;
using ctm::TypeMatrix;

TEST(TypeMatrix, Interface) {  // NOLINT
  static_assert(std::is_same<TypeMatrix<3, 2, int>, TypeMatrix<3, 2, int, int, int, int, int, int>>::value);
}

TEST(TypeMatrix, Conversions) {  // NOLINT
  static_assert(std::is_same<HomogeneousTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);
}

template <int Value_>
struct ValueT {
  static constexpr int Value = Value_;
};

template <int ValueLhs, int ValueRhs>
struct Sum<ValueT<ValueLhs>, ValueT<ValueRhs>> {
  using type = ValueT<ValueLhs + ValueRhs>;
};

template <int ValueLhs, int ValueRhs>
struct Product<ValueT<ValueLhs>, ValueT<ValueRhs>> {
  using type = ValueT<ValueLhs * ValueRhs>;
};

template <int Rows, int Cols, int... Values>
using ValueMatrix = TypeMatrix<Rows, Cols, ValueT<Values>...>;

TEST(TypeMatrix, Sum) {  // NOLINT
  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
  using C = ValueMatrix<2, 2, -1, 4, 6, 5>;
  static_assert(std::is_same<Sum<A, B>::type, C>::value);
}

TEST(TypeMatrix, Product) {  // NOLINT
  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
  using C = ValueMatrix<2, 2, 4, 2, 10, 12>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TypeMatrix, Transpose) {  // NOLINT
  using A = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using B = ValueMatrix<3, 2, 0, 3, 1, 4, 2, 5>;
  static_assert(std::is_same<Transpose<A>::type, B>::value);
}
