#pragma once

#include "ctm/traits.hpp"

namespace ctm {

template <typename Name>
struct Frame {};

template <typename Object>
struct GetFrame;

template <typename Lhs, typename Rhs>
struct Sum<Frame<Lhs>, Frame<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
  using type = Frame<Lhs>;
};

template <typename Lhs, typename Rhs>
struct Difference<Frame<Lhs>, Frame<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
  using type = Frame<Lhs>;
};

}  // namespace ctm
