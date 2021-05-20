#pragma once

#include <ostream>

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
    return values_.isApprox(other.getValues());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        values_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<This>::type(values_.transpose()); }

  template <typename ValuesRhs, typename UnitsRhs>
  auto dot(const QuantityMatrix<ValuesRhs, UnitsRhs>& other) const {
    return typename DotProduct<This, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(values_.dot(other.getValues()));
  }

  auto inverse() const { return typename Inverse<This>::type(values_.inverse()); }

 private:
  using This = QuantityMatrix<Values, Units>;

  Values values_;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename ValuesSum<ValuesLhs, ValuesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Difference<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename ValuesDifference<ValuesLhs, ValuesRhs>::type, typename Difference<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Product<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename ValuesProduct<ValuesLhs, ValuesRhs>::type, typename Product<UnitsLhs, UnitsRhs>::type>;
};

template <typename Values, typename Units>
struct Transpose<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename Transpose<Values>::type, typename Transpose<Units>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct DotProduct<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename DotProduct<ValuesLhs, ValuesRhs>::type, typename DotProduct<UnitsLhs, UnitsRhs>::type>;
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
