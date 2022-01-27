#pragma once

#include "wuzp/core/helpers.hpp"

namespace wuzp {

// Some operations (sums, differences, products, divisions) require the corresponding trait to be defined for an arbitrary number of
// arguments.
#define DEFINE_RECURSIVE_TRAIT_IMPL(Name, NameStr)                                                                  \
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
#define DEFINE_RECURSIVE_TRAIT(Name) DEFINE_RECURSIVE_TRAIT_IMPL(Name, #Name)

template <typename Label>
struct Tag {};
template <typename T, typename... Args>
struct GetTag;

struct FromTensorLabel {};
using FromTensor = Tag<FromTensorLabel>;
struct FromCoordinateMatrixLabel {};
using FromCoordinateMatrix = Tag<FromCoordinateMatrixLabel>;
struct FromQuantityMatrixLabel {};
struct FromCoordinatesLabel {};
using FromCoordinates = Tag<FromCoordinatesLabel>;
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

// TODO(remod): Rename to GetValue.
template <typename T>
struct Scalar {
  static_assert(FailOnInstantiation<Scalar<T>>::value, "'Scalar' trait not specialized for given type.");
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

DEFINE_RECURSIVE_TRAIT(Sum)
DEFINE_RECURSIVE_TRAIT(Difference)
DEFINE_RECURSIVE_TRAIT(Product)
DEFINE_RECURSIVE_TRAIT(Division)

template <typename T, int Exponent>
struct Power {
  static_assert(FailOnInstantiation<Power<T, Exponent>>::value, "'Power' trait not specialized for given type.");
};

template <typename T>
struct Transpose {
  static_assert(FailOnInstantiation<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
};

template <typename Lhs, typename Rhs>
struct DotProduct {
  static_assert(FailOnInstantiation<DotProduct<Lhs, Rhs>>::value, "'DotProduct' trait not specialized for given type.");
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
  static_assert(FailOnInstantiation<Evaluate<T>>::value, "'Evaluate' trait not specialized for given type.");
};
template <typename T>
auto evaluate(T t) {
  return typename Evaluate<T>::type(t);
}

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

template <typename T, typename NewCoordinateSystem>
struct CastCoordinateSystem {
  static_assert(FailOnInstantiation<CastCoordinateSystem<T, NewCoordinateSystem>>::value,
                "'CastCoordinateSystem' trait not specialized for given types.");
};
template <typename To, typename T>
auto castCoordinateSystem(T t) {
  return typename CastCoordinateSystem<T, To>::type(t.getValueMatrix());
}

template <typename UnitT>
struct AccessUnits {
  static_assert(FailOnInstantiation<AccessUnits<UnitT>>::value,
                "'AccessUnits' trait not specialized for given type. Make sure you included the necessary unit headers.");
};

template <typename ValueT, typename UnitT>
struct GetQuantityImplementation {
  static_assert(FailOnInstantiation<GetQuantityImplementation<ValueT, UnitT>>::value,
                "'GetQuantityImplementation' trait not specialized for given types.");
};

}  // namespace wuzp
