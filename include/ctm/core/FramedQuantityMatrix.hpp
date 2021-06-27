#pragma once

#include <ostream>

#include "ctm/core/Frame.hpp"
#include "ctm/core/QuantityMatrix.hpp"
#include "ctm/units/Unit.hpp"
#include "ctm/units/UnitMatrix.hpp"
#include "ctm/value_matrix/ValueMatrix.hpp"
#include "ctm/value_matrix/ValueMatrixTraits.hpp"

namespace ctm {

template <typename Values, typename Units, typename Frames>
class FramedQuantityMatrix {
 public:
  FramedQuantityMatrix() = default;
  explicit FramedQuantityMatrix(const Values& values) : quantityMatrix_(values) {}

  const QuantityMatrix<Values, Units>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValues(const Values& values) { quantityMatrix_.setValues(values); }
  const Values& getValues() const { return quantityMatrix_.getValues(); }

  auto transpose() const { return typename Transpose<This>::type(getValues().transpose()); }

  template <typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
  auto dot(const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& other) const {
    return typename DotProduct<This, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(getValues().dot(other.getValues()));
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

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  static_assert(FakeDependency<Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>,
                                       FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>>::value,
                "Framed quantity matrices cannot be multiplied.");
};

template <typename Values, typename Units, typename Frames>
struct Transpose<FramedQuantityMatrix<Values, Units, Frames>> {
  using type = FramedQuantityMatrix<typename value_matrix::ValueMatrixTraits::template Transpose<Values>::type,
                                    typename Transpose<Units>::type, typename Transpose<Frames>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct DotProduct<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template DotProduct<ValuesLhs, ValuesRhs>::type,
                              typename DotProduct<UnitsLhs, UnitsRhs>::type>;
  using assert_frames_match = typename DotProduct<FramesLhs, FramesRhs>::type;
};

template <typename Values, typename Units, typename Frames>
struct GetFrame<FramedQuantityMatrix<Values, Units, Frames>> {
  static_assert(IsHomogeneous<Frames>::value, "Frames are heterogeneous.");
  using type = std::tuple_element<0, typename Frames::AsTuple>;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator+(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return typename Sum<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
      lhs.getValues() + rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator-(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return
      typename Difference<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
          lhs.getValues() - rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator*(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return typename Product<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
      lhs.getValues() * rhs.getValues());
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
using Position = FramedQuantityMatrix<value_matrix::ValueMatrix<Scalar, Dimension, 1>, FrameMatrix<Dimension, 1, Frame>,
                                      units::UnitMatrix<Dimension, 1, units::Absolute<units::Length>>>;

template <typename Scalar, typename Frame>
using Position2D = Position<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Position3D = Position<3, Scalar, Frame>;

template <int Dimension, typename Scalar, typename Frame>
using Vector = FramedQuantityMatrix<value_matrix::ValueMatrix<Scalar, Dimension, 1>, FrameMatrix<Dimension, 1, Frame>,
                                    units::UnitMatrix<Dimension, 1, units::Length>>;

template <typename Scalar, typename Frame>
using Vector2D = Vector<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Vector3D = Vector<3, Scalar, Frame>;

template <int Dimension, typename Scalar, typename Frame>
using Velocity = FramedQuantityMatrix<Eigen::Matrix<Scalar, Dimension, 1>, FrameMatrix<Dimension, 1, Frame>,
                                      units::UnitMatrix<Dimension, 1, units::Velocity>>;

template <typename Scalar, typename Frame>
using Velocity2D = Velocity<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Velocity3D = Velocity<3, Scalar, Frame>;

}  // namespace ctm
