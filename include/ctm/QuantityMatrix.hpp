#pragma once

#include <ostream>

#include "ctm/Frame.hpp"
#include "ctm/TypeMatrix.hpp"

namespace ctm {

template <typename Values, typename Units>
class QuantityMatrix {
 public:
  QuantityMatrix() = default;
  explicit QuantityMatrix(const Values& values) : values_(values) {}

  void setValues(const Values& values) { values_ = values; }
  const Values& getValues() const { return values_; }

 private:
  Values values_;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
bool operator==(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  static_assert(std::is_same<UnitsLhs, UnitsRhs>::value && "Units mismatch.");
  return lhs.getValues() == rhs.getValues();
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
bool operator!=(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename Sum<ValuesLhs, ValuesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto add(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() + rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator+(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return add(lhs, rhs);
}

template <typename Values, typename Units>
std::ostream& operator<<(std::ostream& stream, const QuantityMatrix<Values, Units>& quantityMatrix) {
  stream << quantityMatrix.getValues();
  return stream;
}

}  // namespace ctm
