#pragma once

#include "wuzp/core/CoordinateSystem.hpp"

namespace wuzp {

template <typename Transformation, typename Object>
struct TransformationTraits {
  static_assert(FakeDependency<TransformationTraits<Transformation, Object>>::value, "'TransformationTraits' trait not specialized.");
};

class Transformation {};

// Translation, Rotation, Reflection, Dilation.

class ActiveTransformation : public Transformation {};

template <typename Derived, typename From, typename To>
class PassiveTransformation : public Transformation {
 public:
  // TODO(remod): Add test to make sure From and To are actually frames.

  template <typename Object>
  auto transform(const Object& object) const {
    static_assert(!std::is_same<typename GetCoordinateSystem<Object>::type, From>::value, "Coordinate system mismatch.");
    return TransformationTraits<Derived, Object>::transform(derived(), object);
  }

 private:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

}  // namespace wuzp
