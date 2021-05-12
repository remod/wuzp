#pragma once

#include "traits.hpp"

namespace ctm {

template <typename Name>
struct Frame {};

// TODO(remod): Use this for now, is there a better solution?
struct NoFrameName {};
using NoFrame = Frame<NoFrameName>;

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

template <typename Lhs, typename Rhs>
struct Product<Frame<Lhs>, Frame<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
  using type = NoFrame;
};

}  // namespace ctm
