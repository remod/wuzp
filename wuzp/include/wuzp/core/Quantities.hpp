#pragma once

#include <ostream>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename ValuesT_, typename UnitsT_>
class Quantities {
 public:
  using ValuesT = ValuesT_;
  using UnitsT = UnitsT_;
  using ScalarT = typename ValuesT::ScalarT;
  using ThisT = Quantities<ValuesT, UnitsT>;

  static_assert(Rows<ValuesT>::value == Rows<UnitsT>::value, "Number of rows do not match.");
  static_assert(Cols<ValuesT>::value == Cols<UnitsT>::value, "Number of columns do not match.");

  Quantities() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromQuantities>) Quantities(const T0& t0, const Ts&... ts)
      : Quantities(typename GetTag<ThisT, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromQuantities>) Quantities(T0&& t0, Ts&&... ts)
      : Quantities(typename GetTag<ThisT, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return ThisT(ValuesT::zero()); }
  static ThisT random() { return ThisT(ValuesT::random()); }

  template <int RowId, int ColId>
  auto getQuantity() const {
    return typename Element<ThisT, RowId, ColId>::type(values_.template getValue<RowId, ColId>());
  }

  auto getQuantity(const int rowId, const int colId) const { return typename GetType<ThisT>::type(values_.getValue(rowId, colId)); }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity& quantity) {
    static_assert(std::is_same<typename Element<UnitsT, RowId, ColId>::type, typename Quantity::UnitT>::value, "Units do not match.");
    values_.template setValue<RowId, ColId>(quantity.getValue());
  }

  template <typename Quantity>
  void setQuantity(const int rowId, const int colId, const Quantity& quantity) {
    static_assert(IsHomogeneous<ThisT>::value && std::is_same<typename Element<UnitsT, 0, 0>::type, typename Quantity::UnitT>::value,
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

  template <typename ValuesOtherT_, typename UnitsOtherT>
  bool isApprox(const Quantities<ValuesOtherT_, UnitsOtherT>& other, const ScalarT precision = Precision<ScalarT>::value()) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitsT, UnitsOtherT>::value, "Units do not match.");
    return values_.isApprox(other.getValues(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        values_.template getBlock<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<ThisT>::type(values_.transpose()); }

  template <typename ValuesOtherT_, typename UnitsOther>
  auto dotProduct(const Quantities<ValuesOtherT_, UnitsOther>& other) const {
    return typename DotProduct<ThisT, Quantities<ValuesOtherT_, UnitsOther>>::type(values_.dotProduct(other.getValues()));
  }

  template <typename ValuesOtherT_, typename UnitsOther>
  auto crossProduct(const Quantities<ValuesOtherT_, UnitsOther>& other) const {
    return typename CrossProduct<ThisT, Quantities<ValuesOtherT_, UnitsOther>>::type(values_.crossProduct(other.getValues()));
  }

  auto inverse() const { return typename Inverse<ThisT>::type(values_.inverse()); }

  //  template <bool IsHomogeneousV = IsHomogeneous<UnitsT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto norm() const {
    static_assert(!IsAbsolute<ThisT>::value, "Quantity matrix is not relative.");
    return typename GetType<ThisT>::type(values_.norm());
  }

  //  template <bool IsHomogeneousV = IsHomogeneous<UnitsT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto squaredNorm() const {
    static_assert(!IsAbsolute<ThisT>::value, "Quantity matrix is not relative.");
    return typename Power<typename GetType<ThisT>::type, Integer<2>>::type(values_.squaredNorm());
  }

 private:
  template <typename ValuesOtherT_, typename UnitsOther>
  // Allow other value matrix types to support lazy evaluation.
  Quantities(FromQuantities /*tag*/, const Quantities<ValuesOtherT_, UnitsOther>& other) : values_(other.getValues()) {
    static_assert(std::is_same<UnitsT, UnitsOther>::value, "Units do not match.");
  }
  template <typename ValuesOtherT_, typename UnitsOther>
  // Allow other value matrix types to support lazy evaluation.
  Quantities(FromQuantities /*tag*/, Quantities<ValuesOtherT_, UnitsOther>&& other)
      : values_(std::move(other.getValues())) {
    static_assert(std::is_same<UnitsT, UnitsOther>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit Quantities(FromQuantitiesArgs /*tag*/, const Quantity<typename Scalar<ValuesT>::type, Units>&... quantities)
      : Quantities(FromValuesArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitsT>::type>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit Quantities(FromQuantitiesArgs /*tag*/, Quantity<typename Scalar<ValuesT>::type, Units>&&... quantities)
      : Quantities(FromValuesArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitsT>::type>::value, "Units do not match.");
  }

  template <typename... Ts>
  explicit Quantities(FromValuesArgs /*tag*/, const Ts&... ts) : values_(ValuesT(ts...)) {}
  template <typename... Ts>
  explicit Quantities(FromValuesArgs /*tag*/, Ts&&... ts) : values_(ValuesT(std::forward<Ts>(ts)...)) {}

  ValuesT values_;
};

template <typename ValuesT_, typename UnitsT_, typename... Ts>
struct GetTag<Quantities<ValuesT_, UnitsT_>, Ts...> {
 private:
  template <typename T>
  struct IsQuantity : std::false_type {};
  template <typename Value, typename Unit>
  struct IsQuantity<Quantity<Value, Unit>> : std::true_type {};
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<IsSpecialization<first, Quantities>::value, FromQuantities,
                          typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantitiesArgs, FromValuesArgs>::type>::type,
      typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantitiesArgs, FromValuesArgs>::type>::type;
};

template <typename ValuesT_, typename Units>
struct GetType<Quantities<ValuesT_, Units>> {
  using type = Quantity<typename Quantities<ValuesT_, Units>::ScalarT, typename GetType<Units>::type>;
};

template <typename ValuesT_, typename Units>
struct Dimension<Quantities<ValuesT_, Units>> {
  static constexpr int value = Dimension<ValuesT_>::value;
};

template <typename ValuesT_, typename Units>
struct Rows<Quantities<ValuesT_, Units>> {
  static constexpr int value = Rows<ValuesT_>::value;
};

template <typename ValuesT_, typename Units>
struct Cols<Quantities<ValuesT_, Units>> {
  static constexpr int value = Cols<ValuesT_>::value;
};

template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
struct Sum<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>> {
  using type = Quantities<typename Sum<ValuesLhsT_, ValuesRhsT_>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
struct Difference<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>> {
  using type = Quantities<typename Difference<ValuesLhsT_, ValuesRhsT_>::type, typename Difference<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
struct Product<Quantities<ValuesLhsT_, UnitsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = Quantities<typename Product<ValuesLhsT_, ValueRhs>::type, typename Product<UnitsLhs, UnitRhs>::type>;
};
template <typename ValueLhs, typename UnitLhs, typename ValuesRhsT_, typename UnitsRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, Quantities<ValuesRhsT_, UnitsRhs>> {
  using type = typename Product<Quantities<ValuesRhsT_, UnitsRhs>>::type;
};
template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
struct Product<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>> {
  using type = Quantities<typename Product<ValuesLhsT_, ValuesRhsT_>::type, typename Product<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
struct Division<Quantities<ValuesLhsT_, UnitsLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = Quantities<typename Division<ValuesLhsT_, ValueRhs>::type, typename Division<UnitsLhs, UnitRhs>::type>;
};

template <typename ValuesT_, typename Units>
struct Transpose<Quantities<ValuesT_, Units>> {
  using type = Quantities<typename Transpose<ValuesT_>::type, typename Transpose<Units>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
struct DotProduct<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>> {
  using type = Quantity<typename DotProduct<ValuesLhsT_, ValuesRhsT_>::type, typename DotProduct<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
struct CrossProduct<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>> {
  using type =
      Quantities<typename CrossProduct<ValuesLhsT_, ValuesRhsT_>::type, typename CrossProduct<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesT_, typename Units, int ElementRowId, int ElementColId>
struct Element<Quantities<ValuesT_, Units>, ElementRowId, ElementColId> {
  using type = Quantity<typename Scalar<ValuesT_>::type, typename Element<Units, ElementRowId, ElementColId>::type>;
};

template <typename ValuesT_, typename Units>
struct Inverse<Quantities<ValuesT_, Units>> {
  using type = Quantities<typename Inverse<ValuesT_>::type, typename Inverse<Units>::type>;
};

template <typename ValuesT_, typename Units, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<Quantities<ValuesT_, Units>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = Quantities<typename Block<ValuesT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<Units, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValuesT_, typename Units>
struct IsAbsolute<Quantities<ValuesT_, Units>> {
  static constexpr bool value = IsAbsolute<Units>::value;
};
template <typename ValuesT_, typename Units>
struct MakeRelative<Quantities<ValuesT_, Units>> {
  using type = Quantities<ValuesT_, typename MakeRelative<Units>::type>;
  static auto make(const Quantities<ValuesT_, Units>& object) { return type(object.getValues()); }
};
template <typename ValuesT_, typename Units>
struct MakeAbsolute<Quantities<ValuesT_, Units>> {
  using type = Quantities<ValuesT_, typename MakeAbsolute<Units>::type>;
  static auto make(const Quantities<ValuesT_, Units>& object) { return type(object.getValues()); }
};

template <typename ValuesT_, typename Units>
struct Evaluate<Quantities<ValuesT_, Units>> {
  using type = Quantities<typename Evaluate<ValuesT_>::type, Units>;
};

template <typename ValuesT_, typename Units>
auto operator+(const Quantities<ValuesT_, Units>& rhs) {
  return rhs;
}
template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
auto operator+(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  return typename Sum<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>>::type(lhs.getValues() +
                                                                                                            rhs.getValues());
}

template <typename ValuesT_, typename Units>
auto operator-(const Quantities<ValuesT_, Units>& rhs) {
  return Quantities<ValuesT_, Units>(-rhs.getValues());
}
template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
auto operator-(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  return typename Difference<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>>::type(lhs.getValues() -
                                                                                                                   rhs.getValues());
}

template <typename ValuesLhsT_, typename UnitsLhs, typename ValueRhs>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, typename AccessUnits<SpecializationDummy<ValueRhs>>::Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValuesRhsT_, typename UnitsRhs>
auto operator*(const ValueLhs lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhsT_, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<Quantities<ValuesLhsT_, UnitsLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValues() * rhs.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValuesRhsT_, typename UnitsRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  return typename Product<Quantities<ValuesLhsT_, UnitsLhs>, Quantities<ValuesRhsT_, UnitsRhs>>::type(lhs.getValues() *
                                                                                                                rhs.getValues());
}

template <typename ValuesLhsT_, typename UnitsLhs, typename ValueRhs>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<UnitsLhs, 0, 0>::type>::type;
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValuesLhsT_, typename UnitsLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<Quantities<ValuesLhsT_, UnitsLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValues() / rhs.getValue());
}

template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
bool operator==(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  static_assert(std::is_same<UnitsLhs, UnitsRhs>::value, "Units do not match.");
  return lhs.getValues() == rhs.getValues();
}

template <typename ValuesLhsT_, typename UnitsLhs, typename ValuesRhsT_, typename UnitsRhs>
bool operator!=(const Quantities<ValuesLhsT_, UnitsLhs>& lhs, const Quantities<ValuesRhsT_, UnitsRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValuesT_, typename Units>
std::ostream& operator<<(std::ostream& lhs, const Quantities<ValuesT_, Units>& rhs) {
  lhs << rhs.getValues();
  return lhs;
}

}  // namespace wuzp
