#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/traits.hpp"

using ctm::Frame;
using ctm::Sum;
using ctm::TypeMatrix;

TEST(Frame, Matrix) {  // NOLINT
  struct FrameAName {};
  using FrameA = Frame<FrameAName>;
  using MyFrames = TypeMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  static_assert(std::is_same<Sum<MyFrames, MyFrames>::type, MyFrames>::value);
}
