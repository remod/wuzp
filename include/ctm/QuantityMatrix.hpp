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

  // Note: Allow other value types to support lazy evaluation.
  template <typename ValuesOther>
  bool isApprox(const QuantityMatrix<ValuesOther, Units>& other) const {
    return getValues().isApprox(other.getValues());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<QuantityMatrix<Values, Units>, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        getValues().template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<QuantityMatrix<Values, Units>>::type(getValues().transpose()); }
  auto inverse() const { return typename Inverse<QuantityMatrix<Values, Units>>::type(getValues().inverse()); }

 private:
  Values values_;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename Sum<ValuesLhs, ValuesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Difference<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename Difference<ValuesLhs, ValuesRhs>::type, typename Difference<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Product<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename Product<ValuesLhs, ValuesRhs>::type, typename Difference<UnitsLhs, UnitsRhs>::type>;
};

template <typename Values, typename Units>
struct Transpose<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename Transpose<Values>::type, typename Transpose<Units>::type>;
};

template <typename Values, typename Units>
struct Inverse<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename Inverse<Values>::type, typename Inverse<Units>::type>;
};

template <typename Values, typename Units, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<QuantityMatrix<Values, Units>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename Block<Values, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<Units, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator+(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() + rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator-(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return typename Difference<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() -
                                                                                                             rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator*(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return
      typename Product<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() * rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
bool operator==(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  static_assert(std::is_same<UnitsLhs, UnitsRhs>::value && "Units mismatch.");
  return lhs.getValues() == rhs.getValues();
}

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
bool operator!=(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename Values, typename Units>
std::ostream& operator<<(std::ostream& stream, const QuantityMatrix<Values, Units>& quantityMatrix) {
  stream << quantityMatrix.getValues();
  return stream;
}

}  // namespace ctm
