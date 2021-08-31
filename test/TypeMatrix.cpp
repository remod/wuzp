#include <gtest/gtest.h>

#include "ctm/core/TypeMatrix.hpp"

#include "test_compilation_error.hpp"

template <int>
struct ValueT {};

struct ValueTraits {
  template <typename T>
  struct Identity;
  template <int Value>
  struct Identity<ValueT<Value>> {
    using type = ValueT<1>;
  };

  DEFINE_RECURSIVE_TRAIT(Sum)
  template <int ValueLhs, int ValueRhs>
  struct Sum<ValueT<ValueLhs>, ValueT<ValueRhs>> {
    using type = ValueT<ValueLhs + ValueRhs>;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <int ValueLhs, int ValueRhs>
  struct Product<ValueT<ValueLhs>, ValueT<ValueRhs>> {
    using type = ValueT<ValueLhs * ValueRhs>;
  };

  //  template <typename T>
  //  struct Inverse;
  //  template <int Value>
  //  struct Inverse<ValueT<Value>> {
  //    using type = ValueT<1 / Value>;
  //  };
};

template <int Rows, int Cols, int... Values>
using ValueMatrix = ctm::TypeMatrix<Rows, Cols, ValueTraits, ValueT<Values>...>;
template <int Size, int AnyValue>
using ValueMatrixIdentity = ctm::TypeMatrixIdentity<Size, ValueTraits, ValueT<AnyValue>>;

TEST(TestTypeMatrix, Conversions) {  // NOLINT
  static_assert(std::is_same<ctm::HomogeneousTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<ctm::ToRows<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ctm::ToCols<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);
}

TEST(TestTypeMatrix, Interface) {  // NOLINT
  static_assert(std::is_same<ValueMatrix<3, 2, 4>, ValueMatrix<3, 2, 4, 4, 4, 4, 4, 4>>::value);
}

TEST(TestTypeMatrix, Rows) {  // NOLINT
  static_assert(ctm::Rows<ValueMatrix<3, 2, 4>>::value == 3);
}

TEST(TestTypeMatrix, Cols) {  // NOLINT
  static_assert(ctm::Cols<ValueMatrix<3, 2, 4>>::value == 2);
}

TEST(TestTypeMatrix, Identity) {  // NOLINT
  static_assert(std::is_same<ValueMatrixIdentity<1, 1>, ValueMatrix<1, 1, 1>>::value);
  static_assert(std::is_same<ValueMatrixIdentity<2, -1>, ValueMatrix<2, 2, 1>>::value);
  static_assert(std::is_same<ValueMatrixIdentity<3, 35>, ValueMatrix<3, 3, 1>>::value);
}

TEST(TestTypeMatrix, Block) {  // NOLINT
  using A = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using B = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using C = ValueMatrix<1, 1, 4>;
  using D = ValueMatrix<1, 2, 0, 1>;
  using E = ValueMatrix<2, 2, 1, 2, 4, 5>;
  static_assert(std::is_same<ctm::Block<A, 0, 0, 2, 3>::type, B>::value);
  static_assert(std::is_same<ctm::Block<A, 1, 1, 1, 1>::type, C>::value);
  static_assert(std::is_same<ctm::Block<A, 0, 0, 1, 2>::type, D>::value);
  static_assert(std::is_same<ctm::Block<A, 0, 1, 2, 2>::type, E>::value);
}

TEST(TestTypeMatrix, Sum) {  // NOLINT
  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
  using C = ValueMatrix<2, 2, -1, 4, 6, 5>;
  static_assert(std::is_same<ctm::Sum<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, SumInvalidTypes){// NOLINT
                                      ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"

struct Traits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Type mismatch.");
    using type = Lhs;
  };
};

using type = ctm::Sum<ctm::TypeMatrix<1, 1, Traits, int>, ctm::TypeMatrix<1, 1, Traits, double>>::type;

int main() {
  type x;
  return 0;
}
)",
                                                                                   "Type mismatch.")}

TEST(TestTypeMatrix, Product1) {  // NOLINT
  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
  using C = ValueMatrix<2, 2, 4, 2, 10, 12>;
  static_assert(std::is_same<ctm::Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product2) {  // NOLINT
  using A = ValueMatrix<1, 4, 0, 1, 2, 3>;
  using B = ValueMatrix<4, 1, -1, 3, 4, 2>;
  using C = ValueMatrix<1, 1, 17>;
  static_assert(std::is_same<ctm::Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product3) {  // NOLINT
  using A = ValueMatrix<1, 4, 0, 1, 2, 3>;
  using B = ValueT<3>;
  using C = ValueMatrix<1, 4, 0, 3, 6, 9>;
  static_assert(std::is_same<ctm::Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product4) {  // NOLINT
  using A = ValueT<3>;
  using B = ValueMatrix<1, 4, 0, 1, 2, 3>;
  using C = ValueMatrix<1, 4, 0, 3, 6, 9>;
  static_assert(std::is_same<ctm::Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, DotProduct) {  // NOLINT
  using A = ValueMatrix<4, 1, 0, 1, 2, 3>;
  using B = ValueMatrix<4, 1, -1, 3, 4, 2>;
  using C = ValueT<17>;
  static_assert(std::is_same<ctm::DotProduct<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Transpose) {  // NOLINT
  using A = ValueMatrix<2, 3, 0, 1, 2, 3, 4, 5>;
  using B = ValueMatrix<3, 2, 0, 3, 1, 4, 2, 5>;
  static_assert(std::is_same<ctm::Transpose<A>::type, B>::value);
}

// TEST(TestTypeMatrix, Inverse) {  // NOLINT
//  using A = ValueMatrix<2, 2, 0, 1, 2, 3>;
//  using B = ValueMatrix<2, 2, -1, 3, 4, 2>;
//  static_assert(std::is_same<ctm::Inverse<A>::type, B>::value);
//}
