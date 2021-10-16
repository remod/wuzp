#pragma once

#include "wuzp/core/helpers.hpp"

namespace wuzp {

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

template <typename T>
struct Dimension {
  static_assert(FakeDependency<Dimension<T>>::value, "'Dimension' trait not specialized for given type.");
};

template <typename T>
struct Rows {
  static_assert(FakeDependency<Rows<T>>::value, "'Rows' trait not specialized for given type.");
};

template <typename T>
struct Cols {
  static_assert(FakeDependency<Cols<T>>::value, "'Cols' trait not specialized for given type.");
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

template <typename Object, int ElementRowId, int ElementColId>
struct Element {
  static_assert(FakeDependency<Element<Object, ElementRowId, ElementColId>>::value, "'Element' trait not specialized for given type.");
};

template <typename Object, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block {
  static_assert(FakeDependency<Block<Object, BlockRowId, BlockColId, BlockRows, BlockCols>>::value,
                "'Block' trait not specialized for given type.");
};

template <typename T>
struct Inverse {
  static_assert(FakeDependency<Inverse<T>>::value, "'Inverse' trait not specialized for given type.");
};

template <typename T>
struct IsAbsolute {
  static_assert(FakeDependency<IsAbsolute<T>>::value, "'IsAbsolute' trait not specialized for given type.");
};
template <typename T>
struct MakeRelative {
  static_assert(FakeDependency<MakeRelative<T>>::value, "'MakeRelative' trait not specialized for given type.");
};
// TODO(remod): Make this more generic: Create a helper which takes two types (T and MakeAbsolute<T>::type) and a boolean to determine which
// one to typedef.
template <typename T, bool Enable>
struct MakeAbsolute {
  static_assert(FakeDependency<MakeAbsolute<T, Enable>>::value, "'MakeAbsolute' trait not specialized for given type.");
};

template <typename T, typename NewCoordinateSystem>
struct CastCoordinateSystem;

template <typename T>
struct Evaluate;

}  // namespace wuzp
