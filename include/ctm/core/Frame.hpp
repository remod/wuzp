#pragma once

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"

namespace ctm {

template <typename Name>
struct Frame {};

// TODO(remod): Use this for now, is there a better solution?
struct NoFrameName {};
using NoFrame = Frame<NoFrameName>;

template <typename Object>
struct GetFrame;

struct FrameTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
    using type = Lhs;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
    using type = Lhs;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename Lhs, typename Rhs>
  struct Product<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Frame mismatch.");
    using type = NoFrame;
  };
};

template <int Rows, int Cols, typename... Frames>
using FrameMatrix = TypeMatrix<Rows, Cols, FrameTraits, Frames...>;

}  // namespace ctm
