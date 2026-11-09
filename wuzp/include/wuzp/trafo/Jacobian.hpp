#pragma once

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

//! The Jacobian of the coordinates of To with respect to those of From, a passive linear transformation of tangent vectors, e.g.
//! velocities, from From to To. The composition of Jacobians is the chain rule: J_AB * J_BC = J_AC.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
using Jacobian = PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>;

}  // namespace wuzp
