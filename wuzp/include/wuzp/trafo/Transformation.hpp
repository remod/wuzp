#pragma once

namespace wuzp {

/**
 * The base class of all transformations: linear transformations, e.g. rotations, Jacobians and Lorentz transformations,
 * translations and proper rigid transformations (first rotation, then translation).
 * An active transformation (ActiveTransformation) moves objects within its coordinate system, a passive transformation
 * (PassiveTransformation) expresses the objects of the coordinate system From in the coordinate system To. A Jacobian is the
 * passive linear transformation of tangent vectors between two coordinate systems, possibly of different dimensions, and a
 * Lorentz transformation the passive linear transformation of spacetime vectors between two inertial frames.
 * Transformations transform tensors with transform(), compose with operator* (see CompositionTraits) and have an inverse().
 * The destructor is protected and not virtual: transformations are values, which are never deleted through a pointer to this
 * class, so they need no virtual table, which would make every transformation larger.
 */
class Transformation {
 protected:
  ~Transformation() = default;
};

}  // namespace wuzp
