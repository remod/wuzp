#pragma once

#include "helpers.hpp"

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

DEFINE_RECURSIVE_TRAIT(ValuesSum)
DEFINE_RECURSIVE_TRAIT(ValuesDifference)
DEFINE_RECURSIVE_TRAIT(ValuesProduct)

template <typename T>
struct Transpose {
  static_assert(FakeDependency<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
};

template <typename Lhs, typename Rhs>
struct DotProduct {
  static_assert(FakeDependency<DotProduct<Lhs, Rhs>>::value, "'DotProduct' trait not specialized for given type.");
};

template <typename T>
struct Inverse {
  static_assert(FakeDependency<Inverse<T>>::value, "'Inverse' trait not specialized for given type.");
};

template <typename TypeOfFamily>
struct Identity {
  static_assert(FakeDependency<Identity<TypeOfFamily>>::value, "'Identity' trait not specialized for given type.");
};

template <typename Object, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block {
  static_assert(FakeDependency<Block<Object, BlockRowId, BlockColId, BlockRows, BlockCols>>::value,
                "'Block' trait not specialized for given type.");
};

}  // namespace ctm
