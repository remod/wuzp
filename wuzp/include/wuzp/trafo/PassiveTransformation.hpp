#pragma once

#include <type_traits>

#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <typename Derived, typename From, typename To>
class PassiveTransformation;

template <typename Derived, typename FromName, typename ToName>
class PassiveTransformation<Derived, CoordinateSystem<FromName>, CoordinateSystem<ToName>> : public Transformation {
 public:
  using From = CoordinateSystem<FromName>;
  using To = CoordinateSystem<ToName>;

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
