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
 */
class Transformation {
 protected:
  ~Transformation() = default;
};

}  // namespace wuzp
