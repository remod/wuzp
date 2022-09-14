#pragma once

#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

// TODO(remod): When instantiating a spatial rotation, test for determinant == 1.
// TODO(remod): Call "unitless rotation" or simply "rotation"?
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int Dimension>
using PassiveSpatialRotation = PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, Dimension, Dimension,
                                                           typename AccessUnits<AccessImplementation<Value>>::Dimensionless>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using PassiveSpatialRotation2 = PassiveSpatialRotation<ValuesT_, Value, CoordinateSystem, From, To, 2>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using PassiveSpatialRotation3 = PassiveSpatialRotation<ValuesT_, Value, CoordinateSystem, From, To, 3>;

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
// template <typename ValueT, typename From, typename To>
// class Quaternion : public QuaternionBase<Quaternion<ValueT, From, To>, From, To, Quantity<Dimensionless, ValueT>> {
//  // TODO(remod): Using friend is non-extendable. Change this.
//  friend TransformationTraits<Quaternion<ValueT, From, To>, PositionVector<3, ValueT, From>>;
//  friend TransformationTraits<Quaternion<ValueT, From, To>, PositionPoint<3, ValueT, From>>;
//
// public:
//  using DimensionlessQuantity = Quantity<Dimensionless, ValueT>;
//
//  template <typename... Ts>
//  explicit Quaternion(Ts... args) : impl_(args...) {}
//
// protected:
//  DimensionlessQuantity wDerived() const { return impl_.w(); }
//
// private:
//  Eigen::Quaternion<ValueT> impl_;
//};
//
// template <typename From, typename To, typename ValueT>
// struct TransformationTraits<Quaternion<ValueT, From, To>, PositionVector<3, ValueT, From>> {
//  static auto transform(const Quaternion<ValueT, From, To>& transformation, const PositionVector<3, ValueT, From>& object) {
//    return PositionVector<3, ValueT, To>(transformation.impl_ * object.getValues());
//  }
//};
//
// template <typename From, typename To, typename ValueT>
// struct TransformationTraits<Quaternion<ValueT, From, To>, PositionPoint<3, ValueT, From>> {
//  static auto transform(const Quaternion<ValueT, From, To>& transformation, const PositionPoint<3, ValueT, From>& object) {
//    return PositionPoint<3, ValueT, To>(transformation.impl_ * object.getValues());
//  }
//};

}  // namespace wuzp
