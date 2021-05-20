#include <gtest/gtest.h>

#include "ctm/core/Frame.hpp"
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"

using ctm::Frame;
using ctm::FrameTraits;
using ctm::Sum;
using ctm::TypeMatrix;

TEST(FrameMatrix, IsSame) {  // NOLINT
  struct FrameAName {};
  using FrameA = Frame<FrameAName>;
  using MyFrames = TypeMatrix<3, 2, FrameTraits, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  static_assert(std::is_same<Sum<MyFrames, MyFrames>::type, MyFrames>::value);
}
