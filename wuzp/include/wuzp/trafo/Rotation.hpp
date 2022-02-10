#pragma once

#include "wuzp/trafo/LinearTransformation.hpp"

namespace wuzp {

// TODO(remod): When instantiating a spacial rotation, test for determinant == 1.
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int Dimension>
using SpatialRotation = LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, Dimension, Dimension,
                                             typename AccessUnits<SpecializationDummy<Scalar>>::Dimensionless>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialRotation2 = SpatialRotation<ValueMatrix, Scalar, CoordinateSystem, From, To, 2>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialRotation3 = SpatialRotation<ValueMatrix, Scalar, CoordinateSystem, From, To, 3>;

// template <typename Derived, typename From, typename To, typename DimensionlessQuantity>
// class QuaternionBase : public PassiveTransformation<Derived, From, To> {
// public:
//  DimensionlessQuantity w() const { return derived().wDerived(); }
//
// private:
//  Derived& derived() { return static_cast<Derived&>(*this); }
//  const Derived& derived() const { return static_cast<const Derived&>(*this); }
//};
//
//// TODO(remod): Do we still need to support quaternions? Or just conversion methods?
// template <typename ScalarT, typename From, typename To>
// class Quaternion : public QuaternionBase<Quaternion<ScalarT, From, To>, From, To, Quantity<Dimensionless, ScalarT>> {
//  // TODO(remod): Using friend is non-extendable. Change this.
//  friend TransformationTraits<Quaternion<ScalarT, From, To>, PositionVector<3, ScalarT, From>>;
//  friend TransformationTraits<Quaternion<ScalarT, From, To>, PositionPoint<3, ScalarT, From>>;
//
// public:
//  using DimensionlessQuantity = Quantity<Dimensionless, ScalarT>;
//
//  template <typename... Ts>
//  explicit Quaternion(Ts... args) : impl_(args...) {}
//
// protected:
//  DimensionlessQuantity wDerived() const { return impl_.w(); }
//
// private:
//  Eigen::Quaternion<ScalarT> impl_;
//};
//
// template <typename From, typename To, typename ScalarT>
// struct TransformationTraits<Quaternion<ScalarT, From, To>, PositionVector<3, ScalarT, From>> {
//  static auto transform(const Quaternion<ScalarT, From, To>& transformation, const PositionVector<3, ScalarT, From>& object) {
//    return PositionVector<3, ScalarT, To>(transformation.impl_ * object.getValueMatrix());
//  }
//};
//
// template <typename From, typename To, typename ScalarT>
// struct TransformationTraits<Quaternion<ScalarT, From, To>, PositionPoint<3, ScalarT, From>> {
//  static auto transform(const Quaternion<ScalarT, From, To>& transformation, const PositionPoint<3, ScalarT, From>& object) {
//    return PositionPoint<3, ScalarT, To>(transformation.impl_ * object.getValueMatrix());
//  }
//};

}  // namespace wuzp
