#pragma once

#include "wuzp/core/helpers.hpp"

namespace wuzp {

template <typename TransformationT_, typename Object>
struct TransformationTraits {
  static_assert(FailOnInstantiation<TransformationTraits<TransformationT_, Object>>::value,
                "'TransformationTraits' trait not specialized.");
};

}  // namespace wuzp
