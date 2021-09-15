#pragma once

#include <ostream>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrixTraits.hpp"

namespace wuzp {

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
class CoordinateMatrix {
 public:
  static_assert(ValueMatrixTraits::Rows<ValueMatrix>::value == Rows<UnitMatrix>::value &&
                    ValueMatrixTraits::Rows<ValueMatrix>::value == Rows<CoordinateSystemMatrix>::value,
                "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<ValueMatrix>::value == Cols<UnitMatrix>::value &&
                    ValueMatrixTraits::Cols<ValueMatrix>::value == Cols<CoordinateSystemMatrix>::value,
                "Number of columns do not match.");

  CoordinateMatrix() = default;
  template <typename... Ts>
  explicit CoordinateMatrix(Ts... ts) : quantityMatrix_(ts...) {}
  explicit CoordinateMatrix(const ValueMatrix& values) : quantityMatrix_(values) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValueMatrixOther>
  explicit CoordinateMatrix(const QuantityMatrix<ValueMatrixOther, UnitMatrix>& quantityMatrix) : quantityMatrix_(quantityMatrix) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValueMatrixOther>
  CoordinateMatrix(const CoordinateMatrix<ValueMatrixOther, UnitMatrix, CoordinateSystemMatrix>& other)
      : quantityMatrix_(other.getQuantityMatrix()) {}

  const QuantityMatrix<ValueMatrix, UnitMatrix>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValueMatrix(const ValueMatrix& values) { quantityMatrix_.setValueMatrix(values); }
  const ValueMatrix& getValueMatrix() const { return quantityMatrix_.getValueMatrix(); }

  template <typename ValueMatrixOther, typename UnitMatrixOther, typename CoordinateSystemMatrixOther>
  bool isApprox(const CoordinateMatrix<ValueMatrixOther, UnitMatrixOther, CoordinateSystemMatrixOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<CoordinateSystemMatrix, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(getValueMatrix().transpose()); }

  template <typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
  auto dot(const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& other) const {
    return QuantityTraits::
        Create<typename DotProduct<This, CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type>::create(
            getValueMatrix().dot(other.getValueMatrix()));
  }

 private:
  using This = CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>;

  QuantityMatrix<ValueMatrix, UnitMatrix> quantityMatrix_;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Sum<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
           CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Sum<ValueMatrixLhs, ValueMatrixRhs>::type,
                                typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type,
                                typename Sum<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Difference<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                  CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Difference<ValueMatrixLhs, ValueMatrixRhs>::type,
                                typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type,
                                typename Difference<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueRhs>::type,
                                typename Product<UnitMatrixLhs, UnitRhs>::type, CoordinateSystemMatrixLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type =
      typename Product<CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
// Use case: Rotate a vector.
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
               CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
 public:
  using type = QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;

 private:
  using CoordinateSystemMatrixProduct = typename Product<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type;
  static_assert(IsHomogeneous<CoordinateSystemMatrixProduct>::value &&
                    std::is_same<typename Element<CoordinateSystemMatrixProduct, 0, 0>::type, NoCoordinateSystem>::value,
                "Coordinate systems do not match.");
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Division<ValueMatrixLhs, ValueRhs>::type,
                                typename Division<UnitMatrixLhs, UnitRhs>::type, CoordinateSystemMatrixLhs>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct Transpose<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type,
                                typename Transpose<CoordinateSystemMatrix>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct DotProduct<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                  CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = Quantity<typename ValueMatrixTraits::template DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type,
                        typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
  using assert_frames_match = typename DotProduct<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix, int BlockRowId, int BlockColId, int BlockRows,
          int BlockCols>
struct Block<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Block<ValueMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<CoordinateSystemMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct GetCoordinateSystem<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  static_assert(IsHomogeneous<CoordinateSystemMatrix>::value, "Coordinate systems are heterogeneous.");
  using type = std::tuple_element<0, typename AsTuple<CoordinateSystemMatrix>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
auto operator+(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& framedQuantityMatrix) {
  return framedQuantityMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator+(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Sum<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                      CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getValueMatrix() +
                                                                                                        rhs.getValueMatrix());
}

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
auto operator-(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& framedQuantityMatrix) {
  return CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>(-framedQuantityMatrix.getValueMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator-(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Difference<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                             CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getValueMatrix() -
                                                                                                               rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const ValueLhs lhs, const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() * QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                          CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getValueMatrix() *
                                                                                                            rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs>
auto operator/(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() / QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
bool operator==(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
                const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  static_assert(std::is_same<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::value, "Coordinate systems do not match.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
bool operator!=(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
                const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
std::ostream& operator<<(std::ostream& stream,
                         const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& framedQuantityMatrix) {
  stream << framedQuantityMatrix.getQuantityMatrix();
  return stream;
}

template <int Dimension, typename Scalar, typename CoordinateSystem>
using PositionPoint = CoordinateMatrix<ValueMatrix<Scalar, Dimension, 1>, UnitMatrix<Dimension, 1, Absolute<Length>>,
                                       CoordinateSystemMatrix<Dimension, 1, CoordinateSystem>>;
template <typename Scalar, typename CoordinateSystem>
using PositionPoint2D = PositionPoint<2, Scalar, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using PositionPoint3D = PositionPoint<3, Scalar, CoordinateSystem>;

template <int Dimension, typename Scalar, typename CoordinateSystem>
using PositionVector = CoordinateMatrix<ValueMatrix<Scalar, Dimension, 1>, UnitMatrix<Dimension, 1, Length>,
                                        CoordinateSystemMatrix<Dimension, 1, CoordinateSystem>>;
template <typename Scalar, typename CoordinateSystem>
using PositionVector2D = PositionVector<2, Scalar, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using PositionVector3D = PositionVector<3, Scalar, CoordinateSystem>;

template <int Dimension, typename Scalar, typename CoordinateSystem>
using VelocityVector = CoordinateMatrix<Eigen::Matrix<Scalar, Dimension, 1>, UnitMatrix<Dimension, 1, Velocity>,
                                        CoordinateSystemMatrix<Dimension, 1, CoordinateSystem>>;
template <typename Scalar, typename CoordinateSystem>
using Velocity2D = VelocityVector<2, Scalar, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using Velocity3D = VelocityVector<3, Scalar, CoordinateSystem>;

}  // namespace wuzp
