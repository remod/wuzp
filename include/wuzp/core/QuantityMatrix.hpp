#pragma once

#include <ostream>
#include <type_traits>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/units/Quantity.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/value_matrix/ValueMatrixTraits.hpp"

namespace wuzp {

template <typename ValueMatrix_, typename UnitMatrix_>
class QuantityMatrix {
 public:
  using ValueMatrixT = ValueMatrix_;
  using UnitMatrixT = UnitMatrix_;
  using This = QuantityMatrix<ValueMatrixT, UnitMatrixT>;

  static_assert(ValueMatrixTraits::Rows<ValueMatrixT>::value == Rows<UnitMatrixT>::value, "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<ValueMatrixT>::value == Cols<UnitMatrixT>::value, "Number of columns do not match.");

  QuantityMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromQuantityMatrix>) QuantityMatrix(const T0& t0, const Ts&... ts)
      : QuantityMatrix(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromQuantityMatrix>) QuantityMatrix(T0&& t0, Ts&&... ts)
      : QuantityMatrix(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  ~QuantityMatrix() = default;

  static This zero() { return This(ValueMatrixTraits::Zero<ValueMatrixT>::value()); }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity quantity) {
    static_assert(std::is_same<typename Element<UnitMatrixT, RowId, ColId>::type, typename QuantityTraits::GetUnit<Quantity>::type>::value,
                  "Units do not match.");
    valueMatrix_(RowId, ColId) = QuantityTraits::GetValue<Quantity>::get(quantity);
  }
  template <int RowId, int ColId>
  auto getQuantity() const {
    return QuantityTraits::Create<Quantity<typename ValueMatrixT::Scalar, typename Element<UnitMatrixT, RowId, ColId>::type>>::create(
        valueMatrix_(RowId, ColId));
  }

  /**
   * Set the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @param valueMatrix Value matrix.
   */
  template <typename ValueMatrixOther>
  void setValueMatrix(const ValueMatrixOther& valueMatrix) {
    valueMatrix_ = valueMatrix;
  }

  /**
   * Get the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @return ValueMatrix of the quantity matrix.
   */
  const ValueMatrixT& getValueMatrix() const { return valueMatrix_; }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  bool isApprox(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
    return valueMatrix_.isApprox(other.getValueMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        valueMatrix_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<This>::type(valueMatrix_.transpose()); }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  auto dot(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& other) const {
    // As boost does not support Quantity::Quantity(value), we need to call our helper create function here.
    return QuantityTraits::Create<typename DotProduct<This, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>>::type>::create(
        valueMatrix_.dot(other.getValueMatrix()));
  }

  auto inverse() const { return typename Inverse<This>::type(valueMatrix_.inverse()); }

 private:
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  explicit QuantityMatrix(FromQuantityMatrix /*tag*/, const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& other)
      : valueMatrix_(other.getValueMatrix()) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  explicit QuantityMatrix(FromQuantityMatrix /*tag*/, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>&& other)
      : valueMatrix_(std::move(other.getValueMatrix())) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(FromQuantities /*tag*/,
                          const Quantity<typename ValueMatrixTraits::Scalar<ValueMatrixT>::type, Units>&... quantities)
      : valueMatrix_(
            QuantityTraits::GetValue<Quantity<typename ValueMatrixTraits::Scalar<ValueMatrixT>::type, Units>>::get(quantities)...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(FromQuantities /*tag*/, Quantity<typename ValueMatrixTraits::Scalar<ValueMatrixT>::type, Units>&&... quantities)
      : valueMatrix_(
            QuantityTraits::GetValue<Quantity<typename ValueMatrixTraits::Scalar<ValueMatrixT>::type, Units>>::get(quantities)...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  template <typename... Ts>
  explicit QuantityMatrix(FromValueMatrixArgs /*tag*/, const Ts&... ts)
      : valueMatrix_(ValueMatrixTraits::Construct<ValueMatrixT, Ts...>::value(ts...)) {}
  template <typename... Ts>
  explicit QuantityMatrix(FromValueMatrixArgs /*tag*/, Ts&&... ts)
      : valueMatrix_(ValueMatrixTraits::Construct<ValueMatrixT, Ts...>::value(std::forward<Ts>(ts)...)) {}

  ValueMatrixT valueMatrix_;
};

template <typename... Xs, typename... Ts>
struct GetTag<QuantityMatrix<Xs...>, Ts...> {
 private:
  template <typename T>
  struct IsQuantity : std::false_type {};
  template <typename Value, typename Unit>
  struct IsQuantity<Quantity<Value, Unit>> : std::true_type {};
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type =
      typename Chose<sizeof...(Ts) == 1,
                     typename Chose<IsSpecialization<first, QuantityMatrix>::value, FromQuantityMatrix,
                                    typename Chose<(IsQuantity<Ts>::value && ...), FromQuantities, FromValueMatrixArgs>::type>::type,
                     typename Chose<(IsQuantity<Ts>::value && ...), FromQuantities, FromValueMatrixArgs>::type>::type;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Dimension<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr int value = ValueMatrixTraits::Dimension<ValueMatrix>::value;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Rows<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr int value = ValueMatrixTraits::Rows<ValueMatrix>::value;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Cols<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr int value = ValueMatrixTraits::Cols<ValueMatrix>::value;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Sum<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Difference<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueRhs>::type,
                              typename Product<UnitMatrixLhs, UnitRhs>::type>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = typename Product<QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Division<ValueMatrixLhs, ValueRhs>::type,
                              typename Division<UnitMatrixLhs, UnitRhs>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Transpose<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::template Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct DotProduct<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = Quantity<typename ValueMatrixTraits::template DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type,
                        typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
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
struct IsAbsolute<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr bool value = IsAbsolute<UnitMatrix>::value;
};
template <typename ValueMatrix, typename UnitMatrix>
struct MakeRelative<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<ValueMatrix, typename MakeRelative<UnitMatrix>::type>;
  static auto make(const QuantityMatrix<ValueMatrix, UnitMatrix>& object) { return type(object.getValueMatrix()); }
};
template <typename ValueMatrix, typename UnitMatrix>
struct MakeAbsolute<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<ValueMatrix, typename MakeAbsolute<UnitMatrix>::type>;
  static auto make(const QuantityMatrix<ValueMatrix, UnitMatrix>& object) { return type(object.getValueMatrix()); }
};

template <typename ValueMatrix, typename UnitMatrix>
struct Evaluate<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename ValueMatrixTraits::Evaluate<ValueMatrix>::type, UnitMatrix>;
};

template <typename ValueMatrix, typename UnitMatrix>
auto operator+(const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  return quantityMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator+(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(
      lhs.getValueMatrix() + rhs.getValueMatrix());
}

template <typename ValueMatrix, typename UnitMatrix>
auto operator-(const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  return QuantityMatrix<ValueMatrix, UnitMatrix>(-quantityMatrix.getValueMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator-(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(
      lhs.getValueMatrix() - rhs.getValueMatrix());
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
  return typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(
      lhs.getValueMatrix() * rhs.getValueMatrix());
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
  static_assert(std::is_same<UnitMatrixLhs, UnitMatrixRhs>::value, "Units do not match.");
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
