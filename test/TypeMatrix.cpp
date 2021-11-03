#include <gtest/gtest.h>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"

#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

template <int Rows, int Cols, typename... Rationals>
using RationalMatrix = TypeMatrix<Rows, Cols, RationalTraits, Rationals...>;

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
  static_assert(std::is_same<RationalMatrix<3, 2, Integer<4>>,
                             RationalMatrix<3, 2, Integer<4>, Integer<4>, Integer<4>, Integer<4>, Integer<4>, Integer<4>>>::value);
}

TEST(TestTypeMatrix, Dimension) {  // NOLINT
  static_assert(Dimension<RationalMatrix<1, 1, Integer<4>>>::value == 0);
  static_assert(Dimension<RationalMatrix<3, 1, Integer<4>>>::value == 1);
  static_assert(Dimension<RationalMatrix<1, 2, Integer<4>>>::value == 1);
  static_assert(Dimension<RationalMatrix<3, 2, Integer<4>>>::value == 2);
}

TEST(TestTypeMatrix, Rows) {  // NOLINT
  static_assert(Rows<RationalMatrix<3, 2, Integer<4>>>::value == 3);
}

TEST(TestTypeMatrix, Cols) {  // NOLINT
  static_assert(Cols<RationalMatrix<3, 2, Integer<4>>>::value == 2);
}

TEST(TestTypeMatrix, DiagonalTypeMatrix) {  // NOLINT
  static_assert(std::is_same<ValueMatrixIdentity<1>, RationalMatrix<1, 1, Integer<1>>>::value);
  static_assert(std::is_same<ValueMatrixIdentity<2>, RationalMatrix<2, 2, Integer<1>, Integer<0>, Integer<0>, Integer<1>>>::value);
  static_assert(std::is_same<ValueMatrixIdentity<3>, RationalMatrix<3, 3, Integer<1>, Integer<0>, Integer<0>, Integer<0>, Integer<1>,
                                                                    Integer<0>, Integer<0>, Integer<0>, Integer<1>>>::value);
}

TEST(TestTypeMatrix, Block) {  // NOLINT
  using A = RationalMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using B = RationalMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using C = RationalMatrix<1, 1, Integer<4>>;
  using D = RationalMatrix<1, 2, Integer<0>, Integer<1>>;
  using E = RationalMatrix<2, 2, Integer<1>, Integer<2>, Integer<4>, Integer<5>>;
  static_assert(std::is_same<Block<A, 0, 0, 2, 3>::type, B>::value);
  static_assert(std::is_same<Block<A, 1, 1, 1, 1>::type, C>::value);
  static_assert(std::is_same<Block<A, 0, 0, 1, 2>::type, D>::value);
  static_assert(std::is_same<Block<A, 0, 1, 2, 2>::type, E>::value);
}

TEST(TestTypeMatrix, Sum) {  // NOLINT
  using A = RationalMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = RationalMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = RationalMatrix<2, 2, Integer<-1>, Integer<4>, Integer<6>, Integer<5>>;
  static_assert(std::is_same<Sum<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, SumInvalidTypes){// NOLINT
                                      ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

 using namespace wuzp;

 struct Traits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Type mismatch.");
    using type = Lhs;
  };
};

 int main() {
  using type = Sum<TypeMatrix<1, 1, Traits, int>, TypeMatrix<1, 1, Traits, double>>::type;
  return 0;
}
)",
                                                                                   "Type mismatch.")}

TEST(TestTypeMatrix, Product1) {  // NOLINT
  using A = RationalMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = RationalMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = RationalMatrix<2, 2, Integer<4>, Integer<2>, Integer<10>, Integer<12>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product2) {  // NOLINT
  using A = RationalMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = RationalMatrix<4, 1, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = RationalMatrix<1, 1, Integer<17>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product3) {  // NOLINT
  using A = RationalMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = Integer<3>;
  using C = RationalMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product4) {  // NOLINT
  using A = Integer<3>;
  using B = RationalMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using C = RationalMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, DotProduct) {  // NOLINT
  using A = RationalMatrix<4, 1, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = RationalMatrix<4, 1, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = Integer<17>;
  static_assert(std::is_same<DotProduct<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Transpose) {  // NOLINT
  using A = RationalMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using B = RationalMatrix<3, 2, Integer<0>, Integer<3>, Integer<1>, Integer<4>, Integer<2>, Integer<5>>;
  static_assert(std::is_same<Transpose<A>::type, B>::value);
}

TEST(TestTypeMatrix, Inverse) {  // NOLINT
  // The inverse is only implemented for unit matrices, not for rational/integer numbers.
  using MassLength = UnitTraits::Product<Mass, Length>::type;
  using MassLength2 = UnitTraits::Product<MassLength, Length>::type;
  using MassMatrixUnits = UnitMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
  using MassInverse = UnitTraits::Inverse<Mass>::type;
  using MassLengthInverse = UnitTraits::Inverse<MassLength>::type;
  using MassLength2Inverse = UnitTraits::Inverse<MassLength2>::type;
  using MassMatrixUnitsInverse = UnitMatrix<3, 3, MassInverse, MassInverse, MassLengthInverse, MassInverse, MassInverse, MassLengthInverse,
                                            MassLengthInverse, MassLengthInverse, MassLength2Inverse>;
  static_assert(std::is_same<Inverse<MassMatrixUnits>::type, MassMatrixUnitsInverse>::value);
}
