#pragma once

namespace wuzp {

/**
 * The base class of all transformations: linear transformations, e.g. rotations and Lorentz transformations, translations,
 * proper rigid transformations (first rotation, then translation) and Jacobians.
 * An active transformation (ActiveTransformation) moves objects within its coordinate system, a passive transformation
 * (PassiveTransformation) expresses the objects of the coordinate system From in the coordinate system To. A Lorentz
 * transformation is the passive linear transformation of spacetime vectors between two inertial frames, and a Jacobian the
 * derivative of a change of coordinates between two coordinate systems, possibly of different dimensions, which pushes forward
 * vectors and pulls back covectors (see Jacobian).
 * Transformations transform tensors with transform(), compose with operator* (see CompositionTraits) and have an inverse().
 * The destructor is protected and not virtual: transformations are values, which are never deleted through a pointer to this
 * class, so they need no virtual table, which would make every transformation larger.
 * It is a class template of the transformation DerivedT_ that derives from it, so that every transformation has an empty base of
 * its own: a proper rigid transformation and its rotation, its first member, would otherwise both have the empty base
 * Transformation, and two objects of the same type cannot have the same address, so the rotation would start after the
 * transformation's empty base, which would make the transformation larger than its members
 * (https://en.cppreference.com/w/cpp/language/ebo). AnyTransformation tells whether a type is a transformation.
 */
template <typename DerivedT_>
class Transformation {
 protected:
  ~Transformation() = default;
};

namespace internal {

//! Deduces the transformation DerivedT_ from a reference to its base class (see AnyTransformation). Not defined: it is only used
//! in unevaluated expressions.
template <typename DerivedT_>
void deduceTransformation(const Transformation<DerivedT_>& transformation);

}  // namespace internal

//! A transformation of wuzp, i.e. a type that derives from Transformation, also through a class that derives from a transformation.
//! The base must be public and unique, like those of wuzp's transformations: a class that derives from a transformation
//! privately is none to outside code, and one that derives from two transformations is ambiguous (compose them with
//! operator* instead).
template <typename T>
concept AnyTransformation = requires(const T& transformation) { internal::deduceTransformation(transformation); };

}  // namespace wuzp
