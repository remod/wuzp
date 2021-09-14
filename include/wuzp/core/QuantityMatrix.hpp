#pragma once

#include <ostream>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/units/Quantity.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/value_matrix/ValueMatrixTraits.hpp"

namespace wuzp {

// template <int ContravariantIndices, int CovariantIndices>
// class Tensor;
//
// using Scalar = Tensor<0, 0>;
// using EuclideanVector = Tensor<1, 0>;       // R * v
// using LinearTransformation = Tensor<1, 1>;  // R^-1 * A * R
// using RotationMatrix = Tensor<1, 1>;        // R^-1 * A * R
// using InertiaTensor = Tensor<0, 2>;         // R^T * A * R

// Orthogonal matrix: R^-1 == R^T

template <typename Values, typename Units>
class QuantityMatrix {
 public:
  static_assert(ValueMatrixTraits::Rows<Values>::value == Rows<Units>::value, "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<Values>::value == Cols<Units>::value, "Number of columns do not match.");

  QuantityMatrix() = default;
  template <typename ValuesOther>
  QuantityMatrix(const QuantityMatrix<ValuesOther, Units>& other) : values_(other.getValues()) {}
  template <typename... Ts>
  explicit QuantityMatrix(Ts... ts) : values_(ts...) {}
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
    static_assert(std::is_same<typename Element<Units, RowId, ColId>::type, typename QuantityTraits::GetUnit<Quantity>::type>::value,
                  "Unit type mismatch.");
    setValue<RowId, ColId>(QuantityTraits::GetValue<Quantity>::get(quantity));
  }
  template <int RowId, int ColId>
  auto getQuantity() const {
    return QuantityTraits::Create<Quantity<typename Values::Scalar, typename Element<Units, RowId, ColId>::type>>::create(
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
    // As boost does not support Quantity::Quantity(value), we need to call our helper create function here.
    return QuantityTraits::Create<typename DotProduct<This, QuantityMatrix<ValuesOther, UnitsOther>>::type>::create(
        values_.dot(other.getValues()));
  }

  auto inverse() const { return typename Inverse<This>::type(values_.inverse()); }

 private:
  using This = QuantityMatrix<Values, Units>;

  Values values_;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Sum<ValuesLhs, ValuesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Difference<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Difference<ValuesLhs, ValuesRhs>::type,
                              typename Difference<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
struct Product<QuantityMatrix<ValuesLhs, UnitsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Product<ValuesLhs, ValueRhs>::type, typename Product<UnitsLhs, UnitRhs>::type>;
};
template <typename ValueLhs, typename UnitLhs, typename ValuesRhs, typename UnitsRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type = typename Product<QuantityMatrix<ValuesRhs, UnitsRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct Product<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Product<ValuesLhs, ValuesRhs>::type, typename Product<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
struct Division<QuantityMatrix<ValuesLhs, UnitsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Division<ValuesLhs, ValueRhs>::type, typename Division<UnitsLhs, UnitRhs>::type>;
};

template <typename Values, typename Units>
struct Transpose<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Transpose<Values>::type, typename Transpose<Units>::type>;
};

template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
struct DotProduct<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>> {
  using type =
      Quantity<typename ValueMatrixTraits::template DotProduct<ValuesLhs, ValuesRhs>::type, typename DotProduct<UnitsLhs, UnitsRhs>::type>;
};

template <typename Values, typename Units>
struct Inverse<QuantityMatrix<Values, Units>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Inverse<Values>::type, typename Inverse<Units>::type>;
};

template <typename Values, typename Units, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<QuantityMatrix<Values, Units>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Block<Values, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<Units, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename Values, typename Units>
auto operator+(const QuantityMatrix<Values, Units>& quantityMatrix) {
  return quantityMatrix;
}
template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator+(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() + rhs.getValues());
}

template <typename Values, typename Units>
auto operator-(const QuantityMatrix<Values, Units>& quantityMatrix) {
  return QuantityMatrix<Values, Units>(-quantityMatrix.getValues());
}
template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator-(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return typename Difference<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() -
                                                                                                             rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename ValueRhs>
auto operator*(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValuesRhs, typename UnitsRhs>
auto operator*(const ValueLhs lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhs, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<QuantityMatrix<ValuesLhs, UnitsLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValues() * QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}
template <typename ValueLhs, typename UnitLhs, typename ValuesRhs, typename UnitsRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhs, typename UnitsLhs, typename ValuesRhs, typename UnitsRhs>
auto operator*(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const QuantityMatrix<ValuesRhs, UnitsRhs>& rhs) {
  return
      typename Product<QuantityMatrix<ValuesLhs, UnitsLhs>, QuantityMatrix<ValuesRhs, UnitsRhs>>::type(lhs.getValues() * rhs.getValues());
}

template <typename ValuesLhs, typename UnitsLhs, typename ValueRhs>
auto operator/(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValuesLhs, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const QuantityMatrix<ValuesLhs, UnitsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<QuantityMatrix<ValuesLhs, UnitsLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValues() / QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
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

}  // namespace wuzp
