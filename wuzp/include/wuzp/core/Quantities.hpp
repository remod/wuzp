#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

/**
 * Container to store quantities.
 * Every quantity consists of a value and a unit.
 * The values are evaluated at run-time, the units at compile-time.
 * @tparam ValuesT_ Container type to store the values (e.g. a scalar, vector, matrix, etc.).
 * @tparam UnitsT_ Container storing the units (e.g. a scalar, vector, matrix, etc.).
 */
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
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromSimilar>) Quantities(T0&& t0, Ts&&... ts)
      : Quantities(typename GetTag<ThisT, T0, Ts...>::type{}, std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return ThisT(ValuesT::zero()); }
  static ThisT random() { return ThisT(ValuesT::random()); }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  auto get() const {
    return get<0, 0>();
  }

  template <int RowId, int ColId, int Dimension = Dimension<ThisT>::value>
  auto get() const {
    return typename Element<ThisT, RowId, ColId>::type{values_.template get<RowId, ColId>()};
  }

  template <int Dimension = Dimension<ThisT>::value>
  auto get(const int rowId, const int colId) const {
    return typename GetType<ThisT>::type{values_.get(rowId, colId)};
  }

  template <typename ValueT_, typename UnitT_, int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  void set(const Quantity<ValueT_, UnitT_>& quantity) {
    return set<0, 0>(quantity);
  }

  template <int RowId, int ColId, typename ValueT_, typename UnitT_, int Dimension = Dimension<ThisT>::value>
  void set(const Quantity<ValueT_, UnitT_>& quantity) {
    static_assert(std::is_same<typename Element<UnitsT, RowId, ColId>::type, UnitT_>::value, "Units do not match.");
    values_.template set<RowId, ColId>(quantity.getValue());
  }

  template <typename ValueT_, typename UnitT_, int Dimension = Dimension<ThisT>::value>
  void set(const int rowId, const int colId, const Quantity<ValueT_, UnitT_>& quantity) {
    static_assert(IsHomogeneous<ThisT>::value, "Quantities are not homogeneous.");
    static_assert(std::is_same<typename Element<UnitsT, 0, 0>::type, UnitT_>::value, "Units do not match.");
    values_.set(rowId, colId, quantity.getValue());
  }

  /**
   * Set the values of the quantities.
   * WARNING: This method circumvents quantity type safety.
   * This method is provided even if the dimension is 0, to ensure all quantities have the same interface in templated functions.
   * @param values ValueMatrix.
   */
  template <typename ValuesOtherT_>
  void setValues(const ValuesOtherT_& values) {
    values_ = values;
  }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  void setValue(ValueT value) {
    values_.set(value);
  }

  /**
   * Get the values of the quantities.
   * WARNING: This method circumvents quantity type safety.
   * This method is provided even if the dimension is 0, to ensure all quantities have the same interface in templated functions.
   * @return ValueMatrix of the quantities.
   */
  const ValuesT& getValues() const { return values_; }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  ValueT getValue() const {
    return getValues().get();
  }

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

  auto transpose() const { return typename Transpose<ThisT>::type{values_.transpose()}; }

  // TODO(remod): Either a) remove methods, or b) remove free functions or c) make them re-use each other.
  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto tensorProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename TensorProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type{values_.tensorProduct(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto contraction(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename Contraction<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type{values_.contraction(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto dotProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename DotProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type{values_.dotProduct(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto crossProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename CrossProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type{values_.crossProduct(other.getValues())};
  }

  auto inverse() const { return typename Inverse<ThisT>::type{values_.inverse()}; }
  auto tensorInverse() const { return typename TensorInverse<ThisT>::type{values_.tensorInverse()}; }

  auto norm() const { return typename GetType<ThisT>::type{values_.norm()}; }

  auto squaredNorm() const { return typename Power<typename GetType<ThisT>::type, Integer<2>>::type{values_.squaredNorm()}; }

  /**
   * Get the determinant.
   * @tparam Dimension Do not use this.
   */
  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 2>>
  auto determinant() const {
    return typename GetType<ThisT>::type{values_.determinant()};
  }

  auto evaluate() const { return typename Evaluate<ThisT>::type{values_.evaluate()}; }

 private:
  template <typename ValuesOtherT_, typename UnitsOtherT_>
  // Allow other values types to support lazy evaluation.
  Quantities(FromSimilar /*tag*/, const Quantities<ValuesOtherT_, UnitsOtherT_>& other) : values_(other.getValues()) {
    static_assert(std::is_same<UnitsT, UnitsOtherT_>::value, "Units do not match.");
  }

  /**
   * Constructor from individual quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantities.
   * @param quantities Quantities.
   */
  template <typename... QuantityValueT_, typename... QuantityUnitsT_>
  explicit Quantities(FromImplementation /*tag*/, const Quantity<QuantityValueT_, QuantityUnitsT_>&... quantities)
      : Quantities(FromImplementationArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<QuantityUnitsT_...>, typename GetTypes<UnitsT>::type>::value, "Units do not match.");
  }

  template <typename... Ts>
  explicit Quantities(FromImplementationArgs /*tag*/, Ts&&... ts) : values_(std::forward<Ts>(ts)...) {}

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
          IsSpecialization<first, Quantities>::value, FromSimilar,
          typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromImplementation, FromImplementationArgs>::type>::type,
      typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromImplementation, FromImplementationArgs>::type>::type;
};

template <typename ValuesT_, typename UnitsT_>
struct GetType<Quantities<ValuesT_, UnitsT_>> {
  static_assert(IsHomogeneous<Quantities<ValuesT_, UnitsT_>>::value, "Quantities are not homogeneous.");
  using type =
      Quantity<typename AccessValues<AccessImplementation<ValuesT_>>::template Value<typename Quantities<ValuesT_, UnitsT_>::ValueT>,
               typename GetType<UnitsT_>::type>;
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

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename Product<ValuesLhsT_, ValuesRhsT_>::type, typename Product<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct TensorProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename TensorProduct<ValuesLhsT_, ValuesRhsT_>::type, typename TensorProduct<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Contraction<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename Contraction<ValuesLhsT_, ValuesRhsT_>::type, typename Contraction<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct DotProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename DotProduct<ValuesLhsT_, ValuesRhsT_>::type, typename DotProduct<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct CrossProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename CrossProduct<ValuesLhsT_, ValuesRhsT_>::type, typename CrossProduct<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type =
      Quantities<typename Division<ValuesLhsT_, typename ValueRhsT_::ValueT>::type, typename Division<UnitsLhsT_, UnitRhsT_>::type>;
};

template <typename ValueT_, typename UnitT_, typename ExponentT>
struct Power<Quantity<ValueT_, UnitT_>, ExponentT> {
  using type = Quantity<ValueT_, typename Power<UnitT_, ExponentT>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Transpose<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Transpose<ValuesT_>::type, typename Transpose<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int ElementRowId, int ElementColId>
struct Element<Quantities<ValuesT_, UnitsT_>, ElementRowId, ElementColId> {
  using type = Quantity<typename AccessValues<AccessImplementation<ValuesT_>>::template Value<typename ValuesT_::ValueT>,
                        typename Element<UnitsT_, ElementRowId, ElementColId>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Inverse<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Inverse<ValuesT_>::type, typename Inverse<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct TensorInverse<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename TensorInverse<ValuesT_>::type, typename TensorInverse<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<Quantities<ValuesT_, UnitsT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = Quantities<typename Block<ValuesT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                          typename Block<UnitsT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Evaluate<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Evaluate<ValuesT_>::type, UnitsT_>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto sum(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return
      typename Sum<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type{sum(lhs.getValues(), rhs.getValues())};
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator+(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return sum(lhs, rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto difference(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Difference<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(
      difference(lhs.getValues(), rhs.getValues()));
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator-(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return difference(lhs, rhs);
}

template <typename ValuesT_, typename UnitsT_>
auto negation(const Quantities<ValuesT_, UnitsT_>& rhs) {
  return Quantities<ValuesT_, UnitsT_>(negation(rhs.getValues()));
}
template <typename ValuesT_, typename UnitsT_>
auto operator-(const Quantities<ValuesT_, UnitsT_>& rhs) {
  return negation(rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  // Evaluate to avoid returning an expression on a temporary object.
  return product(lhs, Quantity<typename AccessValues<AccessImplementation<ValueRhsT_>>::template Value<ValueRhsT_>,
                               typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>(rhs))
      .evaluate();
}
template <typename ValueLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const ValueLhsT_ lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(rhs, lhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  // Evaluate to avoid returning an expression on a temporary object.
  return product(lhs, Quantities<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>,
                                 TypeMatrix<Rows<MatrixSizeRhsT_>::value, Cols<MatrixSizeRhsT_>::value,
                                            typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>>(rhs))
      .evaluate();
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(rhs, lhs);
}
// This specialization also covers the `Quantity` typedef.
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(
      product(lhs.getValues(), rhs.getValues()));
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  return product(lhs, rhs);
}
template <typename ValueLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const ValueLhsT_ lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(lhs, rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto tensorProduct(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename TensorProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(
      lhs.getValues().tensorProduct(rhs.getValues()));
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto contraction(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Contraction<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type(
      lhs.getValues().contraction(rhs.getValues()));
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto division(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  using Dimensionless = typename Identity<typename Element<UnitsLhsT_, 0, 0>::type>::type;
  return division(lhs, Quantity<typename AccessValues<AccessImplementation<ValueRhsT_>>::template Value<ValueRhsT_>, Dimensionless>(rhs));
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto division(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  using Dimensionless = typename Identity<typename Element<UnitsLhsT_, 0, 0>::type>::type;
  return division(lhs, Quantity<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, Dimensionless>(rhs));
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto division(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(
      division(lhs.getValues(), rhs.getValue()));
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  return division(lhs, rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return division(lhs, rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator/(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return division(lhs, rhs);
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

// TODO(remod): Implement for full matrices?
// Exponent first, as value and unit can be derived automatically.
template <typename ExponentT, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  // Evaluate the value here, as we need the result to have a constructor which can take a scalar, which expressions do not have.
  return typename Power<Quantity<typename Evaluate<Value>::type, Unit>, ExponentT>::type(
      std::pow(quantity.getValue(), ExponentT::template value<typename Value::ValueT>()));
}

// Convenience definition, so one can write pow<2>() to get pow<Integer<2>>().
template <int ExponentV, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  return pow<Integer<ExponentV>>(quantity);
}

// Convenience definition, so one can write pow<1, 2>() to get pow<Rational<1, 2>>().
template <int ExponentDividendV, int ExponentDivisorV, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  return pow<Rational<ExponentDividendV, ExponentDivisorV>>(quantity);
}

template <typename Value>
auto sin(const Quantity<Value, typename AccessUnits<AccessImplementation<Value>>::Angle>& angle) {
  return Quantity<Value, typename AccessUnits<AccessImplementation<Value>>::Dimensionless>(std::sin(angle.getValue()));
}

template <typename Value>
auto cos(const Quantity<Value, typename AccessUnits<AccessImplementation<Value>>::Angle>& angle) {
  return Quantity<Value, typename AccessUnits<AccessImplementation<Value>>::Dimensionless>(std::cos(angle.getValue()));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
