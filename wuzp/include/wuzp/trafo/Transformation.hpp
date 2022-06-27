#pragma once

#include "wuzp/core/CoordinateSystem.hpp"

namespace wuzp {

/**
 * Base class for all transformations.
 * Transformations can be one or a combination of the following base-transformations:
 * Translation, rotation, reflection, dilation.
 */
class Transformation {
 protected:
  virtual ~Transformation() = default;
};

}  // namespace wuzp
