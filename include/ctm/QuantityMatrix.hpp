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

  // Note: We allow other values to support lazy evaluation.
  template <typename ValuesOther>
  bool isApprox(const QuantityMatrix<ValuesOther, Units>& other) const {
    return getValues().isApprox(other.getValues());
  }

  template <int RowId, int ColId, int Rows, int Cols>
  auto block() const {
    return typename GetBlock<QuantityMatrix<Values, Units>, RowId, ColId, Rows, Cols>::type(getValues().block<Rows, Cols>(RowId, ColId));
  }

  auto transpose() const { return typename Transpose<QuantityMatrix<Values, Units>>::type(getValues().transpose()); }
  auto inverse() const { return typename Inverse<QuantityMatrix<Values, Units>>::type(getValues().inverse()); }

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

template <typename Values, typename Units, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct GetBlock<QuantityMatrix<Values, Units>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename GetBlock<Values, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename GetBlock<Units, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename Sum<ValuesLhs, ValuesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename Values, typename Units>
struct Transpose<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename Transpose<Values>::type, typename Transpose<Units>::type>;
};

template <typename Values, typename Units>
struct Inverse<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename Inverse<Values>::type, typename Inverse<Units>::type>;
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
