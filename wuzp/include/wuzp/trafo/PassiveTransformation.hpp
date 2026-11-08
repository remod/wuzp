#pragma once

#include <type_traits>

#include "wuzp/core/concepts.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <typename Derived, AnyCoordinateSystem From_, AnyCoordinateSystem To_>
class PassiveTransformation : public Transformation {
 public:
  using From = From_;
  using To = To_;

  template <typename Object>
  auto transform(const Object& object) const {
    static_assert(std::is_same<typename GetCoordinateSystem<Object>::type, From>::value, "Coordinate systems do not match.");
    return TransformationTraits<Derived, Object>::transform(derived(), object);
  }

  template <typename Other>
  bool isApprox(const Other& other) {
    return derived().getTensor().isApprox(other.getTensor());
  }

  // TODO(remod): Add inverse() function.

 private:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

}  // namespace wuzp
