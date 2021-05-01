#pragma once

#include <ostream>

#include "ctm/Frame.hpp"
#include "ctm/QuantityMatrix.hpp"

namespace ctm {

template <typename Values, typename Units, typename Frames>
class FramedQuantityMatrix : public QuantityMatrix<Values, Units> {
 public:
  explicit FramedQuantityMatrix(const Values& values) : QuantityMatrix<Values, Units>(values) {}

//  void setValues(const Values& values) { values_ = values; }
//  const Values& getValues() const { return values_; }
//
// private:
//  Values values_;
};

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
bool operator==(const FramedQuantityMatrix<ValuesLhs, FramesLhs, UnitsLhs>& lhs, const FramedQuantityMatrix<ValuesRhs, FramesRhs, UnitsRhs>& rhs) {
  return lhs.getValues() == rhs.getValues();
}

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
bool operator!=(const FramedQuantityMatrix<ValuesLhs, FramesLhs, UnitsLhs>& lhs, const FramedQuantityMatrix<ValuesRhs, FramesRhs, UnitsRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
struct Sum<FramedQuantityMatrix<ValuesLhs, FramesLhs, UnitsLhs>, FramedQuantityMatrix<ValuesRhs, FramesRhs, UnitsRhs>> {
  using type = FramedQuantityMatrix<typename Sum<ValuesLhs, ValuesRhs>::type, typename Sum<FramesLhs, FramesRhs>::type,
                              typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
auto add(const FramedQuantityMatrix<ValuesLhs, FramesLhs, UnitsLhs>& lhs, const FramedQuantityMatrix<ValuesRhs, FramesRhs, UnitsRhs>& rhs) {
  return typename Sum<FramedQuantityMatrix<ValuesLhs, FramesLhs, UnitsLhs>, FramedQuantityMatrix<ValuesRhs, FramesRhs, UnitsRhs>>::type(
      lhs.getValues() + rhs.getValues());
}

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
auto operator+(const FramedQuantityMatrix<ValuesLhs, FramesLhs, UnitsLhs>& lhs, const FramedQuantityMatrix<ValuesRhs, FramesRhs, UnitsRhs>& rhs) {
  return add(lhs, rhs);
}

template <typename Values, typename Frames, typename Units>
struct GetFrame<FramedQuantityMatrix<Values, Frames, Units>> {
  static_assert(IsHomogeneous<Frames>::value, "Frames are heterogeneous.");
  using type = std::tuple_element<0, typename Frames::AsTuple>;
};

template <typename Values, typename Frames, typename Units>
std::ostream& operator<<(std::ostream& stream, const FramedQuantityMatrix<Values, Frames, Units>& framedQuantityMatrix) {
  stream << framedQuantityMatrix.getValues();
  return stream;
}

}  // namespace ctm
