#pragma once

#include "wuzp/core/CoordinateSystem.hpp"

namespace wuzp {
namespace internal {

template <typename From, typename To>
struct IntermediateName;
template <typename From, typename To>
using Intermediate = CoordinateSystem<IntermediateName<From, To>>;

}  // namespace internal
}  // namespace wuzp
