#pragma once

#include "helpers.hpp"

namespace ctm {

// Some matrix operations (e.g. multiplication) requires the sum of an arbitrary number of elements to be defined.
template <typename... Ts>
struct Sum;

template <typename T0>
struct Sum<T0> {
  using type = T0;
};

template <typename T0, typename T1>
struct Sum<T0, T1> {
  static_assert(FakeDependency<Sum<T0, T1>>::value, "'Sum' trait not specialized for given types.");
};

template <typename T0, typename T1, typename... Ts>
struct Sum<T0, T1, Ts...> {
  using type = typename Sum<typename Sum<T0, T1>::type, Ts...>::type;
};

template <typename... Ts>
struct Difference;

template <typename T0>
struct Difference<T0> {
  using type = T0;
};

template <typename T0, typename T1>
struct Difference<T0, T1> {
  static_assert(FakeDependency<Difference<T0, T1>>::value, "'Difference' trait not specialized for given types.");
};

template <typename T0, typename T1, typename... Ts>
struct Difference<T0, T1, Ts...> {
  using type = typename Difference<typename Difference<T0, T1>::type, Ts...>::type;
};

template <typename... Ts>
struct Product;

template <typename T0>
struct Product<T0> {
  using type = T0;
};

template <typename T0, typename T1>
struct Product<T0, T1> {
  static_assert(FakeDependency<Product<T0, T1>>::value, "'Product' trait not specialized for given types.");
};

template <typename T0, typename T1, typename... Ts>
struct Product<T0, T1, Ts...> {
  using type = typename Product<typename Product<T0, T1>::type, Ts...>::type;
};

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
