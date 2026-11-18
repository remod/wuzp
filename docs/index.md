# Wrong Unit? Zero Points!

*wuzp* is a header-only C++20 library that adds compile-time checks to the linear algebra libraries [Eigen](https://eigen.tuxfamily.org/) and [Blitz++](https://github.com/blitzpp/blitz):

* **Units.** A force cannot be initialized with a mass, only with a mass times an acceleration, and every element of a matrix has its own unit, so heterogeneous matrices such as a mass matrix with `kg` and `kg m^2` entries are checked too.
* **Coordinate systems.** Every tensor is expressed in a coordinate system, a type without runtime cost: adding a force expressed in `A` to a force expressed in `B` does not compile, until a transformation expresses one in the coordinate system of the other.
* **Points and vectors.** Points follow the rules of affine spaces: `p - q` is a vector, `p + v` a point, and operations whose result depends on the origin, e.g. `p + q` or `2 * p`, do not compile.
* **Index variance.** Tensors know whether each index is upper (a vector) or lower (a covector), so only contractions of an upper with a lower index compile, and a covector is not mistaken for a vector.
* **Transformations.** Rotations, translations and proper rigid transformations, active and passive, Lorentz transformations and Jacobians, which compose with `*` in the order of [Drake's monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) and have an `inverse()`.

All checks happen at compile time: with Eigen, *wuzp* is as fast as plain Eigen (see the [benchmarks](https://github.com/remod/wuzp/blob/master/benchmarks/README.md)).

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

struct A {};
struct B {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;
using CoordinateSystemB = wuzp::CoordinateSystem<B>;
using ForceInA = wuzp::ForceVector3<wuzp_eigen::ValueMatrix, double, CoordinateSystemA>;
using ForceInB = wuzp::ForceVector3<wuzp_eigen::ValueMatrix, double, CoordinateSystemB>;
using RotationFromAToB = wuzp::PassiveSpatialRotation3<wuzp_eigen::ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;

int main() {
  const ForceInA forceInA{1.0, 2.0, 3.0};
  const ForceInB forceInB{4.0, 5.0, 6.0};
  const RotationFromAToB rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  // const ForceInB sum = forceInA + forceInB;  // Error: Coordinate systems do not match.
  const ForceInB sum = rotation.transform(forceInA) + forceInB;
  return 0;
}
```

## Where to go next

* [Quickstart](quickstart.md): add *wuzp* to a CMake project and compile a first program.
* Concepts: [units and quantities](concepts/units.md), [points and vectors](concepts/points-and-vectors.md), [coordinate systems and metrics](concepts/coordinate-systems.md), [tensors and index variance](concepts/tensors.md), and [transformations](concepts/transformations.md).
* [Backends](backends.md): the units and linear algebra libraries *wuzp* works with, and how to choose one.
* [Cookbook](cookbook/rigid-body-kinematics.md): worked examples from rigid body kinematics and Lagrangian mechanics.
* [Conventions](conventions.md): the transformation rules, the index placement and the notation, with their sources.
* [Comparison with mp-units and Boost.Units](comparison.md), [class hierarchy](class-hierarchy.md), [limitations](limitations.md), and the [API reference](api/index.html).

The [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md) record why *wuzp* works the way it does, with the sources of its conventions and the results of a review of its mathematical and physical soundness.
