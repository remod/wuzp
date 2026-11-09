#pragma once

#include "wuzp/core/CoordinateSystem.hpp"

namespace wuzp {
namespace internal {

template <typename FromT_, typename ToT_>
struct IntermediateName;
template <typename FromT_, typename ToT_>
using Intermediate = CoordinateSystem<IntermediateName<FromT_, ToT_>>;

}  // namespace internal
}  // namespace wuzp
