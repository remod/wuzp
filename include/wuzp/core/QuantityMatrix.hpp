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

template <typename ValueMatrix, typename UnitMatrixTypeMatrix>
class QuantityMatrix {
 public:
  static_assert(ValueMatrixTraits::Rows<ValueMatrix>::value == Rows<UnitMatrixTypeMatrix>::value, "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<ValueMatrix>::value == Cols<UnitMatrixTypeMatrix>::value, "Number of columns do not match.");

  QuantityMatrix() = default;

  template <typename ValueMatrixOther, typename UnitMatrixTypeMatrixOther>
  QuantityMatrix(const QuantityMatrix<ValueMatrixOther, UnitMatrixTypeMatrixOther>& other) : valueMatrix_(other.getValueMatrix()) {
    static_assert(std::is_same<UnitMatrixTypeMatrix, UnitMatrixTypeMatrixOther>::value, "UnitMatrix mismatch.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(Quantity<typename ValueMatrixTraits::Scalar<ValueMatrix>::type, Units>... quantities)
      : valueMatrix_(QuantityTraits::GetValue<Quantity<typename ValueMatrixTraits::Scalar<ValueMatrix>::type, Units>>::get(quantities)...) {
    static_assert(std::is_same<std::tuple<Units...>, typename AsTuple<UnitMatrixTypeMatrix>::type>::value, "Unit type mismatch.");
  }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity quantity) {
    static_assert(
        std::is_same<typename Element<UnitMatrixTypeMatrix, RowId, ColId>::type, typename QuantityTraits::GetUnit<Quantity>::type>::value,
        "Unit type mismatch.");
    valueMatrix_(RowId, ColId) = QuantityTraits::GetValue<Quantity>::get(quantity);
  }
  template <int RowId, int ColId>
  auto getQuantity() const {
    return QuantityTraits::Create<Quantity<typename ValueMatrix::Scalar, typename Element<UnitMatrixTypeMatrix, RowId, ColId>::type>>::create(
        valueMatrix_(RowId, ColId));
  }

  /**
   * Construct a quantity matrix from a value matrix.
   * WARNING: This method circumvents quantity type safety.
   * @param valueMatrix Value matrix.
   */
  explicit QuantityMatrix(const ValueMatrix& valueMatrix) : valueMatrix_(valueMatrix) {}

  /**
   * Set the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @param valueMatrix Value matrix.
   */
  void setValueMatrix(const ValueMatrix& valueMatrix) { valueMatrix_ = valueMatrix; }

  /**
   * Get the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @return ValueMatrix of the quantity matrix.
   */
  const ValueMatrix& getValueMatrix() const { return valueMatrix_; }

  template <typename ValueMatrixOther, typename UnitMatrixTypeMatrixOther>
  bool isApprox(const QuantityMatrix<ValueMatrixOther, UnitMatrixTypeMatrixOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitMatrixTypeMatrix, UnitMatrixTypeMatrixOther>::value, "UnitMatrix mismatch.");
    return valueMatrix_.isApprox(other.getValueMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        valueMatrix_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<This>::type(valueMatrix_.transpose()); }

  template <typename ValueMatrixOther, typename UnitMatrixTypeMatrixOther>
  auto dot(const QuantityMatrix<ValueMatrixOther, UnitMatrixTypeMatrixOther>& other) const {
    // As boost does not support Quantity::Quantity(value), we need to call our helper create function here.
    return QuantityTraits::Create<typename DotProduct<This, QuantityMatrix<ValueMatrixOther, UnitMatrixTypeMatrixOther>>::type>::create(
        valueMatrix_.dot(other.getValueMatrix()));
  }

  auto inverse() const { return typename Inverse<This>::type(valueMatrix_.inverse()); }

 private:
  using This = QuantityMatrix<ValueMatrix, UnitMatrixTypeMatrix>;

  ValueMatrix valueMatrix_;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Sum<ValueMatrixLhs, ValueMatrixRhs>::type, typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Difference<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueRhs>::type, typename Product<UnitMatrixLhs, UnitRhs>::type>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = typename Product<QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueMatrixRhs>::type, typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      QuantityMatrix<typename ValueMatrixTraits::template Division<ValueMatrixLhs, ValueRhs>::type, typename Division<UnitMatrixLhs, UnitRhs>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Transpose<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct DotProduct<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type =
      Quantity<typename ValueMatrixTraits::template DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type, typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Inverse<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Inverse<ValueMatrix>::type, typename Inverse<UnitMatrix>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<QuantityMatrix<ValueMatrix, UnitMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Block<ValueMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
auto operator+(const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  return quantityMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator+(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(lhs.getValueMatrix() + rhs.getValueMatrix());
}

template <typename ValueMatrix, typename UnitMatrix>
auto operator-(const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  return QuantityMatrix<ValueMatrix, UnitMatrix>(-quantityMatrix.getValueMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator-(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(lhs.getValueMatrix() -
                                                                                                             rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator*(const ValueLhs lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() * QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return
      typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(lhs.getValueMatrix() * rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs>
auto operator/(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() / QuantityTraits::GetValue<Quantity<ValueRhs, UnitRhs>>::get(rhs));
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
bool operator==(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  static_assert(std::is_same<UnitMatrixLhs, UnitMatrixRhs>::value, "UnitMatrix mismatch.");
  return lhs.getValueMatrix() == rhs.getValueMatrix();
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
bool operator!=(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueMatrix, typename UnitMatrix>
std::ostream& operator<<(std::ostream& stream, const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  stream << quantityMatrix.getValueMatrix();
  return stream;
}

}  // namespace wuzp
