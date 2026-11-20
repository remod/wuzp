# Spatial vectors

The velocity of a point of a rotating body depends on the point, and on the frame it is measured in: a tensor, which only knows the coordinate system it is expressed in, cannot tell the velocities of two points apart, nor a velocity measured in the world from one measured in a moving frame.
The spatial vectors of *wuzp* also track the frame a motion is measured in, the frame it describes and the point it is about, like Drake's [spatial vectors](https://drake.mit.edu/doxygen_cxx/group__multibody__spatial__vectors.html), whose [monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) they follow:

| Type | Drake | Rotational part | Translational part |
| --- | --- | --- | --- |
| `SpatialVelocity<ValueMatrix, Value, M, B, P, E>` | `V_MBp_E`, a twist | the angular velocity `ω_MB` of `B` relative to `M` | the velocity `v_MBp` of the point `Bp` of `B` relative to `M` |
| `SpatialAcceleration<ValueMatrix, Value, M, B, P, E>` | `A_MBp_E` | the angular acceleration `α_MB` | the acceleration `a_MBp` |
| `SpatialForce<ValueMatrix, Value, B, P, E>` | `F_Bp_E`, a wrench | the torque `τ_Bp` about `Bp` | the force `f` on `B` |

Both parts are three-dimensional vectors expressed in `E`, the rotational part first, like in Drake and in Lynch and Park, Modern Robotics, Cambridge University Press, 2017, sections 3.3.2 and 3.4, and `getRotational()` and `getTranslational()` return them.
The three types derive from the base class `SpatialVector` (`<wuzp/trafo/SpatialVector.hpp>`), which defines these getters, the sums and the comparisons, and the concept `AnySpatialVector` tells whether a type is a spatial vector.
The frames, the point and the coordinate system are coordinate systems, which must be Euclidean: a point is the origin of a coordinate system.

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

using wuzp_eigen::ValueMatrix;

struct World {};
struct Body {};
struct Tip {};
using WorldFrame = wuzp::CoordinateSystem<World>;
using BodyFrame = wuzp::CoordinateSystem<Body>;
using TipPoint = wuzp::CoordinateSystem<Tip>;  // The tip of a tool fixed to the body.

// V_WBo_W: the body, measured in the world, about the origin of the body, expressed in the world.
using VelocityOfBody = wuzp::SpatialVelocity<ValueMatrix, double, WorldFrame, BodyFrame, BodyFrame, WorldFrame>;
using AngularVelocity = wuzp::AngularVelocityVector3<ValueMatrix, double, WorldFrame>;
using Velocity = wuzp::VelocityVector3<ValueMatrix, double, WorldFrame>;
using Position = wuzp::PositionVector3<ValueMatrix, double, WorldFrame>;

int main() {
  const VelocityOfBody velocityOfBody{AngularVelocity{0.0, 0.0, 1.0}, Velocity{1.0, 0.0, 0.0}};
  const Position p_BoTip_W{0.5, 0.0, 0.0};
  // V_WBtip_W: the angular velocity is the same, the velocity of the tip is (1, 0.5, 0) m/s.
  const auto velocityOfTip = velocityOfBody.shift<TipPoint>(p_BoTip_W);
  // velocityOfBody + velocityOfTip;  // Error: Reference points do not match.
  return 0;
}
```

Spatial vectors are only added, subtracted and compared if all their tags match, and fail otherwise with "Measured-in frames do not match.", "Frames do not match.", "Reference points do not match." or "Coordinate systems do not match.".
A spatial force records the frame `B` it acts on, so that the forces on different bodies are not added, but no frame it is measured in.

## Shifts and compositions

The named functions change the tags, with the transport formulas of Drake, where the position vector `p` between the points is expressed in `E`:

| Function | Result | Formula |
| --- | --- | --- |
| `V_MBp.shift<Q>(p_BpBq)` | `V_MBq` | `ω_MB` unchanged, `v_MBq = v_MBp + ω_MB × p_BpBq` |
| `V_MBp.composeWithMovingFrameVelocity(p_BpCq, V_BCq)` | `V_MCq` | `ω_MC = ω_MB + ω_BC`, `v_MCq = v_MBp + ω_MB × p_BpCq + v_BCq` |
| `A_MBp.shift<Q>(p_BpBq, V_MB)` | `A_MBq` | `α_MB` unchanged, `a_MBq = a_MBp + α_MB × p_BpBq + ω_MB × (ω_MB × p_BpBq)` |
| `A_MBp.composeWithMovingFrameAcceleration(p_BpCq, V_MB, V_BCq, A_BCq)` | `A_MCq` | `α_MC = α_MB + α_BC + ω_MB × ω_BC`, `a_MCq = a_MBp + α_MB × p_BpCq + ω_MB × (ω_MB × p_BpCq) + 2 ω_MB × v_BCq + a_BCq` |
| `F_Bp.shift<Q>(p_BpBq)` | `F_Bq` | `f` unchanged, `τ_Bq = τ_Bp - p_BpBq × f` |

The compositions describe a frame `C` that moves relative to the body `B`, e.g. a point that slides along a rotating rod, with the Coriolis acceleration `2 ω_MB × v_BCq`.
The frames that meet must match, `B` and the frame that `V_BCq` is measured in, like for `X_AB * X_BC`, and fail otherwise with "Adjacent frames do not match.".
The accelerations take the angular velocity `ω_MB` as the spatial velocity `V_MB` of `B` measured in `M`, about any point, so that its tags are checked too.
The position vector is a vector, the difference of two points: a point fails with "Absolute/relative mismatch.".

`velocityOfBodyFixedPoint(v_P, ω, p_PQ)` and `accelerationOfBodyFixedPoint(a_P, α, ω, p_PQ)` (`<wuzp/trafo/kinematics.hpp>`) apply the formulas of the shifts to vectors, which only checks their units and coordinate systems (see [Rigid body kinematics](../cookbook/rigid-body-kinematics.md)).

## Re-expression and the adjoint map

A passive rotation or a passive proper rigid transformation from `E` to `F` re-expresses a spatial vector in `F`: it rotates both parts and changes no other tag, like for vectors, which translations leave unchanged, since the point is the tag `P`, not the origin of `E`:

```cpp
const VelocityInBody velocityInBody = X_BW.transform(velocityOfBody);  // V_WBo_B, still about Bo.
```

The point stays the same, so this is not the adjoint map below, which also moves the point.
The angular velocity of `PassiveSpatialRotation<From = B, To = A>`, that of `B` relative to `A`, is the rotational part of `SpatialVelocity<A, B, P, E>`, expressed in `E = A` or `E = B`.
Active transformations do not transform spatial vectors: they would move the motion or the force, which the frames and the point of the type would no longer describe.

`adjoint(X_AB, V)` maps a spatial velocity or a spatial force about the origin of `B`, expressed in `B`, by the passive proper rigid transformation `X_AB = (R_AB, p_AoBo_A)` to the origin of `A`, expressed in `A`, like the adjoint map of Modern Robotics:

| Spatial vector | Result |
| --- | --- |
| `V_MCbo_B` | `V_MCao_A`: `ω' = R ω`, `v' = R v + p × (R ω)`, i.e. `V_s = [Ad_T_sb] V_b` (section 3.3.2) |
| `F_Cbo_B` | `F_Cao_A`: `f' = R f`, `τ' = R τ + p × (R f)`, i.e. `F_s = [Ad_T_bs]^T F_b` (section 3.4) |

The adjoint is the re-expression by `X_AB` followed by the shift to the origin of `A`, `X_AB.transform(V).shift<A>(-p_AoBo_A)`.
It maps the body twist of Modern Robotics, `V_ABbo_B`, to the spatial twist, `V_ABao_A`, and it preserves the power `τ · ω + f · v` of a spatial force and a spatial velocity.
A spatial vector about another point fails with a message that says to `shift()` it to the origin of `B` first.
A spatial acceleration has no adjoint map, since shifting it needs the angular velocity: re-express it with `transform()` and `shift()` it.

## What is not modeled

The power `dot()` of a spatial force and a spatial velocity, the negation and the products with scalars, `zero()`, the parts as one 6-vector, spatial Jacobians, spatial momenta and inertias, the spatial accelerations of Featherstone, planar spatial vectors and the adjoint map of active transformations are left out.
See "Spatial vectors" in the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#spatial-vectors) for the decisions.
