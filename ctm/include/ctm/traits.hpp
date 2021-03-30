#pragma once

#include "ctm/helpers.hpp"

namespace ctm {

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

}
