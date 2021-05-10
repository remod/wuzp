#pragma once

#include "ctm/helpers.hpp"

namespace ctm {

template <typename Lhs, typename Rhs>
struct Sum {
  static_assert(FakeDependency<Sum<Lhs, Rhs>>::value, "'Sum' trait not specialized for given types.");
};

template <typename Lhs, typename Rhs>
struct Difference {
  static_assert(FakeDependency<Difference<Lhs, Rhs>>::value, "'Difference' trait not specialized for given types.");
};

template <typename Lhs, typename Rhs>
struct Product {
  static_assert(FakeDependency<Difference<Lhs, Rhs>>::value, "'Product' trait not specialized for given types.");
};

template <typename T>
struct Transpose {
  static_assert(FakeDependency<Transpose<T>>::value, "'Transpose' trait not specialized for given type.");
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
