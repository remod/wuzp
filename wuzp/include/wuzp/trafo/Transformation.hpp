#pragma once

#include "wuzp/core/CoordinateSystemTemplate.hpp"

namespace wuzp {

template <typename Transformation, typename Object>
struct TransformationTraits {
  static_assert(FailOnInstantiation<TransformationTraits<Transformation, Object>>::value, "'TransformationTraits' trait not specialized.");
};

class Transformation {
 protected:
  virtual ~Transformation() = default;
};

// Translation, SpatialRotation, Reflection, Dilation.

class ActiveTransformation : public Transformation {};

template <typename Derived, typename CoordinateSystem, typename From, typename To>
class PassiveTransformation : public Transformation {
 public:
  // TODO(remod): Add test to make sure From and To are actually frames.

  template <typename Object>
  auto transform(const Object& object) const {
    static_assert(std::is_same<typename GetCoordinateSystem<Object>::type, From>::value, "Coordinate systems do not match.");
    return TransformationTraits<Derived, Object>::transform(derived(), object);
  }

  template <typename Other>
  bool isApprox(const Other& other) {
    static_assert(std::is_same<typename GetCoordinateSystem<Other>::type, CoordinateSystem>::value, "Coordinate systems do not match.");
    return derived().getTensor().isApprox(other.getTensor());
  }

  // TODO(remod): Add inverse() function.

 private:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

// Static frames
// SpatialRotation:    B_r = A_BA * A_r
//              B_v = A_BA * A_v
//              B_a = A_BA * A_a
// Translation: r_0B = r_AB + r_0A

}  // namespace wuzp
