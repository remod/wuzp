#pragma once

#include <ostream>

#include "ctm/core/Frame.hpp"
#include "ctm/core/QuantityMatrix.hpp"
#include "ctm/units/QuantityTraits.hpp"
#include "ctm/units/Unit.hpp"
#include "ctm/units/UnitMatrix.hpp"
#include "ctm/value_matrix/ValueMatrix.hpp"
#include "ctm/value_matrix/ValueMatrixTraits.hpp"

namespace ctm {

template <typename Values, typename Units, typename Frames>
class FramedQuantityMatrix {
 public:
  static_assert(value_matrix::ValueMatrixTraits::Rows<Values>::value == Rows<Units>::value &&
                    value_matrix::ValueMatrixTraits::Rows<Values>::value == Rows<Frames>::value,
                "Number of rows do not match.");
  static_assert(value_matrix::ValueMatrixTraits::Cols<Values>::value == Cols<Units>::value &&
                    value_matrix::ValueMatrixTraits::Cols<Values>::value == Cols<Frames>::value,
                "Number of columns do not match.");

  FramedQuantityMatrix() = default;
  explicit FramedQuantityMatrix(const Values& values) : quantityMatrix_(values) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValuesOther>
  explicit FramedQuantityMatrix(const QuantityMatrix<ValuesOther, Units>& quantityMatrix) : quantityMatrix_(quantityMatrix) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValuesOther>
  FramedQuantityMatrix(const FramedQuantityMatrix<ValuesOther, Units, Frames>& other) : quantityMatrix_(other.getQuantityMatrix()) {}

  const QuantityMatrix<Values, Units>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValues(const Values& values) { quantityMatrix_.setValues(values); }
  const Values& getValues() const { return quantityMatrix_.getValues(); }

  template <typename ValuesOther, typename UnitsOther, typename FramesOther>
  bool isApprox(const FramedQuantityMatrix<ValuesOther, UnitsOther, FramesOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<Frames, FramesOther>::value, "Frames mismatch.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(getValues().transpose()); }

  template <typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
  auto dot(const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& other) const {
    return units::QuantityTraits::Create<typename DotProduct<This, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type>::create(
        getValues().dot(other.getValues()));
  }

 private:
  using This = FramedQuantityMatrix<Values, Units, Frames>;

  QuantityMatrix<Values, Units> quantityMatrix_;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct Sum<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  using type = FramedQuantityMatrix<typename value_matrix::ValueMatrixTraits::template Sum<ValuesLhs, ValuesRhs>::type,
                                    typename Sum<UnitsLhs, UnitsRhs>::type, typename Sum<FramesLhs, FramesRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct Difference<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  using type = FramedQuantityMatrix<typename value_matrix::ValueMatrixTraits::template Difference<ValuesLhs, ValuesRhs>::type,
                                    typename Difference<UnitsLhs, UnitsRhs>::type, typename Difference<FramesLhs, FramesRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValueRhs, typename UnitRhs>
struct Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, units::Quantity<ValueRhs, UnitRhs>> {
  using type = FramedQuantityMatrix<typename value_matrix::ValueMatrixTraits::template Product<ValuesLhs, ValueRhs>::type,
                                    typename Product<UnitsLhs, UnitRhs>::type, FramesLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct Product<units::Quantity<ValueLhs, UnitLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  using type = typename Product<FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>, units::Quantity<ValueLhs, UnitLhs>>::type;
};
// Use case: Rotate a vector.
template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
 public:
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Product<ValuesLhs, ValuesRhs>::type,
                              typename Product<UnitsLhs, UnitsRhs>::type>;

 private:
  using FrameProduct = typename Product<FramesLhs, FramesRhs>::type;
  static_assert(IsHomogeneous<FrameProduct>::value && std::is_same<typename Element<FrameProduct, 0, 0>::type, NoFrame>::value,
                "Frame mismatch.");
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValueRhs, typename UnitRhs>
struct Division<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, units::Quantity<ValueRhs, UnitRhs>> {
  using type = FramedQuantityMatrix<typename value_matrix::ValueMatrixTraits::template Division<ValuesLhs, ValueRhs>::type,
                                    typename Division<UnitsLhs, UnitRhs>::type, FramesLhs>;
};

template <typename Values, typename Units, typename Frames>
struct Transpose<FramedQuantityMatrix<Values, Units, Frames>> {
  using type = FramedQuantityMatrix<typename value_matrix::ValueMatrixTraits::template Transpose<Values>::type,
                                    typename Transpose<Units>::type, typename Transpose<Frames>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct DotProduct<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  using type = units::Quantity<typename value_matrix::ValueMatrixTraits::template DotProduct<ValuesLhs, ValuesRhs>::type,
                               typename DotProduct<UnitsLhs, UnitsRhs>::type>;
  using assert_frames_match = typename DotProduct<FramesLhs, FramesRhs>::type;
};

template <typename Values, typename Units, typename Frames, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<FramedQuantityMatrix<Values, Units, Frames>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = FramedQuantityMatrix<
      typename value_matrix::ValueMatrixTraits::template Block<Values, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
      typename Block<Units, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
      typename Block<Frames, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename Values, typename Units, typename Frames>
struct GetFrame<FramedQuantityMatrix<Values, Units, Frames>> {
  static_assert(IsHomogeneous<Frames>::value, "Frames are heterogeneous.");
  using type = std::tuple_element<0, typename AsTuple<Frames>::type>;
};

template <typename Values, typename Units, typename Frames>
auto operator+(const FramedQuantityMatrix<Values, Units, Frames>& framedQuantityMatrix) {
  return framedQuantityMatrix;
}
template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator+(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return typename Sum<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
      lhs.getValues() + rhs.getValues());
}

template <typename Values, typename Units, typename Frames>
auto operator-(const FramedQuantityMatrix<Values, Units, Frames>& framedQuantityMatrix) {
  return FramedQuantityMatrix<Values, Units, Frames>(-framedQuantityMatrix.getValues());
}
template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator-(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return
      typename Difference<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
          lhs.getValues() - rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValueRhs>
auto operator*(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs, const ValueRhs rhs) {
  return lhs * units::Quantity<ValueRhs, units::Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator*(const ValueLhs lhs, const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs, const units::Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, units::Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValues() * units::QuantityTraits::GetValue<units::Quantity<ValueRhs, UnitRhs>>::get(rhs));
}
template <typename ValueLhs, typename UnitLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator*(const units::Quantity<ValueLhs, UnitLhs>& lhs, const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator*(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return typename Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
      lhs.getValues() * rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValueRhs>
auto operator/(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs, const ValueRhs rhs) {
  return lhs / units::Quantity<ValueRhs, units::Dimensionless>(rhs);
}
template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs, const units::Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, units::Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValues() / units::QuantityTraits::GetValue<units::Quantity<ValueRhs, UnitRhs>>::get(rhs));
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
bool operator==(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
                const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  static_assert(std::is_same<FramesLhs, FramesRhs>::value, "Frames mismatch.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
bool operator!=(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
                const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename Values, typename Units, typename Frames>
std::ostream& operator<<(std::ostream& stream, const FramedQuantityMatrix<Values, Units, Frames>& framedQuantityMatrix) {
  stream << framedQuantityMatrix.getQuantityMatrix();
  return stream;
}

template <int Dimension, typename Scalar, typename Frame>
using Position = FramedQuantityMatrix<value_matrix::ValueMatrix<Scalar, Dimension, 1>,
                                      units::UnitMatrix<Dimension, 1, units::Absolute<units::Length>>, FrameMatrix<Dimension, 1, Frame>>;

template <typename Scalar, typename Frame>
using Position2D = Position<2, Scalar, Frame>;
template <typename Scalar, typename Frame>
using Position3D = Position<3, Scalar, Frame>;

template <int Dimension, typename Scalar, typename Frame>
using Vector = FramedQuantityMatrix<value_matrix::ValueMatrix<Scalar, Dimension, 1>, units::UnitMatrix<Dimension, 1, units::Length>,
                                    FrameMatrix<Dimension, 1, Frame>>;

template <typename Scalar, typename Frame>
using Vector2D = Vector<2, Scalar, Frame>;
template <typename Scalar, typename Frame>
using Vector3D = Vector<3, Scalar, Frame>;

template <int Dimension, typename Scalar, typename Frame>
using Velocity = FramedQuantityMatrix<Eigen::Matrix<Scalar, Dimension, 1>, units::UnitMatrix<Dimension, 1, units::Velocity>,
                                      FrameMatrix<Dimension, 1, Frame>>;

template <typename Scalar, typename Frame>
using Velocity2D = Velocity<2, Scalar, Frame>;
template <typename Scalar, typename Frame>
using Velocity3D = Velocity<3, Scalar, Frame>;

}  // namespace ctm
