#pragma once

#include "ctm/Transformation.hpp"

namespace ctm {

template <typename Derived, typename From, typename To, typename DimensionlessQuantity>
class QuaternionBase : public PassiveTransformation<Derived, From, To> {
 public:
  DimensionlessQuantity w() const { return derived().wDerived(); }

 private:
  Derived& derived() { return static_cast<Derived&>(*this); }
  const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

class Rotation2D {};

}  // namespace ctm
