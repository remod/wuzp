// Both value matrix backends in one translation unit. Their trait specializations are constrained to their own matrices, so
// they can be used side by side, and operations derive scalars from the backend of their operands (see "The backend
// interface" in DESIGN.md). The other tests run against the backend of the configuration (ValueMatrix.hpp).

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

#include "coordinate_systems.hpp"

#include "Unit.hpp"

// The backends implement the interface of the core. They do not check this in their headers, since it would cost compile
// time in every translation unit of their users (see ValueMatrixBackend). Blitz++ provides no inverses yet (see "Backend
// support for inverses" in DESIGN.md).
static_assert(wuzp::ValueMatrixBackend<wuzp_eigen::ValueMatrix>);
static_assert(wuzp::InvertibleValueMatrixBackend<wuzp_eigen::ValueMatrix>);
static_assert(wuzp::ValueMatrixBackend<wuzp_blitz::ValueMatrix>);

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
