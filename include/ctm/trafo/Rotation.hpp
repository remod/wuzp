#pragma once

// TODO(remod): Create abstraction for quaternion
#include <Eigen/Geometry>

#include "ctm/trafo/Transformation.hpp"

namespace ctm {

template <typename Derived, typename From, typename To, typename DimensionlessQuantity>
class QuaternionBase : public PassiveTransformation<Derived, From, To> {
 public:
  DimensionlessQuantity w() const { return derived().wDerived(); }

 private:
  Derived& derived() { return static_cast<Derived&>(*this); }
  const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

template <typename Scalar, typename From, typename To>
class Quaternion : public QuaternionBase<Quaternion<Scalar, From, To>, From, To, units::Quantity<units::Dimensionless, Scalar>> {
  // TODO(remod): Using friend is non-extendable. Change this.
  friend TransformationTraits<Quaternion<Scalar, From, To>, Vector<3, Scalar, From>>;
  friend TransformationTraits<Quaternion<Scalar, From, To>, Position<3, Scalar, From>>;

 public:
  using DimensionlessQuantity = units::Quantity<units::Dimensionless, Scalar>;

  template <typename... Ts>
  explicit Quaternion(Ts... args) : impl_(args...) {}

 protected:
  DimensionlessQuantity wDerived() const { return impl_.w(); }

 private:
  Eigen::Quaternion<Scalar> impl_;
};

template <typename From, typename To, typename Scalar>
struct TransformationTraits<Quaternion<Scalar, From, To>, Vector<3, Scalar, From>> {
  static auto transform(const Quaternion<Scalar, From, To>& transformation, const Vector<3, Scalar, From>& object) {
    return Vector<3, Scalar, To>(transformation.impl_ * object.getValues());
  }
};

template <typename From, typename To, typename Scalar>
struct TransformationTraits<Quaternion<Scalar, From, To>, Position<3, Scalar, From>> {
  static auto transform(const Quaternion<Scalar, From, To>& transformation, const Position<3, Scalar, From>& object) {
    return Position<3, Scalar, To>(transformation.impl_ * object.getValues());
  }
};

class Rotation2D {};

}  // namespace ctm
