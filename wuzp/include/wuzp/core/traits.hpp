#pragma once

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
  static_assert(FailOnInstantiation<Comparison<Lhs, Rhs>>::value, "'Random' trait not specialized for given type.");
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

WUZP_DEFINE_RECURSIVE_TRAIT(TensorProduct)
template <typename Lhs, typename Rhs>
auto tensorProduct(const Lhs& lhs, const Rhs& rhs);

WUZP_DEFINE_RECURSIVE_TRAIT(Contraction)
template <typename Lhs, typename Rhs>
auto contraction(const Lhs& lhs, const Rhs& rhs);

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

template <typename T, typename ExponentT>
struct Power {
  static_assert(FailOnInstantiation<Power<T, ExponentT>>::value, "'Power' trait not specialized for given type.");
};

template <typename T>
struct Transpose {
  static_assert(FailOnInstantiation<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
};

template <typename Object, int ElementRowId, int ElementColId>
struct Element {
  static_assert(FailOnInstantiation<Element<Object, ElementRowId, ElementColId>>::value, "'Element' trait not specialized for given type.");
};

template <typename Object, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block {
  static_assert(FailOnInstantiation<Block<Object, BlockRowId, BlockColId, BlockRows, BlockCols>>::value,
                "'Block' trait not specialized for given type.");
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
  return MakeRelative<T>::make(t);
}

template <typename T>
struct MakeAbsolute {
  static_assert(FailOnInstantiation<MakeAbsolute<T>>::value, "'MakeAbsolute' trait not specialized for given type.");
};
template <typename T>
auto makeAbsolute(T t) {
  return MakeAbsolute<T>::make(t);
}

template <typename T>
struct GetCoordinateSystem {
  static_assert(FailOnInstantiation<GetCoordinateSystem<T>>::value, "'GetCoordinateSystem' trait not specialized for given type.");
};

template <typename T, typename NewCoordinateSystem>
struct CastCoordinateSystem {
  static_assert(FailOnInstantiation<CastCoordinateSystem<T, NewCoordinateSystem>>::value,
                "'CastCoordinateSystem' trait not specialized for given types.");
};
// TODO(remod): turn this (and other free functions?) into a member
template <typename To, typename T>
auto castCoordinateSystem(T t) {
  return typename CastCoordinateSystem<T, To>::type(t.getValues());
}

template <typename ImplementationT_, typename MatrixSizeT_, typename Arg0, typename... Args>
struct Initialize;

template <typename UnitT>
struct AccessUnits {
  static_assert(FailOnInstantiation<AccessUnits<UnitT>>::value,
                "'AccessUnits' trait not specialized for given type. Make sure you included the necessary unit headers.");
};

template <typename ValueT>
struct AccessValues {
  static_assert(FailOnInstantiation<AccessValues<ValueT>>::value,
                "'AccessValues' trait not specialized for given type. Make sure you included the necessary value headers.");
};

template <typename ValueT>
struct Precision {
  static_assert(FailOnInstantiation<Precision<ValueT>>::value, "'Precision' trait not specialized for given types.");
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
