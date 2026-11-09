#pragma once

#include <type_traits>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <typename Derived>
class ActiveTransformation : public Transformation {
 public:
  template <typename Object>
  auto transform(const Object& object) const {
    return TransformationTraits<Derived, Object>::transform(derived(), object);
  }

  //! Whether the matrices or vectors of both transformations, which must be expressed in the same coordinate system, are
  //! approximately equal. They are compared as quantities: the matrix of a linear transformation is only a tensor if it is square
  //! (see getTensor()).
  template <typename Other>
  bool isApprox(const Other& other) {
    static_assert(std::is_same_v<typename GetCoordinateSystem<Derived>::type, typename GetCoordinateSystem<Other>::type>,
                  "Coordinate systems do not match.");
    return derived().getQuantities().isApprox(other.getQuantities());
  }

  // TODO(remod): Add inverse() function.

 private:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

}  // namespace wuzp
