// Both value matrix backends, and the value matrix archetype of the tests, in one translation unit. Their trait specializations
// are constrained to their own matrices, so they can be used side by side, and operations derive scalars from the backend of
// their operands (see "The backend interface" in DESIGN.md). The other tests run against the backend of the configuration
// (ValueMatrix.hpp).

#include <array>
#include <cmath>
#include <cstddef>
#include <type_traits>

#include <gtest/gtest.h>
#include <Eigen/Core>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp_blitz/ValueMatrix.hpp"
#include "wuzp_eigen/ValueMatrix.hpp"

#include "archetype_values/ValueMatrix.hpp"
#include "coordinate_systems.hpp"

#include "Unit.hpp"

// The backends implement the interface of the core. They do not check this in their headers, since it would cost compile
// time in every translation unit of their users (see ValueMatrixBackend). The Blitz++ backend computes matrix inverses and
// determinants by Gauss-Jordan elimination (see "Backend support for inverses" in DESIGN.md).
static_assert(wuzp::ValueMatrixBackend<wuzp_eigen::ValueMatrix>);
static_assert(wuzp::InvertibleValueMatrixBackend<wuzp_eigen::ValueMatrix>);
static_assert(wuzp::ValueMatrixBackend<wuzp_blitz::ValueMatrix>);
static_assert(wuzp::InvertibleValueMatrixBackend<wuzp_blitz::ValueMatrix>);
// The archetype provides exactly what the concepts require: the tests pass with it (the presets *-archetype-archetype) if the
// core uses nothing beyond them.
static_assert(wuzp::ValueMatrixBackend<archetype_values::ValueMatrix>);
static_assert(wuzp::InvertibleValueMatrixBackend<archetype_values::ValueMatrix>);

namespace {

// A matrix of a linear algebra library without a backend: no traits are specialized for it.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Matrix {};

template <typename ValueT_, int RowsV_, int ColsV_>
using IncompleteValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<RowsV_, ColsV_>, ValueT_, Matrix<ValueT_, RowsV_, ColsV_>>;

// Eigen arrays that are not aligned: the traits of wuzp_eigen are specialized for them, but evaluate their results into aligned
// arrays (see Rebind), which are not the matrices of this alias.
template <typename ValueT_, int RowsV_, int ColsV_>
using UnalignedValueMatrix = wuzp::ValuesWrapper<
    wuzp::MatrixSize<RowsV_, ColsV_>, ValueT_,
    Eigen::Array<ValueT_, RowsV_, ColsV_, Eigen::DontAlign | (RowsV_ == 1 && ColsV_ != 1 ? Eigen::RowMajor : Eigen::ColMajor)>>;

using EigenPosition = wuzp::PositionVector3<wuzp_eigen::ValueMatrix, double, CoordinateSystemA>;
using BlitzPosition = wuzp::PositionVector3<wuzp_blitz::ValueMatrix, double, CoordinateSystemA>;

}  // namespace

// Its value matrices are value matrices, but it does not implement the interface of the core.
static_assert(wuzp::AnyValueMatrix<IncompleteValueMatrix>);
static_assert(!wuzp::ValueMatrixBackend<IncompleteValueMatrix>);
static_assert(!wuzp::InvertibleValueMatrixBackend<IncompleteValueMatrix>);

// The traits exist, but their results are not its value matrices: the concept checks the results, not only the traits.
static_assert(wuzp::AnyValueMatrix<UnalignedValueMatrix>);
static_assert(!wuzp::ValueMatrixBackend<UnalignedValueMatrix>);

TEST(TestValueMatrixBackends, Quantities) {  // NOLINT
  const wuzp_eigen::QuantityD<Length> eigenLength{6.0};
  const wuzp_eigen::QuantityD<Time> eigenTime{2.0};
  const wuzp_blitz::QuantityD<Length> blitzLength{6.0};
  const wuzp_blitz::QuantityD<Time> blitzTime{2.0};

  const wuzp_eigen::QuantityD<Velocity> eigenVelocity = eigenLength / eigenTime;
  const wuzp_blitz::QuantityD<Velocity> blitzVelocity = blitzLength / blitzTime;
  EXPECT_EQ(eigenVelocity.getValue(), 3.0);
  EXPECT_EQ(blitzVelocity.getValue(), 3.0);

  // A scalar is converted into a quantity of the backend of the other operand, also on the left.
  static_assert(std::is_same_v<decltype(eigenLength * 2.0), wuzp_eigen::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(blitzLength * 2.0), wuzp_blitz::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(2.0 * eigenLength), wuzp_eigen::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(2.0 * blitzLength), wuzp_blitz::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(eigenLength / 2.0), wuzp_eigen::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(blitzLength / 2.0), wuzp_blitz::QuantityD<Length>>);
  EXPECT_EQ((eigenLength * 2.0).getValue(), 12.0);
  EXPECT_EQ((blitzLength * 2.0).getValue(), 12.0);
  EXPECT_EQ((2.0 * eigenLength).getValue(), 12.0);
  EXPECT_EQ((2.0 * blitzLength).getValue(), 12.0);
  EXPECT_EQ((eigenLength / 2.0).getValue(), 3.0);
  EXPECT_EQ((blitzLength / 2.0).getValue(), 3.0);
}

TEST(TestValueMatrixBackends, Tensors) {  // NOLINT
  const EigenPosition eigenPosition{1.0, 2.0, 2.0};
  const BlitzPosition blitzPosition{1.0, 2.0, 2.0};

  // Elements and norms are quantities of the backend of the tensor.
  static_assert(std::is_same_v<decltype(eigenPosition.getQuantities().get<0, 0>()), wuzp_eigen::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(blitzPosition.getQuantities().get<0, 0>()), wuzp_blitz::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(eigenPosition.norm()), wuzp_eigen::QuantityD<Length>>);
  static_assert(std::is_same_v<decltype(blitzPosition.norm()), wuzp_blitz::QuantityD<Length>>);
  EXPECT_EQ(eigenPosition.norm().getValue(), 3.0);
  EXPECT_EQ(blitzPosition.norm().getValue(), 3.0);

  static_assert(std::is_same_v<decltype(eigenPosition * 2.0), EigenPosition>);
  static_assert(std::is_same_v<decltype(blitzPosition * 2.0), BlitzPosition>);
  static_assert(std::is_same_v<decltype(2.0 * eigenPosition), EigenPosition>);
  static_assert(std::is_same_v<decltype(2.0 * blitzPosition), BlitzPosition>);
  static_assert(std::is_same_v<decltype(eigenPosition / 2.0), EigenPosition>);
  static_assert(std::is_same_v<decltype(blitzPosition / 2.0), BlitzPosition>);
  EXPECT_EQ(eigenPosition * 2.0, (EigenPosition{2.0, 4.0, 4.0}));
  EXPECT_EQ(blitzPosition * 2.0, (BlitzPosition{2.0, 4.0, 4.0}));
  EXPECT_EQ(2.0 * eigenPosition, (EigenPosition{2.0, 4.0, 4.0}));
  EXPECT_EQ(2.0 * blitzPosition, (BlitzPosition{2.0, 4.0, 4.0}));
  EXPECT_EQ(eigenPosition / 2.0, (EigenPosition{0.5, 1.0, 1.0}));
  EXPECT_EQ(blitzPosition / 2.0, (BlitzPosition{0.5, 1.0, 1.0}));
  EXPECT_EQ(eigenPosition + eigenPosition, eigenPosition * 2.0);
  EXPECT_EQ(blitzPosition + blitzPosition, blitzPosition * 2.0);
}

// Each backend accesses the elements of its matrices with its Coefficient trait.
TEST(TestValueMatrixBackends, ElementAccess) {  // NOLINT
  EigenPosition eigenPosition{1.0, 2.0, 3.0};
  BlitzPosition blitzPosition{1.0, 2.0, 3.0};

  eigenPosition.setQuantity<1, 0>(wuzp_eigen::QuantityD<Length>{5.0});
  blitzPosition.setQuantity<1, 0>(wuzp_blitz::QuantityD<Length>{5.0});
  eigenPosition.setQuantity(2, 0, wuzp_eigen::QuantityD<Length>{6.0});
  blitzPosition.setQuantity(2, 0, wuzp_blitz::QuantityD<Length>{6.0});
  EXPECT_EQ(eigenPosition, (EigenPosition{1.0, 5.0, 6.0}));
  EXPECT_EQ(blitzPosition, (BlitzPosition{1.0, 5.0, 6.0}));

  EXPECT_EQ((eigenPosition.getQuantity<1, 0>().getValue()), 5.0);
  EXPECT_EQ((blitzPosition.getQuantity<1, 0>().getValue()), 5.0);
  EXPECT_EQ(eigenPosition.getQuantity(2, 0).getValue(), 6.0);
  EXPECT_EQ(blitzPosition.getQuantity(2, 0).getValue(), 6.0);
}

namespace {

// The values of a matrix of each backend, given row by row.
template <template <typename, int, int> typename ValueMatrixT_, int DimensionV_>
ValueMatrixT_<double, DimensionV_, DimensionV_> makeMatrix(
    const std::array<double, static_cast<std::size_t>(DimensionV_* DimensionV_)>& elements) {
  auto matrix = ValueMatrixT_<double, DimensionV_, DimensionV_>::zero();
  std::size_t index = 0;
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = 0; col < DimensionV_; ++col) {
      matrix.set(row, col, elements[index++]);
    }
  }
  return matrix;
}

// Whether the inverses and the determinants of Blitz++, computed by Gauss-Jordan elimination with partial pivoting, match those
// of Eigen, relative to their size (see Comparison::approxEqual()), and whether the inverses invert the matrix.
template <int DimensionV_>
void expectInverseAndDeterminantLikeEigen(const std::array<double, static_cast<std::size_t>(DimensionV_* DimensionV_)>& elements,
                                          const double precision) {
  const auto eigenMatrix = makeMatrix<wuzp_eigen::ValueMatrix, DimensionV_>(elements);
  const auto blitzMatrix = makeMatrix<wuzp_blitz::ValueMatrix, DimensionV_>(elements);
  const auto eigenInverse = eigenMatrix.inverse();
  const auto blitzInverse = blitzMatrix.inverse();
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = 0; col < DimensionV_; ++col) {
      EXPECT_NEAR(blitzInverse.get(row, col), eigenInverse.get(row, col), precision * eigenInverse.norm()) << row << ", " << col;
    }
  }
  EXPECT_NEAR(blitzMatrix.determinant(), eigenMatrix.determinant(), precision * std::abs(eigenMatrix.determinant()));
  // The residual of A A^-1 is of the order of the rounding error times ||A|| ||A^-1||.
  const auto product = blitzMatrix.innerProduct(blitzInverse);
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = 0; col < DimensionV_; ++col) {
      EXPECT_NEAR(product.get(row, col), row == col ? 1.0 : 0.0, precision * blitzMatrix.norm() * blitzInverse.norm())
          << row << ", " << col;
    }
  }
}

// Whether the inverse and the determinant of a matrix of each backend and of the archetype, given row by row, are those computed
// by hand, exactly with fractions, up to rounding.
template <int DimensionV_>
void expectInverseAndDeterminant(const std::array<double, static_cast<std::size_t>(DimensionV_* DimensionV_)>& elements,
                                 const std::array<double, static_cast<std::size_t>(DimensionV_* DimensionV_)>& inverse,
                                 const double determinant) {
  const auto eigenMatrix = makeMatrix<wuzp_eigen::ValueMatrix, DimensionV_>(elements);
  const auto blitzMatrix = makeMatrix<wuzp_blitz::ValueMatrix, DimensionV_>(elements);
  const auto archetypeMatrix = makeMatrix<archetype_values::ValueMatrix, DimensionV_>(elements);
  EXPECT_TRUE(eigenMatrix.inverse().isApprox(makeMatrix<wuzp_eigen::ValueMatrix, DimensionV_>(inverse), 1e-14));
  EXPECT_TRUE(blitzMatrix.inverse().isApprox(makeMatrix<wuzp_blitz::ValueMatrix, DimensionV_>(inverse), 1e-14));
  EXPECT_TRUE(archetypeMatrix.inverse().isApprox(makeMatrix<archetype_values::ValueMatrix, DimensionV_>(inverse), 1e-14));
  EXPECT_NEAR(eigenMatrix.determinant(), determinant, 1e-14 * std::abs(determinant));
  EXPECT_NEAR(blitzMatrix.determinant(), determinant, 1e-14 * std::abs(determinant));
  EXPECT_NEAR(archetypeMatrix.determinant(), determinant, 1e-14 * std::abs(determinant));
}

}  // namespace

// Well-conditioned matrices of the sizes 1 to 4, also with a zero on the diagonal, which needs a row swap: the inverses and the
// determinants of both backends agree to rounding.
TEST(TestValueMatrixBackends, InverseAndDeterminantOfWellConditionedMatrices) {  // NOLINT
  expectInverseAndDeterminantLikeEigen<1>({4.0}, 1e-15);
  expectInverseAndDeterminantLikeEigen<2>({4.0, 7.0, 2.0, 6.0}, 1e-14);
  expectInverseAndDeterminantLikeEigen<2>({0.0, 1.0, 1.0, 0.0}, 1e-15);
  expectInverseAndDeterminantLikeEigen<3>({2.0, -1.0, 0.0, -1.0, 2.0, -1.0, 0.0, -1.0, 2.0}, 1e-14);
  expectInverseAndDeterminantLikeEigen<3>({0.0, 2.0, 1.0, 1.0, 0.0, 3.0, 4.0, -2.0, 0.0}, 1e-14);
  expectInverseAndDeterminantLikeEigen<4>({4.0, 1.0, 0.0, 2.0, 1.0, 5.0, 1.0, 0.0, 0.0, 1.0, 6.0, 1.0, 2.0, 0.0, 1.0, 7.0}, 1e-14);
}

// The inverses and the determinants computed by hand, of matrices of the sizes 1 to 4, also with zeros on the diagonal, which need
// row swaps, each of which negates the determinant.
TEST(TestValueMatrixBackends, InverseAndDeterminantComputedByHand) {  // NOLINT
  expectInverseAndDeterminant<1>({4.0}, {0.25}, 4.0);
  // [[a, b], [c, d]]^-1 = [[d, -b], [-c, a]] / (ad - bc), with ad - bc = 4 * 6 - 7 * 2 = 10.
  expectInverseAndDeterminant<2>({4.0, 7.0, 2.0, 6.0}, {0.6, -0.7, -0.2, 0.4}, 10.0);
  // A permutation, which is its own inverse. The elimination swaps the rows once.
  expectInverseAndDeterminant<2>({0.0, 1.0, 1.0, 0.0}, {0.0, 1.0, 1.0, 0.0}, -1.0);
  // The matrix of second differences, whose inverse is min(i, j) (4 - max(i, j)) / 4, with the indices from 1.
  expectInverseAndDeterminant<3>({2.0, -1.0, 0.0, -1.0, 2.0, -1.0, 0.0, -1.0, 2.0}, {0.75, 0.5, 0.25, 0.5, 1.0, 0.5, 0.25, 0.5, 0.75}, 4.0);
  // The adjugate divided by the determinant, 0 * (0 * 0 - 3 * -2) - 2 * (1 * 0 - 3 * 4) + 1 * (1 * -2 - 0 * 4) = 22.
  expectInverseAndDeterminant<3>(
      {0.0, 2.0, 1.0, 1.0, 0.0, 3.0, 4.0, -2.0, 0.0},
      {6.0 / 22.0, -2.0 / 22.0, 6.0 / 22.0, 12.0 / 22.0, -4.0 / 22.0, 1.0 / 22.0, -2.0 / 22.0, 8.0 / 22.0, -2.0 / 22.0}, 22.0);
  expectInverseAndDeterminant<4>(
      {4.0, 1.0, 0.0, 2.0, 1.0, 5.0, 1.0, 0.0, 0.0, 1.0, 6.0, 1.0, 2.0, 0.0, 1.0, 7.0},
      {198.0 / 631.0, -43.0 / 631.0, 17.0 / 631.0, -59.0 / 631.0, -43.0 / 631.0, 140.0 / 631.0, -26.0 / 631.0, 16.0 / 631.0, 17.0 / 631.0,
       -26.0 / 631.0, 113.0 / 631.0, -21.0 / 631.0, -59.0 / 631.0, 16.0 / 631.0, -21.0 / 631.0, 110.0 / 631.0},
      631.0);
  // Zeros on the whole diagonal.
  expectInverseAndDeterminant<4>(
      {0.0, 1.0, 2.0, 0.0, 1.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 1.0, 0.0, 2.0, 1.0, 0.0},
      {0.0, -1.0 / 3.0, 2.0 / 3.0, 0.0, -1.0 / 3.0, 0.0, 0.0, 2.0 / 3.0, 2.0 / 3.0, 0.0, 0.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, -1.0 / 3.0, 0.0},
      9.0);
}

// Badly conditioned matrices: the Hilbert matrix of size 4, whose condition number is about 1.6e4, and a matrix that is nearly
// singular, with the condition number 4e10. The relative errors of both backends grow with the condition number.
TEST(TestValueMatrixBackends, InverseAndDeterminantOfBadlyConditionedMatrices) {  // NOLINT
  expectInverseAndDeterminantLikeEigen<4>({1.0, 1.0 / 2.0, 1.0 / 3.0, 1.0 / 4.0, 1.0 / 2.0, 1.0 / 3.0, 1.0 / 4.0, 1.0 / 5.0, 1.0 / 3.0,
                                           1.0 / 4.0, 1.0 / 5.0, 1.0 / 6.0, 1.0 / 4.0, 1.0 / 5.0, 1.0 / 6.0, 1.0 / 7.0},
                                          1e-10);
  expectInverseAndDeterminantLikeEigen<2>({1.0, 1.0, 1.0, 1.0 + 1e-10}, 1e-5);
}

// The inverse of a singular matrix, here with two equal rows, has non-finite elements with both backends, and the determinant is
// zero: Gauss-Jordan elimination finds a zero pivot, and the inverse of Blitz++ is NaN.
TEST(TestValueMatrixBackends, InverseAndDeterminantOfSingularMatrix) {  // NOLINT
  const std::array<double, 9> elements{1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  const auto eigenMatrix = makeMatrix<wuzp_eigen::ValueMatrix, 3>(elements);
  const auto blitzMatrix = makeMatrix<wuzp_blitz::ValueMatrix, 3>(elements);
  EXPECT_EQ(eigenMatrix.determinant(), 0.0);
  EXPECT_EQ(blitzMatrix.determinant(), 0.0);
  const auto eigenInverse = eigenMatrix.inverse();
  const auto blitzInverse = blitzMatrix.inverse();
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      EXPECT_FALSE(std::isfinite(eigenInverse.get(row, col)));
      EXPECT_TRUE(std::isnan(blitzInverse.get(row, col)));
    }
  }
}

// The determinant of an integer matrix is computed in floating point and rounded to the nearest integer, and with the archetype
// exactly, by Laplace expansion.
TEST(TestValueMatrixBackends, DeterminantOfIntegerMatrix) {  // NOLINT
  const wuzp_eigen::ValueMatrix<int, 3, 3> eigenMatrix{2, 3, 1, 3, 5, 2, 1, 4, 7};
  const wuzp_blitz::ValueMatrix<int, 3, 3> blitzMatrix{2, 3, 1, 3, 5, 2, 1, 4, 7};
  const archetype_values::ValueMatrix<int, 3, 3> archetypeMatrix{2, 3, 1, 3, 5, 2, 1, 4, 7};
  EXPECT_EQ(eigenMatrix.determinant(), 4);
  EXPECT_EQ(blitzMatrix.determinant(), 4);
  EXPECT_EQ(archetypeMatrix.determinant(), 4);
  // With a row swap, and with a negative determinant.
  EXPECT_EQ((wuzp_eigen::ValueMatrix<int, 3, 3>{0, 2, 1, 1, 0, 3, 4, -2, 0}.determinant()), 22);
  EXPECT_EQ((wuzp_blitz::ValueMatrix<int, 3, 3>{0, 2, 1, 1, 0, 3, 4, -2, 0}.determinant()), 22);
  EXPECT_EQ((archetype_values::ValueMatrix<int, 3, 3>{0, 2, 1, 1, 0, 3, 4, -2, 0}.determinant()), 22);
  EXPECT_EQ((wuzp_eigen::ValueMatrix<int, 4, 4>{0, 1, 0, 2, 1, 0, 1, 0, 0, 2, 0, 1, 1, 0, 2, 0}.determinant()), -3);
  EXPECT_EQ((wuzp_blitz::ValueMatrix<int, 4, 4>{0, 1, 0, 2, 1, 0, 1, 0, 0, 2, 0, 1, 1, 0, 2, 0}.determinant()), -3);
  EXPECT_EQ((archetype_values::ValueMatrix<int, 4, 4>{0, 1, 0, 2, 1, 0, 1, 0, 0, 2, 0, 1, 1, 0, 2, 0}.determinant()), -3);
  // A singular matrix, with two equal rows.
  EXPECT_EQ((wuzp_eigen::ValueMatrix<int, 3, 3>{1, 2, 3, 1, 2, 3, 4, 5, 6}.determinant()), 0);
  EXPECT_EQ((wuzp_blitz::ValueMatrix<int, 3, 3>{1, 2, 3, 1, 2, 3, 4, 5, 6}.determinant()), 0);
  EXPECT_EQ((archetype_values::ValueMatrix<int, 3, 3>{1, 2, 3, 1, 2, 3, 4, 5, 6}.determinant()), 0);
}
