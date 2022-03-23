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
  using ValueT = typename ValuesT::ValueT;
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

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  bool isApprox(const Quantities<ValuesOtherT_, UnitsOtherT_>& other, const ValueT precision = Precision<ValueT>::value()) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitsT, UnitsOtherT_>::value, "Units do not match.");
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
  Quantities(FromQuantities /*tag*/, Quantities<ValuesOtherT_, UnitsOther>&& other) : values_(std::move(other.getValues())) {
    static_assert(std::is_same<UnitsT, UnitsOther>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... QuantityUnitsT_>
  explicit Quantities(FromQuantitiesArgs /*tag*/, const Quantity<typename Value<ValuesT>::type, QuantityUnitsT_>&... quantities)
      : Quantities(FromValuesArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<QuantityUnitsT_...>, typename GetTypes<UnitsT>::type>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... QuantityUnitsT_>
  explicit Quantities(FromQuantitiesArgs /*tag*/, Quantity<typename Value<ValuesT>::type, QuantityUnitsT_>&&... quantities)
      : Quantities(FromValuesArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<QuantityUnitsT_...>, typename GetTypes<UnitsT>::type>::value, "Units do not match.");
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
      typename IfThenElse<
          IsSpecialization<first, Quantities>::value, FromQuantities,
          typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantitiesArgs, FromValuesArgs>::type>::type,
      typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantitiesArgs, FromValuesArgs>::type>::type;
};

template <typename ValuesT_, typename UnitsT_>
struct GetType<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantity<typename Quantities<ValuesT_, UnitsT_>::ValueT, typename GetType<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Dimension<Quantities<ValuesT_, UnitsT_>> {
  static constexpr int value = Dimension<ValuesT_>::value;
};

template <typename ValuesT_, typename UnitsT_>
struct Rows<Quantities<ValuesT_, UnitsT_>> {
  static constexpr int value = Rows<ValuesT_>::value;
};

template <typename ValuesT_, typename UnitsT_>
struct Cols<Quantities<ValuesT_, UnitsT_>> {
  static constexpr int value = Cols<ValuesT_>::value;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Sum<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename Sum<ValuesLhsT_, ValuesRhsT_>::type, typename Sum<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Difference<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename Difference<ValuesLhsT_, ValuesRhsT_>::type, typename Difference<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = Quantities<typename Product<ValuesLhsT_, ValueRhsT_>::type, typename Product<UnitsLhsT_, UnitRhsT_>::type>;
};
template <typename ValueLhsT_, typename UnitLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Product<Quantity<ValueLhsT_, UnitLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = typename Product<Quantities<ValuesRhsT_, UnitsRhsT_>>::type;
};
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename Product<ValuesLhsT_, ValuesRhsT_>::type, typename Product<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = Quantities<typename Division<ValuesLhsT_, ValueRhsT_>::type, typename Division<UnitsLhsT_, UnitRhsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Transpose<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Transpose<ValuesT_>::type, typename Transpose<UnitsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct DotProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantity<typename DotProduct<ValuesLhsT_, ValuesRhsT_>::type, typename DotProduct<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct CrossProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename CrossProduct<ValuesLhsT_, ValuesRhsT_>::type, typename CrossProduct<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int ElementRowId, int ElementColId>
struct Element<Quantities<ValuesT_, UnitsT_>, ElementRowId, ElementColId> {
  using type = Quantity<typename Value<ValuesT_>::type, typename Element<UnitsT_, ElementRowId, ElementColId>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Inverse<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Inverse<ValuesT_>::type, typename Inverse<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<Quantities<ValuesT_, UnitsT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = Quantities<typename Block<ValuesT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                          typename Block<UnitsT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct IsAbsolute<Quantities<ValuesT_, UnitsT_>> {
  static constexpr bool value = IsAbsolute<UnitsT_>::value;
};
template <typename ValuesT_, typename UnitsT_>
struct MakeRelative<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<ValuesT_, typename MakeRelative<UnitsT_>::type>;
  static auto make(const Quantities<ValuesT_, UnitsT_>& object) { return type(object.getValues()); }
};
template <typename ValuesT_, typename UnitsT_>
struct MakeAbsolute<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<ValuesT_, typename MakeAbsolute<UnitsT_>::type>;
  static auto make(const Quantities<ValuesT_, UnitsT_>& object) { return type(object.getValues()); }
};

template <typename ValuesT_, typename UnitsT_>
struct Evaluate<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Evaluate<ValuesT_>::type, UnitsT_>;
};

template <typename ValuesT_, typename UnitsT_>
auto operator+(const Quantities<ValuesT_, UnitsT_>& rhs) {
  return rhs;
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator+(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Sum<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(lhs.getValues() + rhs.getValues());
}

template <typename ValuesT_, typename UnitsT_>
auto operator-(const Quantities<ValuesT_, UnitsT_>& rhs) {
  return Quantities<ValuesT_, UnitsT_>(-rhs.getValues());
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator-(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Difference<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(lhs.getValues() -
                                                                                                             rhs.getValues());
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  return lhs * Quantity<ValueRhsT_, typename AccessUnits<SpecializationDummy<ValueRhsT_>>::Dimensionless>(rhs);
}
template <typename ValueLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const ValueLhsT_ lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValues() * rhs.getValue());
}
template <typename ValueLhsT_, typename UnitLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return rhs * lhs;
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return
      typename Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(lhs.getValues() * rhs.getValues());
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  using Dimensionless = typename Identity<typename Element<UnitsLhsT_, 0, 0>::type>::type;
  return lhs / Quantity<ValueRhsT_, Dimensionless>(rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValues() / rhs.getValue());
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
bool operator==(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  static_assert(std::is_same<UnitsLhsT_, UnitsRhsT_>::value, "Units do not match.");
  return lhs.getValues() == rhs.getValues();
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
bool operator!=(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <typename ValuesT_, typename UnitsT_>
std::ostream& operator<<(std::ostream& lhs, const Quantities<ValuesT_, UnitsT_>& rhs) {
  lhs << rhs.getValues();
  return lhs;
}

}  // namespace wuzp
