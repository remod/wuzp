#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/helpers.hpp"

namespace wuzp {

// Some operations (sums, differences, products, divisions) require the corresponding trait to be defined for an arbitrary number of
// arguments.
#define WUZP_DEFINE_RECURSIVE_TRAIT_IMPL(Name, NameStr)                                                             \
  template <typename... Ts>                                                                                         \
  struct Name;                                                                                                      \
  template <typename T0>                                                                                            \
  struct Name<T0> {                                                                                                 \
    using type = T0;                                                                                                \
  };                                                                                                                \
  template <typename T0, typename T1>                                                                               \
  struct Name<T0, T1> {                                                                                             \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                                                \
    static_assert(FailOnInstantiation<Name<T0, T1>>::value, "'" NameStr "' trait not specialized for given type."); \
  };                                                                                                                \
  template <typename T0, typename T1, typename T2, typename... Ts>                                                  \
  struct Name<T0, T1, T2, Ts...> {                                                                                  \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                                                \
    using type = typename Name<typename Name<T0, T1>::type, T2, Ts...>::type;                                       \
  };
#define WUZP_DEFINE_RECURSIVE_TRAIT(Name) WUZP_DEFINE_RECURSIVE_TRAIT_IMPL(Name, #Name)

template <typename Label>
struct Tag {};
template <typename T, typename... Args>
struct GetTag;

struct FromSimilarLabel {};
using FromSimilar = Tag<FromSimilarLabel>;
struct FromImplementationLabel {};
using FromImplementation = Tag<FromImplementationLabel>;
struct FromImplementationArgsLabel {};
using FromImplementationArgs = Tag<FromImplementationArgsLabel>;

template <typename T>
struct GetTypes {
  static_assert(FailOnInstantiation<GetTypes<T>>::value, "'GetTypes' trait not specialized for given type.");
};

template <typename T>
struct GetType {
  static_assert(FailOnInstantiation<GetType<T>>::value, "'GetType' trait not specialized for given type.");
};

template <typename T>
struct GetValue {
  static_assert(FailOnInstantiation<GetValue<T>>::value, "'GetValue' trait not specialized for given type.");
};

template <typename T>
struct Rows {
  static_assert(FailOnInstantiation<Rows<T>>::value, "'Rows' trait not specialized for given type.");
};

template <typename T>
struct Cols {
  static_assert(FailOnInstantiation<Cols<T>>::value, "'Cols' trait not specialized for given type.");
};

template <typename T>
struct Dimension {
  static constexpr int value = static_cast<int>(Rows<T>::value > 1) + static_cast<int>(Cols<T>::value > 1);
};

template <typename Lhs, typename Rhs>
struct Comparison {
  static_assert(FailOnInstantiation<Comparison<Lhs, Rhs>>::value, "'Comparison' trait not specialized for given type.");
};

//! The sum is performed elementwise.
WUZP_DEFINE_RECURSIVE_TRAIT(Sum)
template <typename Lhs, typename Rhs>
struct Sum<ValueType<Lhs>, ValueType<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Value types mismatch.");
  using type = Lhs;
};
template <typename Lhs, typename Rhs>
auto sum(const Lhs& lhs, const Rhs& rhs);

//! The difference is performed elementwise.
WUZP_DEFINE_RECURSIVE_TRAIT(Difference)
template <typename Lhs, typename Rhs>
struct Difference<ValueType<Lhs>, ValueType<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Value types mismatch.");
  using type = Lhs;
};
// Using `auto` as return type results in compiler error: function is used before deduction of `auto`.
template <typename Lhs, typename Rhs>
auto difference(const Lhs& lhs, const Rhs& rhs);

//! The negation is performed elementwise.
template <typename Rhs>
struct Negation {
  static_assert(FailOnInstantiation<Negation<Rhs>>::value, "'Negation' trait not specialized for given type.");
};
template <typename Rhs>
auto negation(const Rhs& rhs);

//! The product is performed elementwise.
WUZP_DEFINE_RECURSIVE_TRAIT(Product)
template <typename Lhs, typename Rhs>
struct Product<ValueType<Lhs>, ValueType<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Value types mismatch.");
  using type = Lhs;
};
template <typename Lhs, typename Rhs>
auto product(const Lhs& lhs, const Rhs& rhs);

WUZP_DEFINE_RECURSIVE_TRAIT(InnerProduct)
template <typename Lhs, typename Rhs>
auto innerProduct(const Lhs& lhs, const Rhs& rhs);

WUZP_DEFINE_RECURSIVE_TRAIT(OuterProduct)
template <typename Lhs, typename Rhs>
auto outerProduct(const Lhs& lhs, const Rhs& rhs);

template <typename Lhs, typename Rhs>
struct DotProduct {
  static_assert(FailOnInstantiation<DotProduct<Lhs, Rhs>>::value, "'DotProduct' trait not specialized for given type.");
};
template <typename Lhs, typename Rhs>
auto dotProduct(const Lhs& lhs, const Rhs& rhs);

template <typename Lhs, typename Rhs>
struct CrossProduct {
  static_assert(FailOnInstantiation<CrossProduct<Lhs, Rhs>>::value, "'CrossProduct' trait not specialized for given type.");
};
template <typename Lhs, typename Rhs>
auto crossProduct(const Lhs& lhs, const Rhs& rhs);

//! The division is performed elementwise.
WUZP_DEFINE_RECURSIVE_TRAIT(Division)
template <typename Lhs, typename Rhs>
struct Division<ValueType<Lhs>, ValueType<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Value types mismatch.");
  using type = Lhs;
};
template <typename Lhs, typename Rhs>
auto division(const Lhs& lhs, const Rhs& rhs);

template <typename T, typename ExponentT_>
struct Power {
  static_assert(FailOnInstantiation<Power<T, ExponentT_>>::value, "'Power' trait not specialized for given type.");
};

template <typename T>
struct Transpose {
  static_assert(FailOnInstantiation<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
};

template <typename Object, int ElementRowIdV_, int ElementColIdV_>
struct Element {
  static_assert(FailOnInstantiation<Element<Object, ElementRowIdV_, ElementColIdV_>>::value,
                "'Element' trait not specialized for given type.");
};

template <typename Object, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block {
  static_assert(FailOnInstantiation<Block<Object, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>>::value,
                "'Block' trait not specialized for given type.");
};

template <typename T>
struct Norm {
  static_assert(FailOnInstantiation<Norm<T>>::value, "'Norm' trait not specialized for given type.");
};

template <typename T>
struct SquaredNorm {
  static_assert(FailOnInstantiation<SquaredNorm<T>>::value, "'SquaredNorm' trait not specialized for given type.");
};

template <typename T>
struct Determinant {
  static_assert(FailOnInstantiation<Determinant<T>>::value, "'Determinant' trait not specialized for given type.");
};

template <typename T>
struct Inverse {
  static_assert(FailOnInstantiation<Inverse<T>>::value, "'Inverse' trait not specialized for given type.");
};

template <typename T>
struct TensorInverse {
  static_assert(FailOnInstantiation<TensorInverse<T>>::value, "'TensorInverse' trait not specialized for given type.");
};

template <typename T>
struct Identity {
  static_assert(FailOnInstantiation<Identity<T>>::value, "'Identity' trait not specialized for given type.");
};

template <typename T>
struct Zero {
  static_assert(FailOnInstantiation<Zero<T>>::value, "'Zero' trait not specialized for given type.");
};

template <typename T>
struct Random {
  static_assert(FailOnInstantiation<Random<T>>::value, "'Random' trait not specialized for given type.");
};

template <typename T>
struct Evaluate {
  using type = std::decay_t<T>;  // Default e.g. for arithmetic types.
};

// A value matrix backend adapts a linear algebra library by specializing the traits of this file, e.g. Sum, Transpose or Copy,
// for AccessImplementation<ImplementationT_>, where ImplementationT_ is a matrix or an expression of the library. ValueMatrixBackend
// in concepts.hpp lists the traits the core needs. The specializations are constrained to the backend's own implementation types,
// e.g. `template <EigenArray ImplementationT_>`, so that several value matrix backends can be used in one program (see "The backend
// interface" in DESIGN.md). Specializations that apply a C++ operator of the library, e.g. `implLhs + implRhs` for Sum, inherit
// it from internal/operator_traits.hpp, e.g. from internal::OperatorSum.

// Value matrices, quantities and tensors have value semantics with every backend: they never share their values with another
// object (see "Results of operations are evaluated" in DESIGN.md). A value matrix backend provides this with Copy and Move,
// which it specializes for AccessImplementation<ImplementationT_> of all its implementation types, including expressions. The
// results of its other traits, e.g. Transpose<AccessImplementation<ImplementationT_>>::value() or Evaluate, may be lazy
// expressions, which the core evaluates into new matrices, but must not be matrices that share the values of their operands:
// the core moves such results instead of copying them. E.g. Blitz++'s transpose() and slices are arrays that share the values
// of the original, so the Blitz++ backend copies them.

//! Copies a matrix of a value matrix backend: `value` returns the copy, which must not share its values with the original.
//! Eigen copies with the copy constructor; Blitz++, whose copy constructor shares the values, makes a deep copy. A copy of an
//! expression may refer to the same operands, like the expression itself.
template <typename T>
struct Copy {
  static_assert(FailOnInstantiation<Copy<T>>::value, "'Copy' trait not specialized for given type.");
};

//! Moves a matrix of a value matrix backend: `construct` returns the moved matrix, `assign` moves it into another one. The
//! moved-from matrix may be left empty, like a moved-from dynamic Eigen matrix, so it can only be assigned to or destroyed.
//! Eigen moves with its move constructor and assignment; Blitz++, which cannot move arrays, hands the values over and leaves the
//! moved-from array empty, so that no two arrays share them.
template <typename T>
struct Move {
  static_assert(FailOnInstantiation<Move<T>>::value, "'Move' trait not specialized for given type.");
};

//! Whether converting a From into a To passes the checks of the converting constructor of To, e.g. whether the units of two
//! quantity matrices match. The converting constructors between similar types are explicit if the conversion is invalid, so
//! that type traits such as std::is_convertible report it as invalid and overloads such as f(VectorInA) and f(VectorInB) are
//! not ambiguous. Direct initialization still reaches the constructor, whose static_assert explains what does not match. A
//! specialization defines each check as a named constant, e.g. RowsMatchV, which the static_asserts reuse.
template <typename To, typename... From>
struct IsValidConversion : std::false_type {};

template <typename T>
struct IsAbsolute {
  static_assert(FailOnInstantiation<IsAbsolute<T>>::value, "'IsAbsolute' trait not specialized for given type.");
};

template <typename T>
struct MakeRelative {
  static_assert(FailOnInstantiation<MakeRelative<T>>::value, "'MakeRelative' trait not specialized for given type.");
};
template <typename T>
auto makeRelative(T t) {
  return MakeRelative<T>::make(std::move(t));
}

template <typename T>
struct MakeAbsolute {
  static_assert(FailOnInstantiation<MakeAbsolute<T>>::value, "'MakeAbsolute' trait not specialized for given type.");
};
template <typename T>
auto makeAbsolute(T t) {
  return MakeAbsolute<T>::make(std::move(t));
}

template <typename T>
struct GetCoordinateSystem {
  static_assert(FailOnInstantiation<GetCoordinateSystem<T>>::value, "'GetCoordinateSystem' trait not specialized for given type.");
};

template <typename T, typename NewCoordinateSystemT_>
struct CastCoordinateSystem {
  static_assert(FailOnInstantiation<CastCoordinateSystem<T, NewCoordinateSystemT_>>::value,
                "'CastCoordinateSystem' trait not specialized for given types.");
};
// TODO(remod): turn this (and other free functions?) into a member
template <typename To, typename T>
auto castCoordinateSystem(T t) {
  return typename Evaluate<typename CastCoordinateSystem<T, To>::type>::type{std::move(t).getValues()};
}

template <typename ImplementationT_, typename MatrixSizeT_>
struct ReserveMemory;

template <typename ImplementationT_, typename ArgsTupleT_>
struct Initialize;

//! Element access of a matrix of a value matrix backend: `get(impl, row, col)` returns an element, `set(impl, row, col, value)`
//! sets it. Eigen and Blitz++ both access elements with `impl(row, col)`.
template <typename T>
struct Coefficient {
  static_assert(FailOnInstantiation<Coefficient<T>>::value, "'Coefficient' trait not specialized for given type.");
};

//! The evaluated type of another value type and size, of the same backend as T: e.g. the 1x1 values of a scalar that an
//! operation combines with values of the type T. A value matrix backend specializes it for
//! AccessImplementation<ImplementationT_>, where `type` is the matrix of its ValueMatrix<ValueT_, RowsV_, ColsV_>, so that
//! the core never needs a default value matrix backend and several backends can be used in one program.
template <typename T, typename ValueT_, int RowsV_, int ColsV_>
struct Rebind {
  static_assert(FailOnInstantiation<Rebind<T, ValueT_, RowsV_, ColsV_>>::value, "'Rebind' trait not specialized for given type.");
};

//! The units for values of the type ValueT_, specialized for AccessImplementation<ValueT_> by the units backend, e.g. `Length`
//! and `Force` (see UnitSystem in concepts.hpp). A program uses exactly one units backend: the specializations of the
//! backends are the same template for every value type, so two of them cannot be used together.
template <typename UnitT_>
struct AccessUnits {
  static_assert(FailOnInstantiation<AccessUnits<UnitT_>>::value,
                "'AccessUnits' trait not specialized for given type. Make sure you included the necessary unit headers.");
};

template <typename ValueT_>
struct Precision {
  static_assert(FailOnInstantiation<Precision<ValueT_>>::value, "'Precision' trait not specialized for given types.");
};

template <>
struct Precision<int> {
  static constexpr int value() { return 0; }
};

template <>
struct Precision<float> {
  static constexpr float value() { return 1e-5f; }
};

template <>
struct Precision<double> {
  static constexpr double value() { return 1e-12; }
};

}  // namespace wuzp
