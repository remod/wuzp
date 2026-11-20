#pragma once

#include <algorithm>
#include <cmath>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/internal/powers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

namespace internal {

//! The pseudo-inverse of quantities with more columns than rows (1), more rows than columns (-1) or as many (0), see
//! Quantities::pseudoInverse(). These are specializations rather than `if constexpr`: MSVC compiles both branches of an
//! `if constexpr` in member functions of class templates, and the units of the other branches need not have products.
template <int ShapeV_>
struct PseudoInverse;

}  // namespace internal

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

  //! The quantity with the single index IdV_ of quantities of dimension 0 or 1: the row of a column, e.g. of a vector, the
  //! column of a row, e.g. of a covector, or 0 for a 1x1 matrix (see internal::vectorElementRowId()). Like
  //! get<RowIdV_, ColIdV_>(), it checks the index at compile time and has the unit of the element.
  template <int IdV_>
  auto get() const
    requires(Dimension<ThisT>::value <= 1)
  {
    return get<internal::vectorElementRowId<ThisT>(IdV_), internal::vectorElementColId<ThisT>(IdV_)>();
  }

  auto get(const int rowId, const int colId) const { return typename GetType<ThisT>::type{values_.get(rowId, colId)}; }

  //! The quantity with the single index id of quantities of dimension 0 or 1, like get<IdV_>(), but with an index known at run
  //! time, which, like get(rowId, colId), requires all elements to have the same unit.
  auto get(const int id) const
    requires(Dimension<ThisT>::value <= 1)
  {
    return get(internal::vectorElementRowId<ThisT>(id), internal::vectorElementColId<ThisT>(id));
  }

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

  //! Sets the quantity with the single index IdV_ of quantities of dimension 0 or 1 (see get<IdV_>()), like
  //! set<RowIdV_, ColIdV_>(), which checks the index and the unit at compile time.
  template <int IdV_, typename ValueT_, typename UnitT_>
  void set(const Quantity<ValueT_, UnitT_>& quantity)
    requires(Dimension<ThisT>::value <= 1)
  {
    set<internal::vectorElementRowId<ThisT>(IdV_), internal::vectorElementColId<ThisT>(IdV_)>(quantity);
  }

  template <typename ValueT_, typename UnitT_>
  void set(const int rowId, const int colId, const Quantity<ValueT_, UnitT_>& quantity) {
    static_assert(IsHomogeneous<ThisT>::value, "Quantities are not homogeneous.");
    static_assert(std::is_same<typename Element<UnitsT, 0, 0>::type, UnitT_>::value, "Units do not match.");
    values_.set(rowId, colId, quantity.getValue());
  }

  //! Sets the quantity with the single index id of quantities of dimension 0 or 1, like set<IdV_>(), but with an index known at
  //! run time, which, like set(rowId, colId, quantity), requires all elements to have the same unit.
  template <typename ValueT_, typename UnitT_>
  void set(const int id, const Quantity<ValueT_, UnitT_>& quantity)
    requires(Dimension<ThisT>::value <= 1)
  {
    set(internal::vectorElementRowId<ThisT>(id), internal::vectorElementColId<ThisT>(id), quantity);
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

  //! Sets the value of a quantity. Like ValuesWrapper::set(), it converts the value into the value type only if their common type
  //! is the value type (see isConvertedToValueType), e.g. not a double for int values, which could truncate it.
  template <typename ValueOtherT_>
  void setValue(ValueOtherT_ value)
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

  //! The matrix inverse of a square matrix, whose units must describe a linear map, M_ij = a_i / b_j: the units of the inverse are
  //! (M^-1)_ij = b_i / a_j (see Inverse<TypeMatrixImpl<...>> in Units.hpp).
  auto inverse() const { return typename Evaluate<typename Inverse<ThisT>::type>::type{values_.inverse()}; }

  /**
   * The Moore-Penrose pseudo-inverse A^+ of a matrix A of full rank: A^T (A A^T)^-1 if it has fewer rows than columns, i.e. full
   * row rank, e.g. the joint velocities q' = J^+ x' of the smallest norm for a redundant robot, (A^T A)^-1 A^T if it has more,
   * i.e. full column rank, the least squares solution, and the inverse A^-1 if it is square. Its units are those of an inverse,
   * (A^+)_ij = b_i / a_j for A_ij = a_i / b_j. A A^T and A^T A sum over the columns or the rows, so the columns of a matrix with
   * fewer rows must have the same unit, and the rows of a matrix with more rows, otherwise it fails with "Units do not match.":
   * the pseudo-inverse minimizes a Euclidean norm, which needs components with the same unit, and for other units it depends on
   * the choice of units (Doty, Melchiorri and Bonivento, "A theory of generalized inverses applied to robotics", The
   * International Journal of Robotics Research 12(1), 1993). Use a weighted pseudo-inverse for them. A matrix that does not have
   * full rank has no pseudo-inverse of this form, the inverse of A A^T or A^T A is not finite or huge, unlike kindr's pseudoInverse(),
   * which takes the singular value decomposition, which not every value matrix backend provides. It needs matrix inverses
   * (InvertibleValueMatrixBackend) and floating-point values.
   */
  auto pseudoInverse() const {
    constexpr int rows = Rows<ThisT>::value;
    constexpr int cols = Cols<ThisT>::value;
    // 1, -1 or 0 (see internal::PseudoInverse), as a difference rather than with conditional operators, whose branches the
    // compiler evaluates, so that it leaves no code that coverage reports as never executed.
    constexpr int shape = static_cast<int>(rows < cols) - static_cast<int>(rows > cols);
    return internal::PseudoInverse<shape>::value(*this);
  }

  //! The elementwise reciprocal, whose units are the reciprocals of the units.
  auto cwiseInverse() const { return typename Evaluate<typename CwiseInverse<ThisT>::type>::type{values_.cwiseInverse()}; }

  auto norm() const { return typename GetType<ThisT>::type{values_.norm()}; }

  auto squaredNorm() const { return typename Power<typename GetType<ThisT>::type, Integer<2>>::type{values_.squaredNorm()}; }

  //! Get the determinant of a square matrix, including a 1x1 matrix, whose units must describe a linear map. Its unit is the
  //! product of the units on the diagonal, e.g. u^n for a matrix with a single unit u (see Determinant<TypeMatrixImpl<...>>).
  auto determinant() const
    requires(Dimension<ThisT>::value != 1)
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
  using type = Quantity<ScalarValues<ValuesT_, typename ValuesT_::ValueT>, typename GetType<UnitsT_>::type>;
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
//! The elementwise quotient of quantities of the same size.
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Quantities<typename Division<ValuesLhsT_, ValuesRhsT_>::type, typename Division<UnitsLhsT_, UnitsRhsT_>::type>;
};

//! The elementwise power, like the product of quantities, which is elementwise too (see pow()).
template <typename ValuesT_, typename UnitsT_, typename ExponentT_>
struct Power<Quantities<ValuesT_, UnitsT_>, ExponentT_> {
  using type = Quantities<ValuesT_, typename Power<UnitsT_, ExponentT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Transpose<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename Transpose<ValuesT_>::type, typename Transpose<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_, int ElementRowIdV_, int ElementColIdV_>
struct Element<Quantities<ValuesT_, UnitsT_>, ElementRowIdV_, ElementColIdV_> {
  using type = Quantity<ScalarValues<ValuesT_, typename ValuesT_::ValueT>, typename Element<UnitsT_, ElementRowIdV_, ElementColIdV_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Inverse<Quantities<ValuesT_, UnitsT_>> {
  // The units first, which also check that the matrix is square.
  using UnitsT = typename Inverse<UnitsT_>::type;
  using type = Quantities<typename Inverse<ValuesT_>::type, UnitsT>;
};

template <typename ValuesT_, typename UnitsT_>
struct CwiseInverse<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantities<typename CwiseInverse<ValuesT_>::type, typename CwiseInverse<UnitsT_>::type>;
};

template <typename ValuesT_, typename UnitsT_>
struct Determinant<Quantities<ValuesT_, UnitsT_>> {
  using type = Quantity<ScalarValues<ValuesT_, typename ValuesT_::ValueT>, typename Determinant<UnitsT_>::type>;
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

namespace internal {

/**
 * An arithmetic scalar as a dimensionless quantity of the value matrix backend and the value type of ValuesT_, e.g. for 2 * v.
 * The scalar is converted into the value type if their common type (the type of their sum in C++) is the value type, e.g. an int
 * scalar with double values, but not a double scalar with int values, nor an unsigned scalar with int values, whose sum would
 * be unsigned (see isConvertedToValueType).
 */
template <typename ValuesT_, ArithmeticValue ScalarT_>
auto toDimensionlessQuantity(const ScalarT_ scalar) {
  using ValueT = typename ValuesT_::ValueT;
  static_assert(isConvertedToValueType<ScalarT_, ValueT>,
                "The common type of the scalar and the value type is not the value type, e.g. a double scalar with int values.");
  return Quantity<ScalarValues<ValuesT_, ValueT>, typename AccessUnits<AccessImplementation<ValueT>>::Dimensionless>(
      static_cast<ValueT>(scalar));
}

}  // namespace internal

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
  // The scalar has the value matrix backend and the value type of the quantities.
  return product(lhs, internal::toDimensionlessQuantity<ValuesLhsT_>(rhs));
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
//! The elementwise product of quantities of the same size, or with a quantity. Quantities have no coordinate system and no
//! indices, so this is the unrestricted counterpart of cwiseProduct() of tensors, e.g. for the components of points.
//! This specialization also covers the `Quantity` typedef.
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
auto innerProduct(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return lhs.innerProduct(rhs);
}

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto outerProduct(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return lhs.outerProduct(rhs);
}

namespace internal {

//! The pseudo-inverse A^T (A A^T)^-1 of a matrix with more columns than rows, of full row rank.
template <>
struct PseudoInverse<1> {
  template <typename QuantitiesT_>
  static auto value(const QuantitiesT_& matrix) {
    const auto transpose = matrix.transpose();
    return innerProduct(transpose, innerProduct(matrix, transpose).inverse());
  }
};

//! The pseudo-inverse (A^T A)^-1 A^T of a matrix with more rows than columns, of full column rank.
template <>
struct PseudoInverse<-1> {
  template <typename QuantitiesT_>
  static auto value(const QuantitiesT_& matrix) {
    const auto transpose = matrix.transpose();
    return innerProduct(innerProduct(transpose, matrix).inverse(), transpose);
  }
};

//! The pseudo-inverse of a square matrix of full rank, its inverse.
template <>
struct PseudoInverse<0> {
  template <typename QuantitiesT_>
  static auto value(const QuantitiesT_& matrix) {
    return matrix.inverse();
  }
};

}  // namespace internal

template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValueRhsT_>
auto division(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const ValueRhsT_ rhs) {
  // The scalar has the value matrix backend and the value type of the quantities.
  return division(lhs, internal::toDimensionlessQuantity<ValuesLhsT_>(rhs));
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
//! The elementwise quotient of quantities of the same size. Quantities have no coordinate system and no indices, so this is the
//! unrestricted counterpart of cwiseQuotient() of tensors, e.g. for the components of points.
template <typename ValuesLhsT_, typename UnitsLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto division(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs, const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<typename Division<Quantities<ValuesLhsT_, UnitsLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type(
      division(lhs.getValues(), rhs.getValues()));
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

// Functions of quantities. The functions of one argument, e.g. sin(), sqrt() and pow<P, Q>(), apply elementwise to quantities of
// any size, like the functions of Eigen's arrays and of std::valarray (https://en.cppreference.com/w/cpp/numeric/valarray), which
// is consistent with the elementwise product of quantities: e.g. exp() of a matrix is not its matrix exponential, and pow<2>() of
// a matrix squares every element and its unit. They also work with heterogeneous units, e.g. sqrt() of an area and a squared time. The
// functions of two arguments, atan2(), hypot(), min() and max(), take single quantities.

namespace internal {

//! The quantities with the units ResultUnitsT_ whose values are function(value) of the values of the quantities: the elementwise
//! functions of quantities. The value matrix backend applies the function with CwiseFunction, so the functions compute the same
//! with every backend. The result is converted into the value type, e.g. std::abs of a short, which is an int.
template <typename ResultUnitsT_, typename ValuesT_, typename UnitsT_, typename FunctionT_>
auto applyCwise(const Quantities<ValuesT_, UnitsT_>& quantities, const FunctionT_& function) {
  using ValueT = typename ValuesT_::ValueT;
  return Quantities<typename Evaluate<ValuesT_>::type, ResultUnitsT_>(
      quantities.getValues().cwiseFunction([&function](const ValueT value) { return static_cast<ValueT>(function(value)); }));
}

//! Rejects the integer values of sin, cos, tan, asin, acos, atan, atan2, exp and log, whose results are not integers in general,
//! and would be truncated: e.g. the sine of an angle, or an angle in radians (see "Integer values" in DESIGN.md). The value is
//! always true.
template <typename ValueT_>
struct RequireFloatingPoint {
  static_assert(std::is_floating_point_v<ValueT_>,
                "sin, cos, tan, asin, acos, atan, atan2, exp and log need floating-point values: their results are not integers in "
                "general.");
  static constexpr bool value = true;
};

//! A function of dimensionless quantities, sin, cos, tan, asin, acos, atan, exp or log, applied elementwise, whose results are
//! dimensionless too, e.g. the angles of asin(). Angles are dimensionless (see "Angles are dimensionless" in DESIGN.md), so the
//! functions take any dimensionless quantities, e.g. also ratios of lengths. The values must be floating point values.
template <typename ValuesT_, typename UnitsT_, typename FunctionT_>
auto dimensionlessFunction(const Quantities<ValuesT_, UnitsT_>& quantities, const FunctionT_& function) {
  using ValueT = typename ValuesT_::ValueT;
  using DimensionlessT =
      TypeMatrix<Rows<UnitsT_>::value, Cols<UnitsT_>::value, typename AccessUnits<AccessImplementation<ValueT>>::Dimensionless>;
  static_assert(std::is_same_v<UnitsT_, DimensionlessT>,
                "sin, cos, tan, asin, acos, atan, exp and log need dimensionless quantities, e.g. angles or ratios of lengths.");
  static_assert(RequireFloatingPoint<ValueT>::value);
  return applyCwise<UnitsT_>(quantities, function);
}

//! The quantity function(lhs value, rhs value) with the unit ResultUnitT_, for atan2, hypot, min and max: both quantities must have
//! the same unit, e.g. two lengths, and the same value type and value matrix backend, like the operands of a sum. The result is
//! converted into the value type, which leaves only the message of a rejected function, e.g. of atan2() of int values, whose
//! std::atan2 is a double.
template <typename ResultUnitT_, typename ValuesLhsT_, typename UnitLhsT_, typename ValuesRhsT_, typename UnitRhsT_, typename FunctionT_>
auto binaryFunction(const Quantity<ValuesLhsT_, UnitLhsT_>& lhs, const Quantity<ValuesRhsT_, UnitRhsT_>& rhs, const FunctionT_& function) {
  using ValuesT = typename Evaluate<ValuesLhsT_>::type;
  static_assert(std::is_same_v<UnitLhsT_, UnitRhsT_>, "atan2, hypot, min and max need quantities with the same unit.");
  static_assert(std::is_same_v<ValuesT, typename Evaluate<ValuesRhsT_>::type>,
                "atan2, hypot, min and max need quantities with the same value type and value matrix backend.");
  return Quantity<ValuesT, ResultUnitT_>(static_cast<typename ValuesT::ValueT>(function(lhs.getValue(), rhs.getValue())));
}

//! std::min and std::max as function objects, for the two overloads of min() and max() each.
struct Min {
  template <typename ValueT_>
  ValueT_ operator()(const ValueT_ lhs, const ValueT_ rhs) const {
    return std::min(lhs, rhs);
  }
};
struct Max {
  template <typename ValueT_>
  ValueT_ operator()(const ValueT_ lhs, const ValueT_ rhs) const {
    return std::max(lhs, rhs);
  }
};

}  // namespace internal

/**
 * The quantities raised to a rational power P/Q, elementwise, e.g. pow<Rational<1, 2>>(area) is a length, and pow<2>() of a
 * length and a time is an area and a squared time.
 * The power of floating point values is computed in their type, with std::sqrt for 1/2 and std::cbrt for 1/3. For integer
 * values, a result that is not an integer is truncated, like the norm of integer vectors, which follows Eigen's norm():
 * pow<1, 2>(QuantityI<Area>(5)) is 2 m (see "Integer values" in DESIGN.md). The truncation is exact, e.g. pow<2, 3>() of 8 m^3 is
 * 4 m^2, which is 3.9999999999999996 with std::pow (see internal::integerPower). A negative value has a real power if the reduced
 * denominator Q is odd, (-1)^P |x|^(P/Q): e.g. pow<1, 3>() of -8 m^3 is -2 m, like std::cbrt, where std::pow is NaN. If Q is
 * even, it has none: floating point values give NaN, like std::pow (https://en.cppreference.com/w/cpp/numeric/math/pow#Error_handling),
 * and integer values throw std::domain_error (see "Powers of negative quantities" in DESIGN.md).
 * The exponent comes first, as the values and the units can be deduced.
 * @throws std::domain_error For a negative integer value and an even Q, and for an integer zero and a negative P.
 * @throws std::range_error For an integer power that does not fit into the value type.
 */
template <typename ExponentT_, typename ValuesT_, typename UnitsT_>
auto pow(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return internal::applyCwise<typename Power<UnitsT_, ExponentT_>::type>(
      quantities, [](const auto value) { return internal::realPower<ExponentT_>(value); });
}

// Convenience definition, so one can write pow<2>() to get pow<Integer<2>>().
template <int ExponentV_, typename ValuesT_, typename UnitsT_>
auto pow(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return pow<Integer<ExponentV_>>(quantities);
}

// Convenience definition, so one can write pow<1, 2>() to get pow<Rational<1, 2>>().
template <int ExponentDividendV_, int ExponentDivisorV_, typename ValuesT_, typename UnitsT_>
auto pow(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return pow<Rational<ExponentDividendV_, ExponentDivisorV_>>(quantities);
}

//! The square root, elementwise: pow<1, 2>(), computed with std::sqrt (https://en.cppreference.com/w/cpp/numeric/math/sqrt), e.g.
//! the square root of an area is a length. The square root of a negative value is NaN for floating point values, like std::sqrt,
//! and throws std::domain_error for integer values; that of other integer values is truncated (see pow()).
//! @throws std::domain_error For a negative integer value.
template <typename ValuesT_, typename UnitsT_>
auto sqrt(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return pow<1, 2>(quantities);
}

//! The cube root, elementwise: pow<1, 3>(), computed with std::cbrt (https://en.cppreference.com/w/cpp/numeric/math/cbrt), e.g.
//! the cube root of a volume is a length. The cube root of a negative value is negative, like std::cbrt; that of an integer value
//! is truncated (see pow()).
template <typename ValuesT_, typename UnitsT_>
auto cbrt(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return pow<1, 3>(quantities);
}

//! The absolute value, elementwise, of the same unit, like std::abs (https://en.cppreference.com/w/cpp/numeric/math/fabs), and the
//! values themselves for unsigned values, for which std::abs is ambiguous. Like with std::abs, the absolute value of the minimum
//! of a signed integer type, e.g. INT_MIN, is undefined behavior: the type cannot represent it.
template <typename ValuesT_, typename UnitsT_>
auto abs(const Quantities<ValuesT_, UnitsT_>& quantities) {
  using ValueT = typename ValuesT_::ValueT;
  return internal::applyCwise<UnitsT_>(quantities, [](const ValueT value) {
    if constexpr (std::is_unsigned_v<ValueT>) {
      return value;
    } else {
      return std::abs(value);
    }
  });
}

//! The sine of angles in radians, elementwise, like std::sin (https://en.cppreference.com/w/cpp/numeric/math/sin): dimensionless
//! quantities. The angles can be any dimensionless quantities (see "Angles are dimensionless" in DESIGN.md).
template <typename ValuesT_, typename UnitsT_>
auto sin(const Quantities<ValuesT_, UnitsT_>& angles) {
  return internal::dimensionlessFunction(angles, [](const auto value) { return std::sin(value); });
}

//! The cosine of angles in radians, elementwise, like std::cos (https://en.cppreference.com/w/cpp/numeric/math/cos), see sin().
template <typename ValuesT_, typename UnitsT_>
auto cos(const Quantities<ValuesT_, UnitsT_>& angles) {
  return internal::dimensionlessFunction(angles, [](const auto value) { return std::cos(value); });
}

//! The tangent of angles in radians, elementwise, like std::tan (https://en.cppreference.com/w/cpp/numeric/math/tan), see sin().
template <typename ValuesT_, typename UnitsT_>
auto tan(const Quantities<ValuesT_, UnitsT_>& angles) {
  return internal::dimensionlessFunction(angles, [](const auto value) { return std::tan(value); });
}

//! The arc sine of dimensionless quantities, elementwise, like std::asin (https://en.cppreference.com/w/cpp/numeric/math/asin):
//! the angles in [-pi/2, pi/2] in radians, which are dimensionless (see "Angles are dimensionless" in DESIGN.md). Values outside
//! [-1, 1] give NaN.
template <typename ValuesT_, typename UnitsT_>
auto asin(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return internal::dimensionlessFunction(quantities, [](const auto value) { return std::asin(value); });
}

//! The arc cosine of dimensionless quantities, elementwise, like std::acos (https://en.cppreference.com/w/cpp/numeric/math/acos):
//! the angles in [0, pi] in radians, see asin().
template <typename ValuesT_, typename UnitsT_>
auto acos(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return internal::dimensionlessFunction(quantities, [](const auto value) { return std::acos(value); });
}

//! The arc tangent of dimensionless quantities, elementwise, like std::atan (https://en.cppreference.com/w/cpp/numeric/math/atan):
//! the angles in [-pi/2, pi/2] in radians, see asin(). atan2() takes the quadrant into account.
template <typename ValuesT_, typename UnitsT_>
auto atan(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return internal::dimensionlessFunction(quantities, [](const auto value) { return std::atan(value); });
}

//! The exponential function e^x of dimensionless quantities, elementwise, like std::exp
//! (https://en.cppreference.com/w/cpp/numeric/math/exp): not the matrix exponential of a matrix.
template <typename ValuesT_, typename UnitsT_>
auto exp(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return internal::dimensionlessFunction(quantities, [](const auto value) { return std::exp(value); });
}

//! The natural logarithm of dimensionless quantities, elementwise, like std::log (https://en.cppreference.com/w/cpp/numeric/math/log):
//! NaN for negative values, and -infinity for zero.
template <typename ValuesT_, typename UnitsT_>
auto log(const Quantities<ValuesT_, UnitsT_>& quantities) {
  return internal::dimensionlessFunction(quantities, [](const auto value) { return std::log(value); });
}

/**
 * The angle of the point (x, y) from the positive x axis, in [-pi, pi] in radians, of two quantities with the same unit, e.g.
 * lengths, like std::atan2 (https://en.cppreference.com/w/cpp/numeric/math/atan2): y comes first, the angle is counterclockwise
 * from the x axis to the y axis, and its branch cut is the negative x axis, where it is pi for y = +0 and -pi for y = -0. The
 * angle is dimensionless (see "Angles are dimensionless" in DESIGN.md). The values must be floating point values.
 */
template <typename ValuesYT_, typename UnitYT_, typename ValuesXT_, typename UnitXT_>
auto atan2(const Quantity<ValuesYT_, UnitYT_>& y, const Quantity<ValuesXT_, UnitXT_>& x) {
  using ValueT = typename ValuesYT_::ValueT;
  static_assert(internal::RequireFloatingPoint<ValueT>::value);
  return internal::binaryFunction<typename AccessUnits<AccessImplementation<ValueT>>::Angle>(
      y, x, [](const auto yValue, const auto xValue) { return std::atan2(yValue, xValue); });
}

//! The length sqrt(x^2 + y^2) of two quantities with the same unit, e.g. the components of a vector, without overflow or
//! underflow in the intermediate steps, like std::hypot (https://en.cppreference.com/w/cpp/numeric/math/hypot). For integer
//! values, it is truncated exactly, like the powers (see internal::realHypot and "Integer values" in DESIGN.md).
//! @throws std::range_error For integer values whose length does not fit into the value type.
template <typename ValuesXT_, typename UnitXT_, typename ValuesYT_, typename UnitYT_>
auto hypot(const Quantity<ValuesXT_, UnitXT_>& x, const Quantity<ValuesYT_, UnitYT_>& y) {
  return internal::binaryFunction<UnitXT_>(x, y, [](const auto xValue, const auto yValue) { return internal::realHypot(xValue, yValue); });
}

//! The smaller of two quantities with the same unit, like std::min (https://en.cppreference.com/w/cpp/algorithm/min), which
//! returns lhs if neither is smaller, e.g. if one is NaN.
template <typename ValuesLhsT_, typename UnitLhsT_, typename ValuesRhsT_, typename UnitRhsT_>
auto min(const Quantity<ValuesLhsT_, UnitLhsT_>& lhs, const Quantity<ValuesRhsT_, UnitRhsT_>& rhs) {
  return internal::binaryFunction<UnitLhsT_>(lhs, rhs, internal::Min{});
}
//! min() of two quantities of the same type. Argument-dependent lookup also finds std::min(const T&, const T&) for them, e.g.
//! through the std::tuple of their units, which would make the overload above ambiguous: this one is more specialized than both.
template <typename ValuesT_, typename UnitT_>
auto min(const Quantity<ValuesT_, UnitT_>& lhs, const Quantity<ValuesT_, UnitT_>& rhs) {
  return internal::binaryFunction<UnitT_>(lhs, rhs, internal::Min{});
}

//! The larger of two quantities with the same unit, like std::max (https://en.cppreference.com/w/cpp/algorithm/max), which
//! returns lhs if neither is larger, e.g. if one is NaN.
template <typename ValuesLhsT_, typename UnitLhsT_, typename ValuesRhsT_, typename UnitRhsT_>
auto max(const Quantity<ValuesLhsT_, UnitLhsT_>& lhs, const Quantity<ValuesRhsT_, UnitRhsT_>& rhs) {
  return internal::binaryFunction<UnitLhsT_>(lhs, rhs, internal::Max{});
}
//! max() of two quantities of the same type, see min().
template <typename ValuesT_, typename UnitT_>
auto max(const Quantity<ValuesT_, UnitT_>& lhs, const Quantity<ValuesT_, UnitT_>& rhs) {
  return internal::binaryFunction<UnitT_>(lhs, rhs, internal::Max{});
}

}  // namespace wuzp
