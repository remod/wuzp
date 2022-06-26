#pragma once

#include "wuzp/core/helpers.hpp"

namespace wuzp {

template <typename Transformation, typename Object>
struct TransformationTraits {
  static_assert(FailOnInstantiation<TransformationTraits<Transformation, Object>>::value, "'TransformationTraits' trait not specialized.");
};

}  // namespace wuzp
