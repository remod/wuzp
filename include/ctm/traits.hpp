#pragma once

#include "ctm/helpers.hpp"

namespace ctm {

// TODO(remod): Extend Product like we did with Sum.
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
