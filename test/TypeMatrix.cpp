#include <gtest/gtest.h>

#include "ctm/TypeMatrix.hpp"

template <int>
struct ValueT {};

namespace ctm {

template <int Value>
struct GetIdentity<ValueT<Value>> {
  using type = ValueT<1>;
};

template <int ValueLhs, int ValueRhs>
struct Sum<ValueT<ValueLhs>, ValueT<ValueRhs>> {
  using type = ValueT<ValueLhs + ValueRhs>;
};

template <int ValueLhs, int ValueRhs>
struct Product<ValueT<ValueLhs>, ValueT<ValueRhs>> {
  using type = ValueT<ValueLhs * ValueRhs>;
};

}  // namespace ctm

template <int Rows, int Cols, int... Values>
using ValueMatrix = ctm::TypeMatrix<Rows, Cols, ValueT<Values>...>;

TEST(TypeMatrix, Conversions) {  // NOLINT
  static_assert(std::is_same<ctm::HomogeneousTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<ctm::ToRows<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ctm::ToCols<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);
}

TEST(TypeMatrix, Interface) {  // NOLINT
  static_assert(std::is_same<ValueMatrix<3, 2, 4>, ValueMatrix<3, 2, 4, 4, 4, 4, 4, 4>>::value);
}

TEST(TypeMatrix, Identity) {  // NOLINT
  static_assert(std::is_same<ctm::Identity<1, ValueT<1>>, ctm::TypeMatrix<1, 1, ValueT<1>>>::value);
  static_assert(std::is_same<ctm::Identity<2, ValueT<-1>>, ctm::TypeMatrix<2, 2, ValueT<1>>>::value);
  static_assert(std::is_same<ctm::Identity<3, ValueT<35>>, ctm::TypeMatrix<3, 3, ValueT<1>>>::value);
}

TEST(TypeMatrix, Block) {  // NOLINT
  using A = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using B = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using C = ValueMatrix<1, 1, 4>;
  using D = ValueMatrix<1, 2, 0, 1>;
  using E = ValueMatrix<2, 2, 1, 2, 4, 5>;
  static_assert(std::is_same<ctm::GetBlock<A, 0, 0, 2, 3>::type, B>::value);
  static_assert(std::is_same<ctm::GetBlock<A, 1, 1, 1, 1>::type, C>::value);
  static_assert(std::is_same<ctm::GetBlock<A, 0, 0, 1, 2>::type, D>::value);
  static_assert(std::is_same<ctm::GetBlock<A, 0, 1, 2, 2>::type, E>::value);
}

TEST(TypeMatrix, Sum) {  // NOLINT
  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
  using C = ValueMatrix<2, 2, -1, 4, 6, 5>;
  static_assert(std::is_same<ctm::Sum<A, B>::type, C>::value);
}

TEST(TypeMatrix, Product) {  // NOLINT
  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
  using C = ValueMatrix<2, 2, 4, 2, 10, 12>;
  static_assert(std::is_same<ctm::Product<A, B>::type, C>::value);
}

TEST(TypeMatrix, Transpose) {  // NOLINT
  using A = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using B = ValueMatrix<3, 2, 0, 3, 1, 4, 2, 5>;
  static_assert(std::is_same<ctm::Transpose<A>::type, B>::value);
}
