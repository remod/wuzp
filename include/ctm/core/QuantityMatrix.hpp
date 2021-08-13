#pragma once

#include <ostream>

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/units/Quantity.hpp"
#include "ctm/units/QuantityTraits.hpp"
#include "ctm/value_matrix/ValueMatrixTraits.hpp"

namespace ctm {

template <typename Values, typename Units>
class QuantityMatrix {
 public:
  QuantityMatrix() = default;
  explicit QuantityMatrix(const Values& values) : values_(values) {}

  void setValues(const Values& values) { values_ = values; }
  const Values& getValues() const { return values_; }

  template <int RowId, int ColId, typename Value>
  void setValue(const Value value) {
    values_(RowId, ColId) = value;
  }
  template <int RowId, int ColId>
  auto getValue() const {
    return values_(RowId, ColId);
  }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity quantity) {
    static_assert(std::is_same<typename std::tuple_element<0, typename AsTuple<Units>::type>::type,
                               typename units::QuantityTraits::GetUnit<Quantity>::type>::value,
                  "Unit type mismatch.");
    setValue<RowId, ColId>(units::QuantityTraits::GetValue<Quantity>::get(quantity));
  }
  template <int RowId, int ColId>
  auto getQuantity() {
    return units::QuantityTraits::
        Create<units::Quantity<typename Values::Scalar, typename std::tuple_element<0, typename AsTuple<Units>::type>::type>>::create(
            getValue<RowId, ColId>());
  }

  template <typename ValuesOther, typename UnitsOther>
  bool isApprox(const QuantityMatrix<ValuesOther, UnitsOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<Units, UnitsOther>::value, "Units mismatch.");
    return values_.isApprox(other.getValues());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        values_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<This>::type(values_.transpose()); }

  template <typename ValuesOther, typename UnitsOther>
  auto dot(const QuantityMatrix<ValuesOther, UnitsOther>& other) const {
    return typename DotProduct<This, QuantityMatrix<ValuesOther, UnitsOther>>::type(values_.dot(other.getValues()));
  }

  auto inverse() const { return typename Inverse<This>::type(values_.inverse()); }

 private:
  using This = QuantityMatrix<Values, Units>;

  Values values_;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Sum<ValuesLhs, ValuesRhs>::type,
                              typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Difference<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Difference<ValuesLhs, ValuesRhs>::type,
                              typename Difference<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Product<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Product<ValuesLhs, ValuesRhs>::type,
                              typename Product<UnitsLhs, UnitsRhs>::type>;
};

template <typename Values, typename Units>
struct Transpose<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Transpose<Values>::type, typename Transpose<Units>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct DotProduct<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template DotProduct<ValuesLhs, ValuesRhs>::type,
                              typename DotProduct<UnitsLhs, UnitsRhs>::type>;
};

template <typename Values, typename Units>
struct Inverse<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Inverse<Values>::type, typename Inverse<Units>::type>;
};

template <typename Values, typename Units, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<QuantityMatrix<Values, Units>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type =
      QuantityMatrix<typename value_matrix::ValueMatrixTraits::template Block<Values, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
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
  static_assert(std::is_same<UnitsLhs, UnitsRhs>::value, "Units mismatch.");
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
