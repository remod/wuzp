#pragma once

#include <ostream>

#include "ctm/Frame.hpp"
#include "ctm/QuantityMatrix.hpp"

namespace ctm {

template <typename Values, typename Units, typename Frames>
class FramedQuantityMatrix {
 public:
  FramedQuantityMatrix() = default;
  explicit FramedQuantityMatrix(const Values& values) : quantityMatrix_(values) {}

  const QuantityMatrix<Values, Units>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValues(const Values& values) { quantityMatrix_.setValues(values); }
  const Values& getValues() const { return quantityMatrix_.getValues(); }

  auto transpose() const {
    return typename Transpose<FramedQuantityMatrix<Values, Units, Frames>>::type(getValues().transpose());
  }

 private:
  QuantityMatrix<Values, Units> quantityMatrix_;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
bool operator==(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
                const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  static_assert(std::is_same<FramesLhs, FramesRhs>::value && "Frames mismatch.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
bool operator!=(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
                const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
struct Sum<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>> {
  using type = FramedQuantityMatrix<typename Sum<ValuesLhs, ValuesRhs>::type,
      typename Sum<UnitsLhs, UnitsRhs>::type, typename Sum<FramesLhs, FramesRhs>::type>;
};

template <typename Values, typename Units, typename Frames>
struct Transpose<FramedQuantityMatrix<Values, Units, Frames>> {
  using type = FramedQuantityMatrix<typename Transpose<Values>::type, typename Transpose<Units>::type, typename Transpose<Frames>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto add(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs, const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return typename Sum<FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>, FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>>::type(
      lhs.getValues() + rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename FramesLhs, typename ValuesRhs, typename UnitsRhs, typename FramesRhs>
auto operator+(const FramedQuantityMatrix<ValuesLhs, UnitsLhs, FramesLhs>& lhs,
               const FramedQuantityMatrix<ValuesRhs, UnitsRhs, FramesRhs>& rhs) {
  return add(lhs, rhs);
}

template <typename Values, typename Units, typename Frames>
struct GetFrame<FramedQuantityMatrix<Values, Units, Frames>> {
  static_assert(IsHomogeneous<Frames>::value, "Frames are heterogeneous.");
  using type = std::tuple_element<0, typename Frames::AsTuple>;
};

template <typename Values, typename Units, typename Frames>
std::ostream& operator<<(std::ostream& stream, const FramedQuantityMatrix<Values, Units, Frames>& framedQuantityMatrix) {
  stream << framedQuantityMatrix.getQuantityMatrix();
  return stream;
}

}  // namespace ctm
