#pragma once

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

  template <typename Other>
  bool isApprox(const Other& other) {
    return derived().getTensor().isApprox(other.getTensor());
  }

  // TODO(remod): Add inverse() function.

 private:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

}  // namespace wuzp
