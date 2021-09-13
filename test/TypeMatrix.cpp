#include <gtest/gtest.h>

#include "ctm/core/Integer.hpp"
#include "ctm/core/Rational.hpp"
#include "ctm/core/TypeMatrix.hpp"

#include "test_compilation_error.hpp"

using namespace ctm;  // NOLINT

template <int Rows, int Cols, typename... Rationals>
using ValueMatrix = TypeMatrix<Rows, Cols, RationalTraits, Rationals...>;

template <int Size>
using ValueMatrixIdentity = DiagonalTypeMatrix<Size, RationalTraits, Integer<0>, Integer<1>>;

TEST(TestTypeMatrix, Conversions) {  // NOLINT
  static_assert(std::is_same<HomogeneousTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);
}

TEST(TestTypeMatrix, Interface) {  // NOLINT
  static_assert(std::is_same<ValueMatrix<3, 2, Integer<4>>,
                             ValueMatrix<3, 2, Integer<4>, Integer<4>, Integer<4>, Integer<4>, Integer<4>, Integer<4>>>::value);
}

TEST(TestTypeMatrix, Rows) {  // NOLINT
  static_assert(Rows<ValueMatrix<3, 2, Integer<4>>>::value == 3);
}

TEST(TestTypeMatrix, Cols) {  // NOLINT
  static_assert(Cols<ValueMatrix<3, 2, Integer<4>>>::value == 2);
}

TEST(TestTypeMatrix, DiagonalTypeMatrix) {  // NOLINT
  static_assert(std::is_same<ValueMatrixIdentity<1>, ValueMatrix<1, 1, Integer<1>>>::value);
  static_assert(std::is_same<ValueMatrixIdentity<2>, ValueMatrix<2, 2, Integer<1>, Integer<0>, Integer<0>, Integer<1>>>::value);
  static_assert(std::is_same<ValueMatrixIdentity<3>, ValueMatrix<3, 3, Integer<1>, Integer<0>, Integer<0>, Integer<0>, Integer<1>,
                                                                 Integer<0>, Integer<0>, Integer<0>, Integer<1>>>::value);
}

TEST(TestTypeMatrix, Block) {  // NOLINT
  using A = ValueMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using B = ValueMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using C = ValueMatrix<1, 1, Integer<4>>;
  using D = ValueMatrix<1, 2, Integer<0>, Integer<1>>;
  using E = ValueMatrix<2, 2, Integer<1>, Integer<2>, Integer<4>, Integer<5>>;
  static_assert(std::is_same<Block<A, 0, 0, 2, 3>::type, B>::value);
  static_assert(std::is_same<Block<A, 1, 1, 1, 1>::type, C>::value);
  static_assert(std::is_same<Block<A, 0, 0, 1, 2>::type, D>::value);
  static_assert(std::is_same<Block<A, 0, 1, 2, 2>::type, E>::value);
}

TEST(TestTypeMatrix, Sum) {  // NOLINT
  using A = ValueMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = ValueMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = ValueMatrix<2, 2, Integer<-1>, Integer<4>, Integer<6>, Integer<5>>;
  static_assert(std::is_same<Sum<A, B>::type, C>::value);
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

int main() {
  using type = ctm::Sum<ctm::TypeMatrix<1, 1, Traits, int>, ctm::TypeMatrix<1, 1, Traits, double>>::type;
  return 0;
}
)",
                                                                                   "Type mismatch.")}

TEST(TestTypeMatrix, Product1) {  // NOLINT
  using A = ValueMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = ValueMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = ValueMatrix<2, 2, Integer<4>, Integer<2>, Integer<10>, Integer<12>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product2) {  // NOLINT
  using A = ValueMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = ValueMatrix<4, 1, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = ValueMatrix<1, 1, Integer<17>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product3) {  // NOLINT
  using A = ValueMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = Integer<3>;
  using C = ValueMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product4) {  // NOLINT
  using A = Integer<3>;
  using B = ValueMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using C = ValueMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, DotProduct) {  // NOLINT
  using A = ValueMatrix<4, 1, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = ValueMatrix<4, 1, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = Integer<17>;
  static_assert(std::is_same<DotProduct<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Transpose) {  // NOLINT
  using A = ValueMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using B = ValueMatrix<3, 2, Integer<0>, Integer<3>, Integer<1>, Integer<4>, Integer<2>, Integer<5>>;
  static_assert(std::is_same<Transpose<A>::type, B>::value);
}
