#pragma once

// TODO(remod): Create abstraction for quaternion
#include <Eigen/Geometry>

#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/units/Quantity.hpp"

namespace wuzp {

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

template <typename Scalar, typename From, typename To, int Dimension>
class RotationMatrix : public PassiveTransformation<RotationMatrix<Scalar, From, To, Dimension>, From, To> {
  // TODO(remod): Using friend is non-extendable. Change this.
  friend TransformationTraits<RotationMatrix<Scalar, From, To, Dimension>, Vector<Dimension, Scalar, From>>;
  friend TransformationTraits<RotationMatrix<Scalar, From, To, Dimension>, Position<Dimension, Scalar, From>>;

 public:
  using Values = value_matrix::ValueMatrix<Scalar, Dimension, Dimension>;
  using Units = units::UnitMatrix<Dimension, Dimension, units::Dimensionless>;
  using CoordinateSystems = CoordinateSystemMatrix<Dimension, Dimension, From>;
  using Impl = CoordinateMatrix<Values, Units, CoordinateSystems>;

  explicit RotationMatrix(Impl impl) : impl_(impl) {}

 private:
  CoordinateMatrix<Values, Units, CoordinateSystems> impl_;
};

template <typename Scalar, typename From, typename To>
using RotationMatrix2D = RotationMatrix<Scalar, From, To, 2>;
template <typename Scalar, typename From, typename To>
using RotationMatrix3D = RotationMatrix<Scalar, From, To, 3>;

template <typename Scalar, typename From, typename To, int Dimension>
struct TransformationTraits<RotationMatrix<Scalar, From, To, Dimension>, Vector<Dimension, Scalar, From>> {
  static auto transform(const RotationMatrix<Scalar, From, To, Dimension>& transformation, const Vector<Dimension, Scalar, From>& object) {
    return Vector<Dimension, Scalar, To>(transformation.impl_ * object);
  }
};

template <typename Scalar, typename From, typename To, int Dimension>
struct TransformationTraits<RotationMatrix<Scalar, From, To, Dimension>, Position<Dimension, Scalar, From>> {
  static auto transform(const RotationMatrix<Scalar, From, To, Dimension>& transformation,
                        const Position<Dimension, Scalar, From>& object) {
    // TODO(remod): Avoid subtraction, just cast.
    // TODO(remod): Avoid constructor, just cast.
    return Position<Dimension, Scalar, To>(
        (transformation.impl_ * (object - Position<Dimension, Scalar, From>(Eigen::Matrix<Scalar, Dimension, 1>::Zero()))).getValues());
  }
};

}  // namespace wuzp
