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

template <typename Label>
struct Tag {};
template <typename T, typename... Args>
struct GetTag;

struct FromTensorLabel {};
using FromTensor = Tag<FromTensorLabel>;
struct FromCoordinateMatrixLabel {};
using FromCoordinateMatrix = Tag<FromCoordinateMatrixLabel>;
struct FromQuantityMatrixLabel {};
using FromQuantityMatrix = Tag<FromQuantityMatrixLabel>;
struct FromQuantitiesLabel {};
using FromQuantities = Tag<FromQuantitiesLabel>;
struct FromCoordinateMatrixArgsLabel {};
using FromCoordinateMatrixArgs = Tag<FromCoordinateMatrixArgsLabel>;
struct FromQuantityMatrixArgsLabel {};
using FromQuantityMatrixArgs = Tag<FromQuantityMatrixArgsLabel>;
struct FromValueMatrixLabel {};
using FromValueMatrix = Tag<FromValueMatrixLabel>;
struct FromValueMatrixArgsLabel {};
using FromValueMatrixArgs = Tag<FromValueMatrixArgsLabel>;

template <typename T>
struct Scalar {
  static_assert(FakeDependency<Scalar<T>>::value, "'Scalar' trait not specialized for given type.");
};

template <typename T>
struct Rows {
  static_assert(FakeDependency<Rows<T>>::value, "'Rows' trait not specialized for given type.");
};

template <typename T>
struct Cols {
  static_assert(FakeDependency<Cols<T>>::value, "'Cols' trait not specialized for given type.");
};

template <typename T>
struct Dimension {
  static constexpr int value = static_cast<int>(Rows<T>::value > 1) + static_cast<int>(Cols<T>::value > 1);
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
struct Zero {
  static_assert(FakeDependency<Zero<T>>::value, "'Zero' trait not specialized for given type.");
};

template <typename T>
struct Random {
  static_assert(FakeDependency<Random<T>>::value, "'Random' trait not specialized for given type.");
};

template <typename T>
struct Evaluate {
  static_assert(FakeDependency<Evaluate<T>>::value, "'Evaluate' trait not specialized for given type.");
};
template <typename T>
static auto evaluate(T t);

template <typename T>
struct IsAbsolute {
  static_assert(FakeDependency<IsAbsolute<T>>::value, "'IsAbsolute' trait not specialized for given type.");
};
template <typename T>
struct MakeRelative {
  static_assert(FakeDependency<MakeRelative<T>>::value, "'MakeRelative' trait not specialized for given type.");
};
template <typename T>
struct MakeAbsolute {
  static_assert(FakeDependency<MakeAbsolute<T>>::value, "'MakeAbsolute' trait not specialized for given type.");
};

template <typename T, typename NewCoordinateSystem>
struct CastCoordinateSystem;

template <typename T>
struct Evaluate;

// TODO(remod): Cleanup - remove traits and only use these methods?

template <typename T>
auto makeRelative(T t) {
  return MakeRelative<T>::make(t);
}
template <typename T>
auto makeAbsolute(T t) {
  return MakeAbsolute<T>::make(t);
}
template <typename To, typename T>
auto castCoordinateSystem(T t) {
  return typename CastCoordinateSystem<T, To>::type(t.getCoordinateMatrix().getQuantityMatrix());
}
template <typename T>
auto evaluate(T t) {
  return typename Evaluate<T>::type(t);
}

}  // namespace wuzp
