#pragma once

#include "ctm/core/helpers.hpp"

namespace ctm {

// Some operations (sums, differences, products, divisions) require the corresponding trait to be defined for an arbitrary number of
// arguments.
#define DEFINE_RECURSIVE_TRAIT(Name)                                      \
  template <typename... Ts>                                               \
  struct Name;                                                            \
  template <typename T0>                                                  \
  struct Name<T0> {                                                       \
    using type = T0;                                                      \
  };                                                                      \
  template <typename T0, typename T1, typename... Ts>                     \
  struct Name<T0, T1, Ts...> {                                            \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                      \
    using type = typename Name<typename Name<T0, T1>::type, Ts...>::type; \
  };

DEFINE_RECURSIVE_TRAIT(Sum)
DEFINE_RECURSIVE_TRAIT(Difference)
DEFINE_RECURSIVE_TRAIT(Product)
DEFINE_RECURSIVE_TRAIT(Division)

template <typename T>
struct Transpose {
  static_assert(FakeDependency<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
};

template <typename Lhs, typename Rhs>
struct DotProduct {
  static_assert(FakeDependency<DotProduct<Lhs, Rhs>>::value, "'DotProduct' trait not specialized for given type.");
};

template <typename Object, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block {
  static_assert(FakeDependency<Block<Object, BlockRowId, BlockColId, BlockRows, BlockCols>>::value,
                "'Block' trait not specialized for given type.");
};

template <typename T>
struct Inverse;

}  // namespace ctm
