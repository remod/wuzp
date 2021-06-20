#pragma once

#include "ctm/core/helpers.hpp"

namespace ctm {

// Some matrix operations (e.g. multiplication) requires the sum of an arbitrary number of elements to be defined.
#define DEFINE_RECURSIVE_TRAIT(Name)                                      \
  template <typename... Ts>                                               \
  struct Name;                                                            \
                                                                          \
  template <typename T0>                                                  \
  struct Name<T0> {                                                       \
    using type = T0;                                                      \
  };                                                                      \
                                                                          \
  template <typename T0, typename T1, typename... Ts>                     \
  struct Name<T0, T1, Ts...> {                                            \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                      \
    using type = typename Name<typename Name<T0, T1>::type, Ts...>::type; \
  };

DEFINE_RECURSIVE_TRAIT(Sum)
DEFINE_RECURSIVE_TRAIT(Difference)
DEFINE_RECURSIVE_TRAIT(Product)
DEFINE_RECURSIVE_TRAIT(Division)

DEFINE_RECURSIVE_TRAIT(ValueSum)
DEFINE_RECURSIVE_TRAIT(ValueDifference)
DEFINE_RECURSIVE_TRAIT(ValueProduct)
DEFINE_RECURSIVE_TRAIT(ValueDivision)

template <typename ValueLhs, typename ValueRhs>
struct ValueSum<ValueLhs, ValueRhs> {
  // Design decision: Do not allow mixing types, require a manual cast.
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
  using type = ValueLhs;
};

template <typename ValueLhs, typename ValueRhs>
struct ValueDifference<ValueLhs, ValueRhs> {
  // Design decision: Do not allow mixing types, require a manual cast.
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
  using type = ValueLhs;
};

template <typename ValueLhs, typename ValueRhs>
struct ValueProduct<ValueLhs, ValueRhs> {
  // Design decision: Do not allow mixing types, require a manual cast.
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
  using type = ValueLhs;
};

template <typename ValueLhs, typename ValueRhs>
struct ValueDivision<ValueLhs, ValueRhs> {
  // Design decision: Do not allow mixing types, require a manual cast.
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
  using type = ValueLhs;
};

// DEFINE_RECURSIVE_TRAIT(UnitSum)
// DEFINE_RECURSIVE_TRAIT(UnitDifference)
// DEFINE_RECURSIVE_TRAIT(UnitProduct)
// DEFINE_RECURSIVE_TRAIT(UnitDivision)

template <typename T>
struct Transpose {
  static_assert(FakeDependency<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
};

template <typename Lhs, typename Rhs>
struct DotProduct {
  static_assert(FakeDependency<DotProduct<Lhs, Rhs>>::value, "'DotProduct' trait not specialized for given type.");
};

template <typename Unit>
struct UnitInverse;

template <typename UnitOfFamily>
struct UnitIdentity;

template <typename Object, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block {
  static_assert(FakeDependency<Block<Object, BlockRowId, BlockColId, BlockRows, BlockCols>>::value,
                "'Block' trait not specialized for given type.");
};

template <typename T>
struct Inverse;

}  // namespace ctm
