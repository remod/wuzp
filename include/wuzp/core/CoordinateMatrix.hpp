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

template <typename Values, typename Units, typename CoordinateSystems>
class CoordinateMatrix {
 public:
  static_assert(ValueMatrixTraits::Rows<Values>::value == Rows<Units>::value &&
                    ValueMatrixTraits::Rows<Values>::value == Rows<CoordinateSystems>::value,
                "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<Values>::value == Cols<Units>::value &&
                    ValueMatrixTraits::Cols<Values>::value == Cols<CoordinateSystems>::value,
                "Number of columns do not match.");

  CoordinateMatrix() = default;
  template <typename... Ts>
  explicit CoordinateMatrix(Ts... ts) : quantityMatrix_(ts...) {}
  explicit CoordinateMatrix(const Values& values) : quantityMatrix_(values) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValuesOther>
  explicit CoordinateMatrix(const QuantityMatrix<ValuesOther, Units>& quantityMatrix) : quantityMatrix_(quantityMatrix) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValuesOther>
  CoordinateMatrix(const CoordinateMatrix<ValuesOther, Units, CoordinateSystems>& other) : quantityMatrix_(other.getQuantityMatrix()) {}

  const QuantityMatrix<Values, Units>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValues(const Values& values) { quantityMatrix_.setValues(values); }
  const Values& getValues() const { return quantityMatrix_.getValues(); }

  template <typename ValuesOther, typename UnitsOther, typename CoordinateSystemsOther>
  bool isApprox(const CoordinateMatrix<ValuesOther, UnitsOther, CoordinateSystemsOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<CoordinateSystems, CoordinateSystemsOther>::value, "Coordinate systems mismatch.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(getValues().transpose()); }

  template <typename ValuesRhs, typename UnitsRhs, typename CoordinateSystemsRhs>
  auto dot(const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& other) const {
    return QuantityTraits::Create<typename DotProduct<This, CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>>::type>::create(
        getValues().dot(other.getValues()));
  }

 private:
  using This = CoordinateMatrix<Values, Units, CoordinateSystems>;

  QuantityMatrix<Values, Units> quantityMatrix_;
};

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
struct Sum<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>, CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Sum<ValuesLhs, ValuesRhs>::type,
                                typename Sum<UnitsLhs, UnitsRhs>::type, typename Sum<CoordinateSystemsLhs, CoordinateSystemsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
struct Difference<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>,
                  CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Difference<ValuesLhs, ValuesRhs>::type,
                                typename Difference<UnitsLhs, UnitsRhs>::type,
                                typename Difference<CoordinateSystemsLhs, CoordinateSystemsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
struct Product<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Product<ValuesLhs, ValueRhs>::type,
                                typename Product<UnitsLhs, UnitRhs>::type, CoordinateSystemsLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename ValuesRhs, typename UnitsRhs, typename CoordinateSystemsRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>> {
  using type = typename Product<CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
// Use case: Rotate a vector.
template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
struct Product<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>, CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>> {
 public:
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Product<ValuesLhs, ValuesRhs>::type, typename Product<UnitsLhs, UnitsRhs>::type>;

 private:
  using CoordinateSystemProduct = typename Product<CoordinateSystemsLhs, CoordinateSystemsRhs>::type;
  static_assert(IsHomogeneous<CoordinateSystemProduct>::value &&
                    std::is_same<typename Element<CoordinateSystemProduct, 0, 0>::type, NoCoordinateSystem>::value,
                "Coordinate system mismatch.");
};

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
struct Division<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Division<ValuesLhs, ValueRhs>::type,
                                typename Division<UnitsLhs, UnitRhs>::type, CoordinateSystemsLhs>;
};

template <typename Values, typename Units, typename CoordinateSystems>
struct Transpose<CoordinateMatrix<Values, Units, CoordinateSystems>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Transpose<Values>::type, typename Transpose<Units>::type,
                                typename Transpose<CoordinateSystems>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
struct DotProduct<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>,
                  CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>> {
  using type =
      Quantity<typename ValueMatrixTraits::template DotProduct<ValuesLhs, ValuesRhs>::type, typename DotProduct<UnitsLhs, UnitsRhs>::type>;
  using assert_frames_match = typename DotProduct<CoordinateSystemsLhs, CoordinateSystemsRhs>::type;
};

template <typename Values, typename Units, typename CoordinateSystems, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<CoordinateMatrix<Values, Units, CoordinateSystems>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Block<Values, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<Units, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<CoordinateSystems, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename Values, typename Units, typename CoordinateSystems>
struct GetCoordinateSystem<CoordinateMatrix<Values, Units, CoordinateSystems>> {
  static_assert(IsHomogeneous<CoordinateSystems>::value, "Coordinate systems are heterogeneous.");
  using type = std::tuple_element<0, typename AsTuple<CoordinateSystems>::type>;
};

template <typename Values, typename Units, typename CoordinateSystems>
auto operator+(const CoordinateMatrix<Values, Units, CoordinateSystems>& framedQuantityMatrix) {
  return framedQuantityMatrix;
}
template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
auto operator+(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs,
               const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  return typename Sum<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>,
                      CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>>::type(lhs.getValues() + rhs.getValues());
}

template <typename Values, typename Units, typename CoordinateSystems>
auto operator-(const CoordinateMatrix<Values, Units, CoordinateSystems>& framedQuantityMatrix) {
  return CoordinateMatrix<Values, Units, CoordinateSystems>(-framedQuantityMatrix.getValues());
}
template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
auto operator-(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs,
               const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  return typename Difference<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>,
                             CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>>::type(lhs.getValues() - rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValueRhs>
auto operator*(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValuesRhs, typename UnitsRhs, typename CoordinateSystemsRhs>
auto operator*(const ValueLhs lhs, const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValues() * QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}
template <typename ValueLhs, typename UnitLhs, typename ValuesRhs, typename UnitsRhs, typename CoordinateSystemsRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
auto operator*(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs,
               const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>,
                          CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>>::type(lhs.getValues() * rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValueRhs>
auto operator/(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValues() / QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
bool operator==(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs,
                const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  static_assert(std::is_same<CoordinateSystemsLhs, CoordinateSystemsRhs>::value, "Coordinate systems mismatch.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename ValuesLhs, typename UnitsLhs, typename CoordinateSystemsLhs, typename ValuesRhs, typename UnitsRhs,
          typename CoordinateSystemsRhs>
bool operator!=(const CoordinateMatrix<ValuesLhs, UnitsLhs, CoordinateSystemsLhs>& lhs,
                const CoordinateMatrix<ValuesRhs, UnitsRhs, CoordinateSystemsRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename Values, typename Units, typename CoordinateSystems>
std::ostream& operator<<(std::ostream& stream, const CoordinateMatrix<Values, Units, CoordinateSystems>& framedQuantityMatrix) {
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
