# Limitations

**Rank.**
Tensors have rank 0, 1 or 2: e.g. the outer product of two vectors is supported, but not that of a vector and a matrix.

**Angles are dimensionless**, with every units backend, like the SI treats them, so *wuzp* cannot tell `rad/s` from `Hz`, nor a torque from an energy, and `sin()` and `cos()` take any dimensionless quantity with floating-point values (see the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#angles-are-dimensionless)).

**Coordinate systems** are axes, an origin and a constant metric, Euclidean, i.e. orthonormal axes, or Minkowski.
Other metrics, e.g. of axes that are not orthonormal or of curvilinear coordinates, whose metric depends on the position, are not modeled (see [Coordinate systems and metrics](concepts/coordinate-systems.md)).

**Moving frames** are not modeled: a passive transformation of a velocity re-expresses it, without adding the velocity of the frame or `ω × r`, and quantities about a point, e.g. a torque, are not shifted to another point.
Write the transport formulas where they are needed (see [Rigid body kinematics](cookbook/rigid-body-kinematics.md) and the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#moving-frames)).

**Cross products** return vectors, which is right under proper rotations in right-handed orthonormal bases, but not under reflections or other linear transformations, which must transform the factors instead (see [Tensors and index variance](concepts/tensors.md#cross-product)).

**Approximate comparison.**
`isApprox()` compares relatively, like Eigen's, so it cannot compare with zero, and for points it depends on the origin: compare the distance of points with an absolute tolerance instead, e.g. `(p - q).norm().getValue() <= 1e-9`.

**Powers of negative quantities.**
`pow<1, 3>()` of a negative quantity is its real cube root, like `std::cbrt`, but `pow<1, 2>()` of a negative quantity has no real result: it is NaN for floating point values, like `std::pow`, and throws `std::domain_error` for integer values.

**Unit conversions.**
*wuzp* does not convert units: quantities with different units, e.g. `m` and `km`, cannot be added.
With mp-units, `wuzp_mp_units::UnitType` accepts named units such as `si::newton`, but only coherent SI units, e.g. not kilometres, grams or degrees.

**Expressions of the value matrix libraries.**
The results of *wuzp*'s operations are evaluated, so they can be stored in `auto` variables, also when they are computed from temporaries.
Only the expressions of the value matrix libraries themselves, e.g. Eigen expressions built from `getImplementation()`, can refer to temporaries (see [Eigen's pitfalls](https://eigen.tuxfamily.org/dox/TopicPitfalls.html)).

**Blitz++** is slower than Eigen, since every evaluated result allocates an array ([#58](https://github.com/remod/wuzp/issues/58)).
It cannot be installed with the Debian package `libblitz0-dev` when compiled with Clang, because the package does not contain a required header file: build and install it from [GitHub](https://github.com/blitzpp/blitz) instead ([blitzpp/blitz#177](https://github.com/blitzpp/blitz/issues/177)).

**Compile time.**
With Eigen, the same computations take about 20-45% longer to compile than with plain Eigen (see the [benchmarks](https://github.com/remod/wuzp/blob/master/benchmarks/README.md#compile-time)).
