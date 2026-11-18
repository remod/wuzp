# Transformations

*wuzp* has these transformations ([code example](https://github.com/remod/wuzp/blob/master/examples/transformation.cpp)):

| Transformation | Active | Passive |
| --- | --- | --- |
| Linear transformation | `ActiveLinearTransformation` | `PassiveLinearTransformation` |
| Rotation | `ActiveSpatialRotation2`, `ActiveSpatialRotation3` | `PassiveSpatialRotation2`, `PassiveSpatialRotation3` |
| Translation | `ActiveSpatialTranslation2`, `ActiveSpatialTranslation3` | `PassiveSpatialTranslation2`, `PassiveSpatialTranslation3` |
| Proper rigid transformation (first rotation, then translation) | `ActiveProperRigidTransformation2`, `...3` | `PassiveProperRigidTransformation2`, `...3` |
| Lorentz transformation | | `LorentzTransformation2`, `...3`, `...4` |
| Jacobian | | `Jacobian` |

An **active** transformation moves the objects expressed in its coordinate system, resulting in new objects.
A **passive** transformation from `From` to `To` expresses the objects of `From` in `To`: there is only one object, but the coordinate system it is expressed in changes ([Active and passive transformation](https://en.wikipedia.org/wiki/Active_and_passive_transformation)).

```cpp
template <typename From, typename To>
using RigidTransformation = wuzp::PassiveProperRigidTransformation3<wuzp_eigen::ValueMatrix, From, To, double>;
using TransformationFromAToB = RigidTransformation<CoordinateSystemA, CoordinateSystemB>;

const TransformationFromAToB::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
const TransformationFromAToB::Translation translation{0.0, 0.0, -1.0};
const TransformationFromAToB transformation{rotation, translation};
const ForceInB force = transformation.transform(forceInA);  // Only rotated: a force is a vector.
const PointInB point = transformation.transform(pointInA);  // Rotated and translated.
```

`transform()` of a passive transformation only takes objects expressed in `From`, and that of an active transformation objects expressed in its coordinate system.

## How tensors transform

For a linear transformation with the component matrix `A`, i.e. `v' = A v` for vectors:

| Object | Indices | Rule |
| --- | --- | --- |
| Vector | `Indices<Up>` | `v' = A v` |
| Point | `Indices<Up>`, absolute | `p' = A p` |
| Covector | `Indices<Down>` | `w' = w A^-1` |
| Linear map | `Indices<Up, Down>` | `T' = A T A^-1` |
| Bilinear form, e.g. a mass matrix | `Indices<Down, Down>` | `M' = A^-T M A^-1` |
| E.g. a covariance | `Indices<Up, Up>` | `S' = A S A^T` |

Translations move points and leave vectors, covectors and tensors with two upper indices unchanged, whose components do not depend on the origin.
They reject the other tensors, since a common quantity of each kind depends on the origin, e.g. the inertia about a point, which the parallel axis theorem shifts, and the type cannot tell it from one that does not.
A proper rigid transformation, `x -> R x + t`, therefore transforms vectors, covectors and tensors with two upper indices by its rotation alone (see [Conventions](../conventions.md)).

## Composition and inverse

Transformations compose with `*` and have an `inverse()`.
`a2 * a1` applies `a1` first, then `a2`, like the product of their matrices.
Passive transformations compose in the order of [Drake's monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html), where `X_AB` transforms from `B` to `A`, and the coordinate systems that meet must match:

```cpp
X_AB * X_BC                    // OK: X_AC, from C to A.
X_AB * X_CB                    // Error: Adjacent frames do not match.
X_AB.inverse()                 // OK: X_BA, from A to B.
rotation_in_A * rotation_in_B  // Error: Coordinate systems do not match.
rotation * translation         // OK: a proper rigid transformation.
translation * scaling          // Error: These transformations cannot be composed.
```

The compositions that are again transformations of *wuzp* are defined: linear transformations with linear transformations, rotations with rotations, Lorentz transformations with Lorentz transformations, translations with translations, rotations, translations and proper rigid transformations with each other into proper rigid transformations, `(R1, t1) * (R2, t2) = (R1 R2, R1 t2 + t1)` (Solà, Deray and Atchuthan, ["A micro Lie theory for state estimation in robotics"](https://arxiv.org/abs/1812.01537), eq. 171), and Jacobians with Jacobians and changes of basis by the chain rule.
Other combinations are rejected, e.g. a translation with a scaling, whose composition is affine but not rigid.

The inverse of a rotation is its transpose `R^T`, that of a Lorentz transformation `η Λ^T η`, and that of a proper rigid transformation `(R^T, -R^T t)`: they need no matrix inverse and add no rounding errors.
The inverse of a linear transformation is the matrix inverse.

An active transformation and the passive transformation with the same parameters cancel each other: e.g. the active rotation by `R` and the passive rotation by the same angle around the same axis, whose matrix is `R^T`.
For proper rigid transformations, the passive one with the parameters `(R, t)` of a motion has the matrix and vector `(R^T, -R^T t)`, not `-t`.

## Validity of rotations and Lorentz transformations

Rotations and Lorentz transformations check that they are valid in debug builds, i.e. if `NDEBUG` is not defined, like [Drake's RotationMatrix](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html), and throw `std::invalid_argument` otherwise:

* A rotation is valid if `max_ij |(R R^T - I)_ij| <= tolerance` and `det(R) > 0`, which rules out reflections.
* A Lorentz transformation is valid if `max_ij |(Λ^T η Λ - η)_ij| <= tolerance`, the definition of Lorentz transformations (Carroll, [Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), eq. 1.14).

```cpp
Rotation{1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}  // Throws in debug builds: a shear is not a rotation.
Rotation::makeUnchecked(matrix)                        // Not checked, e.g. in hot paths.
Rotation::projectToRotation(matrix)                    // The nearest rotation, e.g. after rounding errors.
rotation.isValid()                                     // Checks in every build.
```

The tolerance is `1e-9` for `double`, `1e-4` for `float` and `0` for `int` (`ValidityTolerance`).
`projectToRotation()` computes the orthogonal factor of the polar decomposition by the Newton-Schulz iteration (Higham, Functions of Matrices, SIAM 2008, section 8.3), which works with every value matrix backend.
`createBoostMatrix()` creates a Lorentz boost from the velocity of `To` relative to `From`, which must be smaller than the speed of light.

## Jacobians

A Jacobian `J = d(To)/d(From)` of a change of coordinates, e.g. of the task space coordinates of a robot with respect to its joint coordinates, is its linearization at one point, possibly between coordinate systems of different dimensions.
It maps the tangent objects at that point, not the points: vectors are pushed forward, covectors pulled back (Lee, Introduction to Smooth Manifolds, 2nd ed., Springer, 2013, chapters 3, 11 and 12).

```cpp
jacobian.transform(jointVelocity)    // OK: the task space velocity J q'.
jacobian.transform(jointCovariance)  // OK: the task space covariance J S J^T.
jacobian.pullback(taskForce)         // OK: the joint torques J^T F of a force covector.
jacobian.pullback(forceVector.transpose())  // OK: a force vector as a covector, in a Euclidean coordinate system.
jacobian.pullback(taskMassMatrix)    // OK: the joint space inertia J^T M J.
jacobian.transform(jointMassMatrix.inverse()).inverse()  // OK: the operational space inertia (J M^-1 J^T)^-1 of Khatib.
jacobian.transform(jointPosition)    // Error: A Jacobian maps vectors, not points: ...
jacobianAB * jacobianBC              // OK: the chain rule, J_AC.
rotationEW * jacobianWQ              // OK: the Jacobian J_EQ, expressed in E instead of W.
```

The matrix `J_ij = dx_i / dq_j` has the units `[x_i] / [q_j]`, so the units of the results follow: e.g. the pullback of a force in `N` by the Jacobian of Cartesian coordinates with respect to polar coordinates is a force in `N` along `r` and a torque in `N m` about the origin.
Only square Jacobians have an `inverse()`.
The [rotating dumbbell](../cookbook/rotating-dumbbell.md) shows a complete example.
