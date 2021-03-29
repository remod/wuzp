#pragma once

namespace ctm {

template <typename Type>
struct Frame {};

template <typename Lhs, typename Rhs>
struct Sum<Frame<Lhs>, Frame<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
  using type = Frame<Lhs>;
};

}
