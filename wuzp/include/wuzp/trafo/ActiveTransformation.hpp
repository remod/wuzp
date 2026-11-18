#pragma once

#include <type_traits>

#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

//! An active transformation, which moves objects within its coordinate system (see Transformation).
template <typename DerivedT_>
class ActiveTransformation : public internal::Crtp<DerivedT_, Transformation> {
 public:
  template <typename Object>
  auto transform(const Object& object) const {
    return TransformationTraits<DerivedT_, Object>::transform(this->derived(), object);
  }
};

/**
 * The composition of two active transformations in the same coordinate system, which applies rhs first, then lhs:
 * (lhs * rhs).transform(x) is lhs.transform(rhs.transform(x)), like the product of their matrices (see CompositionTraits).
 */
template <typename LhsT_, typename RhsT_>
auto operator*(const ActiveTransformation<LhsT_>& lhs, const ActiveTransformation<RhsT_>& rhs) {
  static_assert(std::is_same_v<typename GetCoordinateSystem<LhsT_>::type, typename GetCoordinateSystem<RhsT_>::type>,
                "Coordinate systems do not match.");
  return CompositionTraits<LhsT_, RhsT_>::compose(static_cast<const LhsT_&>(lhs), static_cast<const RhsT_&>(rhs));
}

}  // namespace wuzp
