#pragma once

#include "ctm/helpers.hpp"

namespace ctm {

template <typename TypeOfFamily>
struct GetIdentity {
  static_assert(FakeDependency<GetIdentity<TypeOfFamily>>::value, "'GetIdentity' trait not specialized.");
};

template <typename Object, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct GetBlock {
  static_assert(FakeDependency<GetBlock<Object, BlockRowId, BlockColId, BlockRows, BlockCols>>::value, "'GetBlock' trait not specialized.");
};

template <typename... Ts>
struct Sum;

template <typename Lhs>
struct Sum<Lhs> {
  using type = Lhs;
};

template <typename Lhs, typename Rhs>
struct Sum<Lhs, Rhs> {
  static_assert(FakeDependency<Sum<Lhs, Rhs>>::value, "'Sum' trait not specialized.");
};

template <typename Lhs, typename Rhs, typename... Others>
struct Sum<Lhs, Rhs, Others...> {
  using type = typename Sum<typename Sum<Lhs, Rhs>::type, Others...>::type;
};

// TODO(remod): Extend Product like we did with Sum.
template <typename Lhs, typename Rhs>
struct Product;

template <typename T>
struct Transpose {
  static_assert(FakeDependency<Transpose<T>>::value, "'Transpose' trait not specialized.");
};

template <typename T>
struct Inverse {
  static_assert(FakeDependency<Inverse<T>>::value, "'Inverse' trait not specialized.");
};

}  // namespace ctm
