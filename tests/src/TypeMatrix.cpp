#include <gtest/gtest.h>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/Type.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"

#include "test_compilation_error.hpp"

#include "Unit.hpp"

using namespace wuzp;  // NOLINT

template <int Size>
using ValuesIdentity = DiagonalTypeMatrix<Size, Integer<0>, Integer<1>>;

TEST(TestTypeMatrix, Conversions) {  // NOLINT
  static_assert(std::is_same<HomogeneousTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type,
                             std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);
}

TEST(TestTypeMatrix, Interface) {  // NOLINT
  static_assert(std::is_same<TypeMatrix<3, 2, Integer<4>>,
                             TypeMatrix<3, 2, Integer<4>, Integer<4>, Integer<4>, Integer<4>, Integer<4>, Integer<4>>>::value);
}

TEST(TestTypeMatrix, Dimension) {  // NOLINT
  static_assert(Dimension<TypeMatrix<1, 1, Integer<4>>>::value == 0);
  static_assert(Dimension<TypeMatrix<3, 1, Integer<4>>>::value == 1);
  static_assert(Dimension<TypeMatrix<1, 2, Integer<4>>>::value == 1);
  static_assert(Dimension<TypeMatrix<3, 2, Integer<4>>>::value == 2);
}

TEST(TestTypeMatrix, Rows) {  // NOLINT
  static_assert(Rows<TypeMatrix<3, 2, Integer<4>>>::value == 3);
}

TEST(TestTypeMatrix, Cols) {  // NOLINT
  static_assert(Cols<TypeMatrix<3, 2, Integer<4>>>::value == 2);
}

TEST(TestTypeMatrix, DiagonalTypeMatrix) {  // NOLINT
  static_assert(std::is_same<ValuesIdentity<1>, TypeMatrix<1, 1, Integer<1>>>::value);
  static_assert(std::is_same<ValuesIdentity<2>, TypeMatrix<2, 2, Integer<1>, Integer<0>, Integer<0>, Integer<1>>>::value);
  static_assert(std::is_same<ValuesIdentity<3>, TypeMatrix<3, 3, Integer<1>, Integer<0>, Integer<0>, Integer<0>, Integer<1>, Integer<0>,
                                                           Integer<0>, Integer<0>, Integer<1>>>::value);
}

TEST(TestTypeMatrix, Block) {  // NOLINT
  using A = TypeMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using B = TypeMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using C = TypeMatrix<1, 1, Integer<4>>;
  using D = TypeMatrix<1, 2, Integer<0>, Integer<1>>;
  using E = TypeMatrix<2, 2, Integer<1>, Integer<2>, Integer<4>, Integer<5>>;
  static_assert(std::is_same<Block<A, 0, 0, 2, 3>::type, B>::value);
  static_assert(std::is_same<Block<A, 1, 1, 1, 1>::type, C>::value);
  static_assert(std::is_same<Block<A, 0, 0, 1, 2>::type, D>::value);
  static_assert(std::is_same<Block<A, 0, 1, 2, 2>::type, E>::value);
}

TEST(TestTypeMatrix, Sum) {  // NOLINT
  using A = TypeMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = TypeMatrix<2, 2, Integer<-1>, Integer<4>, Integer<6>, Integer<5>>;
  static_assert(std::is_same<Sum<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, SumInvalidTypes){// NOLINT
                                      ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

using namespace wuzp;

template <typename T>
struct MyType;

template <typename Lhs, typename Rhs>
struct Sum<MyType<Lhs>, MyType<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Type mismatch.");
  using type = Lhs;
};

int main() {
  using type = Sum<TypeMatrix<1, 1, MyType<int>>, TypeMatrix<1, 1, MyType<double>>>::type;
  return 0;
}
)",
                                                                                   "Type mismatch.")}

TEST(TestTypeMatrix, ProductWithTypeOnRhs) {  // NOLINT
  using A = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = Integer<3>;
  using C = TypeMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, ProductWithTypeOnLhs) {  // NOLINT
  using A = Integer<3>;
  using B = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using C = TypeMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, ProductWith1x1TypeMatrixOnRhs) {  // NOLINT
  using A = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<1, 1, Integer<3>>;
  using C = TypeMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, ProductWith1x1TypeMatrixOnLhs) {  // NOLINT
  using A = TypeMatrix<1, 1, Integer<3>>;
  using B = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using C = TypeMatrix<1, 4, Integer<0>, Integer<3>, Integer<6>, Integer<9>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, ProductWith1x1TypeMatricesOnLhsAndRhs) {  // NOLINT
  using A = TypeMatrix<1, 1, Integer<2>>;
  using B = TypeMatrix<1, 1, Integer<3>>;
  using C = TypeMatrix<1, 1, Integer<6>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Product) {  // NOLINT
  using A = TypeMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = TypeMatrix<2, 2, Integer<0>, Integer<3>, Integer<8>, Integer<6>>;
  static_assert(std::is_same<Product<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, TensorProduct) {  // NOLINT
                                       // TODO(remod): Implement
  //  using A = TypeMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  //  using B = TypeMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  //  using C = TypeMatrix<2, 2, Integer<0>, Integer<3>, Integer<8>, Integer<6>>;
  //  static_assert(std::is_same<TensorProduct<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Contraction1) {  // NOLINT
  using A = TypeMatrix<2, 2, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<2, 2, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = TypeMatrix<2, 2, Integer<4>, Integer<2>, Integer<10>, Integer<12>>;
  static_assert(std::is_same<Contraction<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Contraction2) {  // NOLINT
  using A = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<4, 1, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = TypeMatrix<1, 1, Integer<17>>;
  static_assert(std::is_same<Contraction<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, DivisionByType) {  // NOLINT
  using A = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = Integer<3>;
  using C = TypeMatrix<1, 4, Integer<0>, Rational<1, 3>, Rational<2, 3>, Integer<1>>;
  static_assert(std::is_same<Division<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, DivisionBy1x1TypeMatrix) {  // NOLINT
  using A = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<1, 1, Integer<3>>;
  using C = TypeMatrix<1, 4, Integer<0>, Rational<1, 3>, Rational<2, 3>, Integer<1>>;
  static_assert(std::is_same<Division<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Division) {  // NOLINT
  using A = TypeMatrix<1, 4, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<1, 4, Integer<2>, Integer<2>, Integer<1>, Rational<1, 2>>;
  using C = TypeMatrix<1, 4, Integer<0>, Rational<1, 2>, Integer<2>, Integer<6>>;
  static_assert(std::is_same<Division<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, DotProduct) {  // NOLINT
  using A = TypeMatrix<4, 1, Integer<0>, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<4, 1, Integer<-1>, Integer<3>, Integer<4>, Integer<2>>;
  using C = Type<Integer<17>>;
  static_assert(std::is_same<DotProduct<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, CrossProduct) {  // NOLINT
  using A = TypeMatrix<3, 1, Integer<1>, Integer<2>, Integer<3>>;
  using B = TypeMatrix<3, 1, Integer<-1>, Integer<3>, Integer<4>>;
  using C = TypeMatrix<3, 1, Integer<-1>, Integer<-7>, Integer<5>>;
  static_assert(std::is_same<CrossProduct<A, B>::type, C>::value);
}

TEST(TestTypeMatrix, Transpose) {  // NOLINT
  using A = TypeMatrix<2, 3, Integer<0>, Integer<1>, Integer<2>, Integer<3>, Integer<4>, Integer<5>>;
  using B = TypeMatrix<3, 2, Integer<0>, Integer<3>, Integer<1>, Integer<4>, Integer<2>, Integer<5>>;
  static_assert(std::is_same<Transpose<A>::type, B>::value);
}

TEST(TestTypeMatrix, Inverse) {  // NOLINT
  using A = TypeMatrix<2, 3, Integer<1>, Integer<2>, Integer<3>, Integer<10>, Rational<1, 2>, Integer<5>>;
  using B = TypeMatrix<2, 3, Integer<1>, Rational<1, 2>, Rational<1, 3>, Rational<1, 10>, Integer<2>, Rational<1, 5>>;
  static_assert(std::is_same<Inverse<A>::type, B>::value);
}

TEST(TestTypeMatrix, TensorInverse) {  // NOLINT
  // The tensor inverse is only implemented for unit matrices, not for rational/integer numbers.
  using MassLength = Product<Mass, Length>::type;
  using MassLength2 = Product<MassLength, Length>::type;
  using MassInverse = Inverse<Mass>::type;
  using MassLengthInverse = Inverse<MassLength>::type;
  using MassLength2Inverse = Inverse<MassLength2>::type;
  using MassMatrixUnits = TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
  using MassMatrixUnitsInverse = TypeMatrix<3, 3, MassInverse, MassInverse, MassLengthInverse, MassInverse, MassInverse, MassLengthInverse,
                                            MassLengthInverse, MassLengthInverse, MassLength2Inverse>;
  static_assert(std::is_same<Inverse<MassMatrixUnits>::type, MassMatrixUnitsInverse>::value);
}
