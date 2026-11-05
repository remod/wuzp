#pragma once

#include "wuzp/core/traits.hpp"

namespace wuzp::internal {

template <bool Value_>
struct AbsoluteImpl {
  static constexpr bool value = Value_;
};

}  // namespace wuzp::internal
