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

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems>
class CoordinateMatrix {
 public:
  static_assert(ValueMatrixTraits::Rows<ValueMatrix>::value == Rows<UnitMatrix>::value &&
                    ValueMatrixTraits::Rows<ValueMatrix>::value == Rows<CoordinateSystems>::value,
                "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<ValueMatrix>::value == Cols<UnitMatrix>::value &&
                    ValueMatrixTraits::Cols<ValueMatrix>::value == Cols<CoordinateSystems>::value,
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
  CoordinateMatrix(const CoordinateMatrix<ValueMatrixOther, UnitMatrix, CoordinateSystems>& other)
      : quantityMatrix_(other.getQuantityMatrix()) {}

  const QuantityMatrix<ValueMatrix, UnitMatrix>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValueMatrix(const ValueMatrix& values) { quantityMatrix_.setValueMatrix(values); }
  const ValueMatrix& getValueMatrix() const { return quantityMatrix_.getValueMatrix(); }

  template <typename ValueMatrixOther, typename UnitMatrixOther, typename CoordinateSystemsOther>
  bool isApprox(const CoordinateMatrix<ValueMatrixOther, UnitMatrixOther, CoordinateSystemsOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<CoordinateSystems, CoordinateSystemsOther>::value, "Coordinate systems do not match.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(getValueMatrix().transpose()); }

  template <typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemsRhs>
  auto dot(const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& other) const {
    return QuantityTraits::Create<typename DotProduct<This, CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>>::type>::
        create(getValueMatrix().dot(other.getValueMatrix()));
  }

 private:
  using This = CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>;

  QuantityMatrix<ValueMatrix, UnitMatrix> quantityMatrix_;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
struct Sum<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
           CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>> {
  using type =
      CoordinateMatrix<typename ValueMatrixTraits::template Sum<ValueMatrixLhs, ValueMatrixRhs>::type,
                       typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type, typename Sum<CoordinateSystemsLhs, CoordinateSystemsRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
struct Difference<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
                  CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Difference<ValueMatrixLhs, ValueMatrixRhs>::type,
                                typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type,
                                typename Difference<CoordinateSystemsLhs, CoordinateSystemsRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
struct Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueRhs>::type,
                                typename Product<UnitMatrixLhs, UnitRhs>::type, CoordinateSystemsLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemsRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>> {
  using type = typename Product<CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
// Use case: Rotate a vector.
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
struct Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
               CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>> {
 public:
  using type = QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;

 private:
  using CoordinateSystemProduct = typename Product<CoordinateSystemsLhs, CoordinateSystemsRhs>::type;
  static_assert(IsHomogeneous<CoordinateSystemProduct>::value &&
                    std::is_same<typename Element<CoordinateSystemProduct, 0, 0>::type, NoCoordinateSystem>::value,
                "Coordinate systems do not match.");
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
struct Division<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Division<ValueMatrixLhs, ValueRhs>::type,
                                typename Division<UnitMatrixLhs, UnitRhs>::type, CoordinateSystemsLhs>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems>
struct Transpose<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type,
                                typename Transpose<CoordinateSystems>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
struct DotProduct<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
                  CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>> {
  using type = Quantity<typename ValueMatrixTraits::template DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type,
                        typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
  using assert_frames_match = typename DotProduct<CoordinateSystemsLhs, CoordinateSystemsRhs>::type;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems, int BlockRowId, int BlockColId, int BlockRows,
          int BlockCols>
struct Block<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Block<ValueMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<CoordinateSystems, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems>
struct GetCoordinateSystem<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>> {
  static_assert(IsHomogeneous<CoordinateSystems>::value, "Coordinate systems are heterogeneous.");
  using type = std::tuple_element<0, typename AsTuple<CoordinateSystems>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems>
auto operator+(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>& framedQuantityMatrix) {
  return framedQuantityMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
auto operator+(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  return typename Sum<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
                      CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>>::type(lhs.getValueMatrix() +
                                                                                                   rhs.getValueMatrix());
}

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems>
auto operator-(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>& framedQuantityMatrix) {
  return CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>(-framedQuantityMatrix.getValueMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
auto operator-(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  return typename Difference<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
                             CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>>::type(lhs.getValueMatrix() -
                                                                                                          rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemsRhs>
auto operator*(const ValueLhs lhs, const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() * QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemsRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>,
                          CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>>::type(lhs.getValueMatrix() *
                                                                                                       rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueRhs>
auto operator/(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() / QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
bool operator==(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs,
                const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  static_assert(std::is_same<CoordinateSystemsLhs, CoordinateSystemsRhs>::value, "Coordinate systems do not match.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemsLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          typename CoordinateSystemsRhs>
bool operator!=(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemsLhs>& lhs,
                const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemsRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystems>
std::ostream& operator<<(std::ostream& stream, const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystems>& framedQuantityMatrix) {
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
