#pragma once

#include <stdexcept>

//! Whether calling the function throws std::invalid_argument. The tests of the validity checks of rotations and Lorentz
//! transformations compare it with wuzp::internal::checksValidity, since the checks only run in debug builds. This replaces an
//! if constexpr that chooses between EXPECT_THROW and EXPECT_NO_THROW: MSVC warns about the unreferenced labels of the macro in
//! the discarded branch (C4102).
template <typename FunctionT_>
bool throwsInvalidArgument(const FunctionT_& function) {
  try {
    static_cast<void>(function());
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
