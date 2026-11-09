#pragma once

#include <cmath>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

/**
 * Container to store quantities.
 * Every quantity consists of a value and a unit.
 * The values are evaluated at run-time, the units at compile-time.
 * @tparam ValuesT_ Container type to store the values (e.g. a scalar, vector, matrix, etc.).
 * @tparam UnitsT_ Container storing the units (e.g. a scalar, vector, matrix, etc.).
 */
template <AnyValuesWrapper ValuesT_, AnyUnitMatrix UnitsT_>
class Quantities {
 public:
  using ValuesT = ValuesT_;
  using UnitsT = UnitsT_;
  using ValueT = typename ValuesT::ValueT;
  using ThisT = Quantities<ValuesT, UnitsT>;

  static_assert(Rows<ValuesT>::value == Rows<UnitsT>::value, "Number of rows do not match.");
  static_assert(Cols<ValuesT>::value == Cols<UnitsT>::value, "Number of columns do not match.");

  Quantities() = default;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename T0, typename... Ts>
    requires(!IsSpecialization<T0, Tag>::value && !CopiesOrMoves<Quantities, T0, Ts...>)
  // Allow implicit construction from similar quantity matrices with the same units. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!IsValidConversion<ThisT, std::remove_cvref_t<T0>, std::remove_cvref_t<Ts>...>::value) Quantities(T0&& t0, Ts&&... ts)
      : Quantities(typename GetTag<ThisT, T0, Ts...>::type{}, std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return ThisT(ValuesT::zero()); }
  static ThisT random() { return ThisT(ValuesT::random()); }

  auto get() const
    requires(Dimension<ThisT>::value == 0)
  {
    return get<0, 0>();
  }

  template <int RowIdV_, int ColIdV_>
  auto get() const {
    return typename Element<ThisT, RowIdV_, ColIdV_>::type{values_.template get<RowIdV_, ColIdV_>()};
  }

  auto get(const int rowId, const int colId) const { return typename GetType<ThisT>::type{values_.get(rowId, colId)}; }

  template <typename ValueT_, typename UnitT_>
  void set(const Quantity<ValueT_, UnitT_>& quantity)
    requires(Dimension<ThisT>::value == 0)
  {
    return set<0, 0>(quantity);
  }

  template <int RowIdV_, int ColIdV_, typename ValueT_, typename UnitT_>
  void set(const Quantity<ValueT_, UnitT_>& quantity) {
    static_assert(std::is_same<typename Element<UnitsT, RowIdV_, ColIdV_>::type, UnitT_>::value, "Units do not match.");
    values_.template set<RowIdV_, ColIdV_>(quantity.getValue());
  }

  template <typename ValueT_, typename UnitT_>
  void set(const int rowId, const int colId, const Quantity<ValueT_, UnitT_>& quantity) {
    static_assert(IsHomogeneous<ThisT>::value, "Quantities are not homogeneous.");
    static_assert(std::is_same<typename Element<UnitsT, 0, 0>::type, UnitT_>::value, "Units do not match.");
    values_.set(rowId, colId, quantity.getValue());
  }

  /**
   * Set the values of the quantities.
   * WARNING: This method circumvents quantity type safety.
   * This method is provided even if the dimension is 0, to ensure all quantities have the same interface in templated functions.
   * @param values ValueMatrix of the same size.
   */
  template <typename ValuesOtherT_>
  void setValues(const ValuesOtherT_& values) {
    // Direct initialization checks the size, see the converting constructor of ValuesWrapper.
    values_ = ValuesT(values);
  }

  void setValue(ValueT value)
    requires(Dimension<ThisT>::value == 0)
  {
    values_.set(value);
  }

  /**
   * Get the values of the quantities.
   * WARNING: This method circumvents quantity type safety.
   * This method is provided even if the dimension is 0, to ensure all quantities have the same interface in templated functions.
   * @return ValueMatrix of the quantities.
   */
  const ValuesT& getValues() const& { return values_; }
  //! The values of a temporary are moved out of it, which avoids a copy and does not refer to the destroyed temporary.
  ValuesT getValues() && { return std::move(values_); }

  ValueT getValue() const
    requires(Dimension<ThisT>::value == 0)
  {
    return getValues().get();
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  bool isApprox(const Quantities<ValuesOtherT_, UnitsOtherT_>& other, const ValueT precision = Precision<ValueT>::value()) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitsT, UnitsOtherT_>::value, "Units do not match.");
    return values_.isApprox(other.getValues(), precision);
  }

  // The member functions and operators return evaluated results (see ValuesWrapper), of the evaluated types of the traits.

  template <int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
  auto block() const {
    return typename Evaluate<typename Block<ThisT, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type>::type(
        values_.template getBlock<BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>());
  }

  auto transpose() const { return typename Evaluate<typename Transpose<ThisT>::type>::type{values_.transpose()}; }

  // TODO(remod): Either a) remove methods, or b) remove free functions or c) make them re-use each other.
  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto tensorProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename Evaluate<typename TensorProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type>::type{
        values_.tensorProduct(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto innerProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename Evaluate<typename InnerProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type>::type{
        values_.innerProduct(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto outerProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename Evaluate<typename OuterProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type>::type{
        values_.outerProduct(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto dotProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename Evaluate<typename DotProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type>::type{
        values_.dotProduct(other.getValues())};
  }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  auto crossProduct(const Quantities<ValuesOtherT_, UnitsOtherT_>& other) const {
    return typename Evaluate<typename CrossProduct<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::type>::type{
        values_.crossProduct(other.getValues())};
  }

  auto inverse() const { return typename Evaluate<typename Inverse<ThisT>::type>::type{values_.inverse()}; }
  auto tensorInverse() const { return typename Evaluate<typename TensorInverse<ThisT>::type>::type{values_.tensorInverse()}; }

  auto norm() const { return typename GetType<ThisT>::type{values_.norm()}; }

  auto squaredNorm() const { return typename Power<typename GetType<ThisT>::type, Integer<2>>::type{values_.squaredNorm()}; }

  //! Get the determinant. Its unit is the product of the units on the diagonal, e.g. u^n for a matrix with a single unit u.
  auto determinant() const
    requires(Dimension<ThisT>::value == 2)
  {
    return typename Determinant<ThisT>::type{values_.determinant()};
  }

  auto evaluate() const& { return typename Evaluate<ThisT>::type{values_.evaluate()}; }
  auto evaluate() && { return typename Evaluate<ThisT>::type{std::move(values_).evaluate()}; }

 private:
  template <typename ValuesOtherT_, typename UnitsOtherT_>
  // Allow other values types to support lazy evaluation.
  Quantities(FromSimilar /*tag*/, const Quantities<ValuesOtherT_, UnitsOtherT_>& other) : values_(other.getValues()) {
    static_assert(IsValidConversion<ThisT, Quantities<ValuesOtherT_, UnitsOtherT_>>::UnitsMatchV, "Units do not match.");
  }

  /**
   * Constructor from individual quantities.
   * @tparam QuantityUnitsT_ Units of the quantities, order and types must correspond to the units of the quantities.
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

template <typename ValuesT_, typename UnitsT_, typename ValuesOtherT_, typename UnitsOtherT_>
struct IsValidConversion<Quantities<ValuesT_, UnitsT_>, Quantities<ValuesOtherT_, UnitsOtherT_>> {
  // The converting constructor checks this with a static_assert, which explains why a conversion fails. The value matrices
  // have the sizes of the unit matrices, so they match if the units match.
  static constexpr bool UnitsMatchV = std::is_same_v<UnitsT_, UnitsOtherT_>;
  static constexpr bool value = UnitsMatchV;
};

template <typename ValuesT_, typename UnitsT_, typename... Ts>
struct GetTag<Quantities<ValuesT_, UnitsT_>, Ts...> {
 private:
  template <typename T>
  struct IsQuantity : std::false_type {};
  template <typename ValueT_, typename UnitT_>
  struct IsQuantity<Quantity<ValueT_, UnitT_>> : std::true_type {};
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<
          AnyQuantities<first>, FromSimilar,
          typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromImplementation, FromImplementationArgs>::type>::type,
      typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromImplementation, FromImplementationArgs>::type>::type;
};

// Quantities are homogeneous if all elements have the same unit.
template <typename ValuesT_, typename UnitsT_>
struct IsHomogeneous<Quantities<ValuesT_, UnitsT_>> {
  static constexpr bool value = IsHomogeneous<UnitsT_>::value;
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

template <typename ValuesT_, typename UnitsT_>
struct Negation<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Negation<ValuesT_>::type, UnitsT_>;
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
struct InnerProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename InnerProduct<ValuesLhsT_, ValuesRhsT_>::type, typename InnerProduct<UnitsLhsT_, UnitsRhsT_>::type>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct OuterProduct<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename OuterProduct<ValuesLhsT_, ValuesRhsT_>::type, typename OuterProduct<UnitsLhsT_, UnitsRhsT_>::type>;
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

template <typename ValueT_, typename UnitT_, typename ExponentT_>
struct Power<Quantity<ValueT_, UnitT_>, ExponentT_> {
  using type = Quantity<ValueT_, typename Power<UnitT_, ExponentT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Transpose<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Transpose<ValuesT_>::type, typename Transpose<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int ElementRowIdV_, int ElementColIdV_>
struct Element<Quantities<ValuesT_, UnitsT_>, ElementRowIdV_, ElementColIdV_> {
  using type = Quantity<typename AccessValues<AccessImplementation<ValuesT_>>::template Value<typename ValuesT_::ValueT>,
                        typename Element<UnitsT_, ElementRowIdV_, ElementColIdV_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Inverse<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Inverse<ValuesT_>::type, typename Inverse<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct TensorInverse<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename TensorInverse<ValuesT_>::type, typename TensorInverse<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Determinant<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantity<typename AccessValues<AccessImplementation<ValuesT_>>::template Value<typename ValuesT_::ValueT>,
                        typename Determinant<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<Quantities<ValuesT_, UnitsT_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  using type = Quantities<typename Block<ValuesT_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type,
                          typename Block<UnitsT_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Evaluate<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Evaluate<ValuesT_>::type, UnitsT_>;
};

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto sum(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<typename Sum<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type{
      sum(lhs.getValues(), rhs.getValues())};
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator+(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return sum(lhs, rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto difference(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<typename Difference<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type(
      difference(lhs.getValues(), rhs.getValues()));
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator-(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return difference(lhs, rhs);
}

template <typename ValuesT_, typename UnitsT_>
auto negation(const Quantities<ValuesT_, UnitsT_>& rhs) {
  return typename Evaluate<typename Negation<Quantities<ValuesT_, UnitsT_>>::type>::type(negation(rhs.getValues()));
}
template <typename ValuesT_, typename UnitsT_>
auto operator-(const Quantities<ValuesT_, UnitsT_>& rhs) {
  return negation(rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  return product(lhs, Quantity<typename AccessValues<AccessImplementation<ValueRhsT_>>::template Value<ValueRhsT_>,
                               typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>(rhs));
}
template <typename ValueLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const ValueLhsT_ lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(rhs, lhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, Quantities<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>,
                                 TypeMatrix<Rows<MatrixSizeRhsT_>::value, Cols<MatrixSizeRhsT_>::value,
                                            typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>>(rhs));
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(rhs, lhs);
}
// This specialization also covers the `Quantity` typedef.
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<typename Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type(
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
  return lhs.tensorProduct(rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto innerProduct(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return lhs.innerProduct(rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto outerProduct(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return lhs.outerProduct(rhs);
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
  return typename Evaluate<typename Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type>::type(
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
/**
 * The quantity raised to a rational power, e.g. pow<Rational<1, 2>>(area) is a length.
 * The power is computed in floating point and converted to the value type. For integer values, a result that is not an integer
 * is truncated, like the norm of integer vectors, which follows Eigen's norm(): pow<1, 2>(QuantityI<Area>(5)) is 2 m (see
 * "Integer values" in DESIGN.md).
 * The exponent comes first, as the value and the unit can be deduced.
 */
template <typename ExponentT_, typename ValueT_, typename UnitT_>
auto pow(const Quantity<ValueT_, UnitT_>& quantity) {
  using ValueT = typename ValueT_::ValueT;
  // An exponent of an integer type would truncate a non-integer exponent, e.g. 1/2 to 0.
  using ExponentValueT = std::conditional_t<std::is_floating_point_v<ValueT>, ValueT, double>;
  // Evaluate the value here, as we need the result to have a constructor which can take a scalar, which expressions do not have.
  return typename Power<Quantity<typename Evaluate<ValueT_>::type, UnitT_>, ExponentT_>::type(
      static_cast<ValueT>(std::pow(quantity.getValue(), ExponentT_::template value<ExponentValueT>())));
}

// Convenience definition, so one can write pow<2>() to get pow<Integer<2>>().
template <int ExponentV_, typename ValueT_, typename UnitT_>
auto pow(const Quantity<ValueT_, UnitT_>& quantity) {
  return pow<Integer<ExponentV_>>(quantity);
}

// Convenience definition, so one can write pow<1, 2>() to get pow<Rational<1, 2>>().
template <int ExponentDividendV_, int ExponentDivisorV_, typename ValueT_, typename UnitT_>
auto pow(const Quantity<ValueT_, UnitT_>& quantity) {
  return pow<Rational<ExponentDividendV_, ExponentDivisorV_>>(quantity);
}

// The results of sin and cos are constructed from a scalar, so they have the evaluated value type: an expression, e.g. of the
// sum of two angles, has no constructor from a scalar.
template <typename ValueT_>
auto sin(const Quantity<ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Angle>& angle) {
  return Quantity<typename Evaluate<ValueT_>::type, typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>(
      std::sin(angle.getValue()));
}

template <typename ValueT_>
auto cos(const Quantity<ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Angle>& angle) {
  return Quantity<typename Evaluate<ValueT_>::type, typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>(
      std::cos(angle.getValue()));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
