# Conventions

This page collects the conventions *wuzp* follows, with their sources.
The [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md) explain the decisions behind them.

## Index placement

*wuzp* follows chapter 1 of Carroll's [Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), on which his book Spacetime and Geometry is based:

* An upper index is contravariant, e.g. the components `v^i` of a vector, a lower index covariant, e.g. the components `w_i` of a covector (eqs. 1.30 to 1.39).
* Vectors are stored as columns, covectors as rows (eq. 1.39).
* The first index of a tensor of rank 2 is the row of its matrix, the second the column, like `M^i_j` in eq. 1.54.
* A contraction pairs an upper with a lower index, and the order of the indices matters (eqs. 1.59 and 1.60): `innerProduct(lhs, rhs)` contracts the last index of `lhs` with the first index of `rhs`.
* The pairing `w_i v^i` of a covector and a vector needs no metric (eq. 1.35); the dot product `g_ij v^i w^j` does (eq. 1.55).
* The inverse of a tensor of rank 2 maps the indices `(X, Y)` to `(not Y, not X)`, e.g. `g_ij` to `g^ij`, with `g^ij g_jk = δ^i_k` (eq. 1.56).

## Transformation rules

For a linear transformation with component matrix `A`, i.e. contravariant components transform as `v' = A v`:

| Object | Indices | Rule |
| --- | --- | --- |
| Vector | `Indices<Up>` | `v' = A v` |
| Point | `Indices<Up>`, absolute | `p' = A p` (via a relative intermediate) |
| Covector | `Indices<Down>` | `w' = w A^-1` |
| Linear map | `Indices<Up, Down>` | `T' = A T A^-1` |
| Bilinear form, e.g. a mass matrix | `Indices<Down, Down>` | `M' = A^-T M A^-1` |
| E.g. a covariance matrix | `Indices<Up, Up>` | `T' = A T A^T` |

`A^-1` appears twice in the rule for bilinear forms because `A` transforms components, so covariant indices transform with `A^-1`.
Equivalently, `A^-1` is the change-of-basis matrix `P` of the [Wikipedia article on change of basis](https://en.wikipedia.org/wiki/Change_of_basis) ("old = P new").

The matrix `A` of a passive linear transformation maps the components of a vector in `From` to its components in `To`: its rows are the dual basis covectors of `To`, and the basis vectors of `To` are the columns of `A^-1`, all expressed in `From` (Carroll, eqs. 1.38 and 1.30).
Only for orthonormal bases, e.g. for a rotation, are the rows of `A` also the basis vectors of `To`.

A Jacobian `J` of a change of coordinates from `From` to `To` maps tangent objects, not points: `transform()` pushes vectors forward, `v' = J v`, and tensors with two upper indices, `T' = J T J^T`; `pullback()` pulls covectors back, `w' = w J`, and tensors with two lower indices, `M' = J^T M J` (Lee, Introduction to Smooth Manifolds, 2nd ed., Springer, 2013, chapters 3, 11 and 12).

## Active and passive transformations

* An active transformation moves the objects expressed in its coordinate system.
* A passive transformation from `From` to `To` expresses the objects of `From` in `To`, and only takes objects expressed in `From` ([Active and passive transformation](https://en.wikipedia.org/wiki/Active_and_passive_transformation)).
* `PassiveTransformation<From = B, To = A>` is `X_AB` in the [monogram notation of Drake](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) and `^A_B T` in the notation of Craig (Introduction to Robotics, chapter 2): it maps the coordinates of a point in `B` to its coordinates in `A`.
* Passive transformations compose like Drake's, `X_AB * X_BC = X_AC`, i.e. `^A_C T = ^A_B T ^B_C T` in the notation of Craig, and the coordinate systems that meet must match.
* A motion and the change of coordinates to the coordinate system that it moves `From` onto cancel each other: the passive rotation to the coordinate system rotated by an active rotation `R` has the matrix `R^T`, the passive translation to the coordinate system translated by `t` the vector `-t`, and the passive proper rigid transformation to the coordinate system moved by `(R, t)` the rotation and translation `(R^T, -R^T t)`.
* The parameters of a passive rotation, e.g. its quaternion, describe its own matrix, in the convention of robotics: those of `PassiveSpatialRotation<From = B, To = A>` describe `R_AB`, the orientation of `B` relative to `A`, like Drake's `R_AB` of `q_AB`, kindr's `C_IB` of `q_IB` and Craig's `^A_B R` (see [Rotations](#rotations)).
  The Lorentz boost keeps the convention of physics: its velocity is that of `To` relative to `From` (see [Metrics and spacetime](#metrics-and-spacetime)).

## Translations and rigid transformations

* The vector of a passive translation is the vector from the origin of `To` to the origin of `From`, expressed in `To`, i.e. Drake's `p_AoBo_A`.
* A proper rigid transformation first rotates, then translates: `x -> R x + t`.
* In a passive proper rigid transformation, the rotation transforms from `From` into an intermediate coordinate system with the origin of `From` and the axes of `To`, and the translation from there into `To`, so the translation is expressed in `To`, like that of Drake's [RigidTransform](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html) `X_AB = (R_AB, p_AoBo_A)`.
* Rigid transformations compose as `(R1, t1) * (R2, t2) = (R1 R2, R1 t2 + t1)` and invert as `(R, t)^-1 = (R^T, -R^T t)` (Solà, Deray and Atchuthan, ["A micro Lie theory for state estimation in robotics"](https://arxiv.org/abs/1812.01537), eqs. 170 and 171).
* Translations leave vectors, covectors and tensors with two upper indices unchanged, move points, and reject the other tensors, which may depend on the origin.

## Rotations

The parameterizations, the manifold operations and the time derivatives of three-dimensional rotations (see [Parameterizations of rotations](concepts/transformations.md#parameterizations-of-rotations)):

* **Quaternions** follow Hamilton's convention, `ij = k`, with the real part first, `(w, x, y, z)`: the unit quaternion `q = (cos(θ/2), sin(θ/2) n)` rotates a vector `v` into `q v q^*`, by the right-handed angle `θ` around the axis `n` (Solà, ["Quaternion kinematics for the error-state Kalman filter"](https://arxiv.org/abs/1711.02508)), like Eigen, kindr, Drake and ROS, unlike the JPL convention.
  Its matrix is that of Eigen's `Quaternion::toRotationMatrix()`.
  `toQuaternion()` returns the unique quaternion, whose first element that is not zero is positive, like kindr's `getUnique()`, so its angle is in `[0, π]`.
* **Angle-axis and rotation vectors** are right-handed, with the angle in `[0, π]` when converted from a rotation; the rotation vector is the angle times the unit axis.
* **Euler angles** are intrinsic: ZYX `(yaw, pitch, roll)` is `R = R_z(yaw) R_y(pitch) R_x(roll)`, i.e. around `z`, then the new `y`, then the newest `x`, and XYZ `(a0, a1, a2)` is `R = R_x(a0) R_y(a1) R_z(a2)`, the extrinsic rotations in the opposite order.
  Converted from a rotation, the first and the last angle are in `[-π, π)` and the second in `[-π/2, π/2]`, like kindr's `getUnique()`; at the gimbal lock, a second angle of `±π/2`, only the sum or the difference of the others is defined.
* **The parameters of a passive rotation** describe its own matrix, in the convention of robotics: the parameters of `PassiveSpatialRotation<From = B, To = A>`, e.g. the quaternion `q_AB`, give its matrix `R_AB = R(q_AB)`, which maps the coordinates in `B` to those in `A`, the orientation of `B` relative to `A`, like Drake's `RotationMatrix` `R_AB` of `q_AB` ([monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html)), kindr's `C_IB` of `q_IB` and Craig's `^A_B R` (Introduction to Robotics, chapter 2).
  An active and a passive rotation with the same parameters have the same matrix.
* **The exponential and the logarithmic map** of SO(3) are `fromRotationVector()` and `toRotationVector()`.
* **boxPlus and boxMinus** perturb on the left, `R (+) v = Exp(v) R` and `R (-) S = Log(R S^T)`, the left plus and minus of Solà, Deray and Atchuthan, ["A micro Lie theory for state estimation in robotics"](https://arxiv.org/abs/1812.01537) (eqs. 27 and 28), with the increment in the coordinate system of an active rotation, or in `To` of a passive rotation, which equals `R Exp(v')` for the same vector `v'` expressed in `From`.
* **The Jacobian of the exponential map** is the left Jacobian of SO(3), `J(v) = I + (1 - cos|v|) / |v|^2 [v]x + (|v| - sin|v|) / |v|^3 [v]x^2`, so that `Exp(v + dv) = Exp(J dv) Exp(v)` to first order (Solà et al., eq. 145); for a rotation vector of a passive rotation expressed in `From`, it is the right Jacobian `J(-v) = J(v)^T`, so that `Exp(v + dv) = Exp(v) Exp(J dv)` (eq. 143).
* **Angular velocities** `w` are defined by `dR/dt = [w]x R`, for an active rotation expressed in its coordinate system; for a passive rotation, `w` is the angular velocity of `From` relative to `To`, expressed in `To`, and `dR/dt = R [w']x` for the same vector `w'` expressed in `From`. The derivative of a quaternion is `dq/dt = (0, w) q / 2 = q (0, w') / 2`.
* **The skew-symmetric matrix** of a vector is `[v]x = [[0, -v_z, v_y], [v_z, 0, -v_x], [-v_y, v_x, 0]]`, with `[v]x w = v x w`.
* **The pseudo-inverse** is the Moore-Penrose pseudo-inverse of a matrix of full rank, `A^T (A A^T)^-1` with fewer rows than columns and `(A^T A)^-1 A^T` with more.

### Comparison with kindr

[kindr](https://github.com/ANYbotics/kindr) 1.2, whose conventions the tests check (`tests/src/KindrConventions.cpp`), has passive rotations and Hamilton quaternions since its 1.0 (see its [cheat sheet](https://opensource.docs.anymal.com/doxygen/kindr/master/cheatsheet_latest.pdf)).
Its rotation `C_IB` maps the coordinates of a vector in `B` to those in `I`, `I_r = C_IB B_r`, and its parameters, e.g. the quaternion `q_IB`, are those of the matrix `C_IB`, like those of *wuzp*: `C_IB` is `PassiveSpatialRotation<From = B, To = I>` with the same parameters.

| Topic | kindr | *wuzp* |
| --- | --- | --- |
| Rotation | `C_IB`, `C_IB.rotate(B_r)` is `I_r`, `inverseRotate()` the inverse | `PassiveSpatialRotation<B, I>`, `X_IB` in Drake's notation: its `transform()` and the `transform()` of its `inverse()` |
| Parameters | `RotationQuaternion(w, x, y, z)` `q_IB`, `AngleAxis`, `RotationVector`, `EulerAnglesZyx(yaw, pitch, roll)`, `EulerAnglesXyz(roll, pitch, yaw)` | `fromQuaternion((w, x, y, z))`, `fromAngleAxis()`, `fromRotationVector()` (axis and vector in `I`, where their components are those in `B`), `fromEulerAnglesZyx((yaw, pitch, roll))`, `fromEulerAnglesXyz((roll, pitch, yaw))`: the same matrix |
| Active rotations | none | `ActiveSpatialRotation` with the same parameters has the same matrix |
| Composition | `C_IB * C_BC = C_IC` | `X_IB * X_BC = X_IC`, with the same parameters |
| Unique parameters | `getUnique()` of a quaternion and of Euler angles, with the angles in `[-π, π)`, `[-π/2, π/2)` and `[-π, π)` | `toQuaternion()` and `toEulerAngles...()` return them, with the second angle in `[-π/2, π/2]`; kindr's conversion into Euler angles returns the yaw in `[0, π]` (Eigen's `eulerAngles()`), until `getUnique()`. Within `1e-3` of the gimbal lock, kindr's `getUnique()` moves the yaw into the roll (ZYX) or the third angle into the first (XYZ), which changes the rotation; *wuzp* keeps the rotation, with the first angle 0 at an exact gimbal lock |
| Exponential and logarithmic map | `exponentialMap(v)`, `logarithmicMap()` | `fromRotationVector(v)`, `toRotationVector()` |
| boxPlus, boxMinus | `C.boxPlus(I_v) = Exp(I_v) C`, `C1.boxMinus(C2) = Log(C1 C2^-1)` in `I` | `boxPlus(v)` with `v` in `I` (`To`), or the same vector in `B` (`From`), e.g. `boxPlus(B_w_IB * dt)`; `boxMinus()` in `I`, `boxMinus<B>()` in `B` |
| Jacobian of the exponential map | `getJacobianOfExponentialMap(v)` | `exponentialMapJacobian(v)`, the left Jacobian |
| Angle between rotations | `getDisparityAngle()` | `angularDistance()` |
| Rotation between two vectors | `C_IB.setFromVectors(B_v, I_v)`, `I_v = C_IB B_v` | `PassiveSpatialRotation<B, I>::fromTwoVectors(B_v, I_v)` |
| Angular velocities | `LocalAngularVelocity` `B_w_IB`, `GlobalAngularVelocity` `I_w_IB` | The angular velocity of `B` relative to `I`, expressed in `B` (`From`) or `I` (`To`), which the coordinate system of the vector tells apart |
| Derivatives | `RotationQuaternionDiff`, `RotationMatrixDiff`, `EulerAnglesZyxDiff`, `EulerAnglesXyzDiff`, and the angular velocities of them | `quaternionDerivative()`, `matrixDerivative()`, `eulerAnglesZyxDerivative()`, `eulerAnglesXyzDerivative()`, and the `angularVelocityFrom...()`, in `To` by default; at the gimbal lock, kindr asserts, *wuzp* returns values that are not finite or arbitrarily large |
| Skew-symmetric matrix | `getSkewMatrixFromVector()`, `getVectorFromSkewMatrix()` | `skewSymmetricMatrix()`, `vectorOfSkewSymmetricMatrix()`, which averages the elements of each pair |
| Pseudo-inverse | `pseudoInverse()`, by a singular value decomposition, also of lower rank | `pseudoInverse()` of matrices of full rank, with checked units |
| Units | `AngularVelocity` and `Torque` are types of their own | Angles are dimensionless, so an angular velocity is in `1/s` and a torque in `N m` |
| Twists and wrenches | `Twist` and `Wrench`, the linear part first | `SpatialVelocity` and `SpatialForce`, the rotational part first, like Drake and Modern Robotics, with the frames and the point as types (see [Spatial vectors](#spatial-vectors)) |

### Comparison with Eigen

The tests compare with Eigen's geometry module too (`tests/src/EigenConventions.cpp`), with every value matrix backend; tf2 and Drake are not in the dev containers, so their conventions are not tested.

| Eigen | *wuzp* |
| --- | --- |
| `Quaternion(w, x, y, z)`, stored as `(x, y, z, w)` in `coeffs()` | `QuaternionT{w, x, y, z}` |
| `q.toRotationMatrix()`, `q * v` | the matrix and `transform()` of `fromQuaternion(q)`, of an active and of a passive rotation |
| `AngleAxis(angle, axis)` | `fromAngleAxis(angle, axis)` |
| `AngleAxis(z) * AngleAxis(y) * AngleAxis(x)`, `eulerAngles(2, 1, 0)` with the first angle in `[0, π]`, `canonicalEulerAngles(2, 1, 0)` (Eigen 5) | `fromEulerAnglesZyx()`, `toEulerAnglesZyx()` with the ranges of `canonicalEulerAngles()` |
| `Quaternion::FromTwoVectors(a, b)` | `fromTwoVectors(a, b)` |
| `angularDistance()` | `angularDistance()` |

## Spatial vectors

The spatial vectors follow Drake's [spatial vectors](https://drake.mit.edu/doxygen_cxx/group__multibody__spatial__vectors.html) and its [monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) (see [Spatial vectors](concepts/spatial-vectors.md)):

* `SpatialVelocity<M, B, P, E>` is `V_MBp_E`, the spatial velocity of the frame `B` measured in `M` about the point `Bp`, expressed in `E`, `SpatialAcceleration<M, B, P, E>` is `A_MBp_E`, and `SpatialForce<B, P, E>` is `F_Bp_E`, the spatial force on `B` about `Bp`.
  A point is the origin of a coordinate system, e.g. `Bo` is `B`.
* The rotational part comes first, `(ω, v)`, `(α, a)` and `(τ, f)`, like Drake, Modern Robotics (Lynch and Park, Cambridge University Press, 2017, sections 3.3.2 and 3.4) and Featherstone, unlike kindr.
* The shifts and compositions are those of Drake's `Shift()`, `ComposeWithMovingFrameVelocity()` and `ComposeWithMovingFrameAcceleration()`: `v_MBq = v_MBp + ω_MB × p_BpBq`, `a_MBq = a_MBp + α_MB × p_BpBq + ω_MB × (ω_MB × p_BpBq)` and `τ_Bq = τ_Bp - p_BpBq × f`; `ω_MC = ω_MB + ω_BC`, `v_MCq = v_MBp + ω_MB × p_BpCq + v_BCq`, `α_MC = α_MB + α_BC + ω_MB × ω_BC` and `a_MCq = a_MBp + α_MB × p_BpCq + ω_MB × (ω_MB × p_BpCq) + 2 ω_MB × v_BCq + a_BCq`.
  The note of Drake's `ComposeWithMovingFrameAcceleration()` starts the last sum with `a_BCo` instead of `a_MBo`, a typo that its derivation corrects.
* The spatial accelerations are Drake's, the derivatives of the velocities of points, not Featherstone's, the derivatives of the spatial velocities about a point fixed in `M`, which differ from them by `ω × v`.
* `X_AB.transform(V_MBp_B)` is `V_MBp_A`, rotated by `R_AB` about the same point, like a vector.
* The adjoint map of the passive proper rigid transformation `X_AB = (R_AB, p_AoBo_A)` is `[Ad_T] = [[R, 0], [[p] R, R]]` for twists about the origin of `B`, expressed in `B`, `V_s = [Ad_T_sb] V_b` in Modern Robotics, and its transposed inverse for wrenches, `F_s = [Ad_T_bs]^T F_b`: `ω' = R ω`, `v' = R v + p × (R ω)`, `f' = R f` and `τ' = R τ + p × (R f)`, i.e. `X_AB.transform(V).shift<A>(-p_AoBo_A)`.
* The angular velocity of `PassiveSpatialRotation<From = B, To = A>` is the rotational part of `SpatialVelocity<A, B, P, E>`, expressed in `E = A` or `E = B`.

## Metrics and spacetime

* The default metric of a coordinate system is Euclidean, the identity of orthonormal axes, where a vector and the covector it lowers to have the same components (Carroll, after eq. 1.62).
* `Minkowski` is `η = diag(1, -1, ..., -1)`, with the signature `(+, -, ..., -)` and the time component first, in length units (`x^0 = c t`).
  Carroll uses the signature `(-, +, +, +)`, whose metric is `-η`; both give the same Lorentz transformations, defined by `η = Λ^T η Λ` (eq. 1.14).
* The passive Lorentz boost has the standard form with `Λ_0i = -γ β_i`, for the velocity of `To` relative to `From`, expressed in `From`, in the convention of physics, unlike the parameters of passive rotations, which describe `From` relative to `To` (see [Rotations](#rotations)).
* The inverse of a Lorentz transformation is `η Λ^T η`, which follows from `η = Λ^T η Λ`; that of a rotation is `R^T`.

## Units

* Angles are dimensionless, with every units backend, like the SI treats them ([SI Brochure](https://www.bipm.org/en/publications/si-brochure), 9th edition, section 2.3.3).
* The units of a matrix that describes a linear map have the form `M_ij = a_i / b_j`, and those of its inverse are `(M^-1)_ij = b_i / a_j` (Hart, [Multidimensional Analysis](https://doi.org/10.1007/978-1-4612-4208-6), 1995, sections 3.1 and 3.3).
* The units of a Jacobian are `[x_i] / [q_j]` for the coordinates `q` of `From` and `x` of `To`.

## Comparisons

`isApprox(other, precision)` compares relatively, like Eigen's [`isApprox`](https://eigen.tuxfamily.org/dox/classEigen_1_1DenseBase.html#ae8443357b808cd393be1b51974213f9c): `||a - b|| <= p min(||a||, ||b||)`, in the Frobenius norm of the values.
Integer values compare exactly, whatever the precision, like with Eigen: the squares of their differences would overflow.
It cannot compare with zero, and for points it depends on the origin (see [Limitations](limitations.md)).
