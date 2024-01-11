#pragma once

#include "wuzp/core/ValuesWrapper.hpp"

namespace wuzp {

template <typename ValueT_>
using ValueWrapper = typename AccessValues<AccessImplementation<ValueT_>>::template Value<ValueT_>;

}  // namespace wuzp
