#pragma once

#include <ostream>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/UnitMatrix.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename ValuesT_, typename UnitMatrix_>
class QuantityMatrix {
 public:
  using ValuesT = ValuesT_;
  using UnitMatrixT = UnitMatrix_;
  using ScalarT = typename ValuesT::ScalarT;
  using ThisT = QuantityMatrix<ValuesT, UnitMatrixT>;

  static_assert(Rows<ValuesT>::value == Rows<UnitMatrixT>::value, "Number of rows do not match.");
  static_assert(Cols<ValuesT>::value == Cols<UnitMatrixT>::value, "Number of columns do not match.");

  QuantityMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromQuantityMatrix>) QuantityMatrix(const T0& t0, const Ts&... ts)
      : QuantityMatrix(typename GetTag<ThisT, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromQuantityMatrix>) QuantityMatrix(T0&& t0, Ts&&... ts)
      : QuantityMatrix(typename GetTag<ThisT, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return ThisT(ValuesT::zero()); }
  static ThisT random() { return ThisT(ValuesT::random()); }

  template <int RowId, int ColId>
  auto getQuantity() const {
    return typename Element<ThisT, RowId, ColId>::type(values_.template getValue<RowId, ColId>());
  }

  auto getQuantity(const int rowId, const int colId) const { return typename GetType<ThisT>::type(values_.getValue(rowId, colId)); }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity& quantity) {
    static_assert(std::is_same<typename Element<UnitMatrixT, RowId, ColId>::type, typename Quantity::UnitT>::value, "Units do not match.");
    values_.template setValue<RowId, ColId>(quantity.getValue());
  }

  template <typename Quantity>
  void setQuantity(const int rowId, const int colId, const Quantity& quantity) {
    static_assert(IsHomogeneous<ThisT>::value && std::is_same<typename Element<UnitMatrixT, 0, 0>::type, typename Quantity::UnitT>::value,
                  "Units do not match.");
    values_.setValue(rowId, colId, quantity.getValue());
  }

  /**
   * Set the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @param values Values.
   */
  template <typename ValuesOtherT_>
  void setValues(const ValuesOtherT_& values) {
    values_ = values;
  }

  /**
   * Get the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @return Values of the quantity matrix.
   */
  const ValuesT& getValues() const { return values_; }

  template <typename ValuesOtherT_, typename UnitMatrixOtherT>
  bool isApprox(const QuantityMatrix<ValuesOtherT_, UnitMatrixOtherT>& other, const ScalarT precision = Precision<ScalarT>::value()) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOtherT>::value, "Units do not match.");
    return values_.isApprox(other.getValues(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        values_.template getBlock<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<ThisT>::type(values_.transpose()); }

  template <typename ValuesOtherT_, typename UnitMatrixOther>
  auto dotProduct(const QuantityMatrix<ValuesOtherT_, UnitMatrixOther>& other) const {
    return typename DotProduct<ThisT, QuantityMatrix<ValuesOtherT_, UnitMatrixOther>>::type(values_.dotProduct(other.getValues()));
  }

  template <typename ValuesOtherT_, typename UnitMatrixOther>
  auto crossProduct(const QuantityMatrix<ValuesOtherT_, UnitMatrixOther>& other) const {
    return typename CrossProduct<ThisT, QuantityMatrix<ValuesOtherT_, UnitMatrixOther>>::type(values_.crossProduct(other.getValues()));
  }

  auto inverse() const { return typename Inverse<ThisT>::type(values_.inverse()); }

  //  template <bool IsHomogeneousV = IsHomogeneous<UnitMatrixT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto norm() const {
    static_assert(!IsAbsolute<ThisT>::value, "Quantity matrix is not relative.");
    return typename GetType<ThisT>::type(values_.norm());
  }

  //  template <bool IsHomogeneousV = IsHomogeneous<UnitMatrixT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto squaredNorm() const {
    static_assert(!IsAbsolute<ThisT>::value, "Quantity matrix is not relative.");
    return typename Power<typename GetType<ThisT>::type, Integer<2>>::type(values_.squaredNorm());
  }

 private:
  template <typename ValuesOtherT_, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  QuantityMatrix(FromQuantityMatrix /*tag*/, const QuantityMatrix<ValuesOtherT_, UnitMatrixOther>& other) : values_(other.getValues()) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }
  template <typename ValuesOtherT_, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  QuantityMatrix(FromQuantityMatrix /*tag*/, QuantityMatrix<ValuesOtherT_, UnitMatrixOther>&& other)
      : values_(std::move(other.getValues())) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(FromQuantities /*tag*/, const Quantity<typename Scalar<ValuesT>::type, Units>&... quantities)
      : QuantityMatrix(FromValuesArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(FromQuantities /*tag*/, Quantity<typename Scalar<ValuesT>::type, Units>&&... quantities)
      : QuantityMatrix(FromValuesArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  template <typename... Ts>
  explicit QuantityMatrix(FromValuesArgs /*tag*/, const Ts&... ts) : values_(ValuesT(ts...)) {}
  template <typename... Ts>
  explicit QuantityMatrix(FromValuesArgs /*tag*/, Ts&&... ts) : values_(ValuesT(std::forward<Ts>(ts)...)) {}

  ValuesT values_;
};

template <typename ValuesT_, typename UnitMatrix_, typename... Ts>
struct GetTag<QuantityMatrix<ValuesT_, UnitMatrix_>, Ts...> {
 private:
  template <typename T>
  struct IsQuantity : std::false_type {};
  template <typename Value, typename Unit>
  struct IsQuantity<Quantity<Value, Unit>> : std::true_type {};
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<IsSpecialization<first, QuantityMatrix>::value, FromQuantityMatrix,
                          typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantities, FromValuesArgs>::type>::type,
      typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantities, FromValuesArgs>::type>::type;
};

template <typename ValuesT_, typename UnitMatrix>
struct GetType<QuantityMatrix<ValuesT_, UnitMatrix>> {
  using type = Quantity<typename QuantityMatrix<ValuesT_, UnitMatrix>::ScalarT, typename GetType<UnitMatrix>::type>;
};

template <typename ValuesT_, typename UnitMatrix>
struct Dimension<QuantityMatrix<ValuesT_, UnitMatrix>> {
  static constexpr int value = Dimension<ValuesT_>::value;
};

template <typename ValuesT_, typename UnitMatrix>
struct Rows<QuantityMatrix<ValuesT_, UnitMatrix>> {
  static constexpr int value = Rows<ValuesT_>::value;
};

template <typename ValuesT_, typename UnitMatrix>
struct Cols<QuantityMatrix<ValuesT_, UnitMatrix>> {
  static constexpr int value = Cols<ValuesT_>::value;
};

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
struct Sum<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename Sum<ValuesLhsT_, ValuesRhsT_>::type, typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
struct Difference<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename Difference<ValuesLhsT_, ValuesRhsT_>::type, typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename Product<ValuesLhsT_, ValueRhs>::type, typename Product<UnitMatrixLhs, UnitRhs>::type>;
};
template <typename ValueLhs, typename UnitLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>> {
  using type = typename Product<QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>>::type;
};
template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
struct Product<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename Product<ValuesLhsT_, ValuesRhsT_>::type, typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename Division<ValuesLhsT_, ValueRhs>::type, typename Division<UnitMatrixLhs, UnitRhs>::type>;
};

template <typename ValuesT_, typename UnitMatrix>
struct Transpose<QuantityMatrix<ValuesT_, UnitMatrix>> {
  using type = QuantityMatrix<typename Transpose<ValuesT_>::type, typename Transpose<UnitMatrix>::type>;
};

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
struct DotProduct<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>> {
  using type = Quantity<typename DotProduct<ValuesLhsT_, ValuesRhsT_>::type, typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
struct CrossProduct<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>> {
  using type =
      QuantityMatrix<typename CrossProduct<ValuesLhsT_, ValuesRhsT_>::type, typename CrossProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValuesT_, typename UnitMatrix, int ElementRowId, int ElementColId>
struct Element<QuantityMatrix<ValuesT_, UnitMatrix>, ElementRowId, ElementColId> {
  using type = Quantity<typename Scalar<ValuesT_>::type, typename Element<UnitMatrix, ElementRowId, ElementColId>::type>;
};

template <typename ValuesT_, typename UnitMatrix>
struct Inverse<QuantityMatrix<ValuesT_, UnitMatrix>> {
  using type = QuantityMatrix<typename Inverse<ValuesT_>::type, typename Inverse<UnitMatrix>::type>;
};

template <typename ValuesT_, typename UnitMatrix, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<QuantityMatrix<ValuesT_, UnitMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename Block<ValuesT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValuesT_, typename UnitMatrix>
struct IsAbsolute<QuantityMatrix<ValuesT_, UnitMatrix>> {
  static constexpr bool value = IsAbsolute<UnitMatrix>::value;
};
template <typename ValuesT_, typename UnitMatrix>
struct MakeRelative<QuantityMatrix<ValuesT_, UnitMatrix>> {
  using type = QuantityMatrix<ValuesT_, typename MakeRelative<UnitMatrix>::type>;
  static auto make(const QuantityMatrix<ValuesT_, UnitMatrix>& object) { return type(object.getValues()); }
};
template <typename ValuesT_, typename UnitMatrix>
struct MakeAbsolute<QuantityMatrix<ValuesT_, UnitMatrix>> {
  using type = QuantityMatrix<ValuesT_, typename MakeAbsolute<UnitMatrix>::type>;
  static auto make(const QuantityMatrix<ValuesT_, UnitMatrix>& object) { return type(object.getValues()); }
};

template <typename ValuesT_, typename UnitMatrix>
struct Evaluate<QuantityMatrix<ValuesT_, UnitMatrix>> {
  using type = QuantityMatrix<typename Evaluate<ValuesT_>::type, UnitMatrix>;
};

template <typename ValuesT_, typename UnitMatrix>
auto operator+(const QuantityMatrix<ValuesT_, UnitMatrix>& rhs) {
  return rhs;
}
template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
auto operator+(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>>::type(lhs.getValues() +
                                                                                                                    rhs.getValues());
}

template <typename ValuesT_, typename UnitMatrix>
auto operator-(const QuantityMatrix<ValuesT_, UnitMatrix>& rhs) {
  return QuantityMatrix<ValuesT_, UnitMatrix>(-rhs.getValues());
}
template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
auto operator-(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  return typename Difference<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>>::type(lhs.getValues() -
                                                                                                                           rhs.getValues());
}

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValueRhs>
auto operator*(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, typename AccessUnits<SpecializationDummy<ValueRhs>>::Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
auto operator*(const ValueLhs lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValues() * rhs.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
auto operator*(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  return typename Product<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>>::type(lhs.getValues() *
                                                                                                                        rhs.getValues());
}

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValueRhs>
auto operator/(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<UnitMatrixLhs, 0, 0>::type>::type;
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValues() / rhs.getValue());
}

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
bool operator==(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  static_assert(std::is_same<UnitMatrixLhs, UnitMatrixRhs>::value, "Units do not match.");
  return lhs.getValues() == rhs.getValues();
}

template <typename ValuesLhsT_, typename UnitMatrixLhs, typename ValuesRhsT_, typename UnitMatrixRhs>
bool operator!=(const QuantityMatrix<ValuesLhsT_, UnitMatrixLhs>& lhs, const QuantityMatrix<ValuesRhsT_, UnitMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValuesT_, typename UnitMatrix>
std::ostream& operator<<(std::ostream& lhs, const QuantityMatrix<ValuesT_, UnitMatrix>& rhs) {
  lhs << rhs.getValues();
  return lhs;
}

}  // namespace wuzp
