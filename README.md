# Wrong Unit? Zero Points!

*wuzp* is a C++20 library which extends existing numeric algebra libraries with

* unit type safety,
* coordinate system type safety, and
* transformations.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CI](https://github.com/remod/wuzp/actions/workflows/ci.yml/badge.svg)](https://github.com/remod/wuzp/actions/workflows/ci.yml)
[![Code Coverage](https://codecov.io/gh/remod/wuzp/branch/master/graph/badge.svg?token=FVPNHZKXVX)](https://codecov.io/gh/remod/wuzp)

## Type Safety

Type safety speeds up development because your IDE, your compiler will report any mistakes without you having to write and run unit tests.

*wuzp* provides compile time type safety for units and coordinate systems.

It prevents errors like mixing quantities with incompatible units ([code example](examples/quantity.cpp)):

```cpp
force = mass  // Error: Mismatching units!
force = mass * acceleration  // OK.
```

While relative quantities can be operated on freely, there are some restrictions for absolute quantities.[^1]
Absolute quantities are points, relative quantities are vectors, and points follow the rules of affine spaces, like [CGAL's points](https://doc.cgal.org/latest/Kernel_23/index.html) and [mp-units' quantity points](https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/the_affine_space/): only the operations whose result does not depend on the origin of the coordinate system compile ([code example](examples/absolute_quantity.cpp), [DESIGN.md](DESIGN.md#points-and-vectors)).
For example, it does not make sense to add two absolute quantities, or to scale one:

```cpp
time_point + time_point      // Error: Two absolute objects cannot be summed.
time_point * 2.0             // Error: An absolute object cannot appear in a multiplication.
time_point + duration        // OK: a time point.
end - start                  // OK: a duration.
start + (end - start) * 0.5  // OK: the time point halfway between start and end.
```

An absolute scalar, e.g. a time point, is a point in a one-dimensional coordinate system whose origin is its zero, e.g. an epoch.
`makeRelative(point)` is the vector from the origin of the coordinate system to the point, and `makeAbsolute(vector)` the point at the vector from the origin.

When operating with different coordinate systems, it is crucial to prevent mixing them up:

```cpp
force_in_A + force_in_B  // Error: Mismatching coordinate systems!
force_in_A + transformation_B_to_A.transform(force_in_B)  // OK.
```

Tensors know whether each of their indices is upper (contravariant, e.g. a velocity) or lower (covariant, e.g. a gradient), following [Carroll](https://arxiv.org/abs/gr-qc/9712019), so only products that are tensors compile ([DESIGN.md](DESIGN.md#products-of-tensors)):

```cpp
velocity * velocity                // Error: A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0.
innerProduct(velocity, velocity)   // Error: An inner product contracts an upper with a lower index, ...
cwiseProduct(velocity, velocity)   // OK: the elementwise product, as quantities without a coordinate system.
2 * mass * acceleration            // OK.
innerProduct(innerProduct(massMatrix, velocity), velocity) / 2  // OK: the kinetic energy M_ij v^i v^j / 2.
```

**Note:** While a single object can only have a single coordinate system, it is possible to assign it a heterogeneous set of units.
This is required e.g. to represent mass matrices, which contain `kg` for translational degrees of freedom and `kg m^2` for rotational degrees of freedom.

## Transformations

In general, a transformation can be one or a combination of the following base transformations:

* Rotation
* Translation
* Reflection
* Dilation

*wuzp* supports the following transformations as of today:

* Rotation
* Translation
* Proper rigid transformation (= first rotation, then translation)
* Jacobian
* Lorentz transformation

These transformations are supported in their **active** and **passive** form.
In the **active** case the object is transformed, resulting in a new object.
In the **passive** case there is only a single object, but the coordinate system which it is expressed in is transformed.[^2]

As mentioned above, *wuzp* distinguishes between absolute and relative quantities.
In geometric terms, absolute positions can be interpreted as points, while relative positions can be interpreted as vectors.
This distinction becomes important when transforming points and vectors:
Unlike points, vectors are agnostic to their position and therefore unaffectd by translations.

The following table visualizes how a proper rigid transformation transforms a point and a vector in the active and passive case:

| | Target: Point | Target: Vector |
| --- | --- | --- |
| Type:<br>Active | <img src="doc/active_proper_rigid_transformation_of_point.svg" alt="Active Proper Rigid Transformation of Point" height="200"/><br>P is first rotated, then translated into P'. | <img src="doc/active_proper_rigid_transformation_of_vector.svg" alt="Active Proper Rigid Transformation of Vector" height="200"/><br>*r* is first rotated, then translated into *r'*. As a vector is agnostic to its position, only the rotational part of the transformation affects its components. |
| Type:<br>Passive | <img src="doc/passive_proper_rigid_transformation_of_point.svg" alt="Passive Proper Rigid Transformation of Point" height="200"/><br>P is expressed in coordinate system A, then in coordinate system B. | <img src="doc/passive_proper_rigid_transformation_of_vector.svg" alt="Active Proper Rigid Transformation of Vector" height="200"/><br>*r* is expressed in coordinate system A, then in coordinate system B. As a vector is agnostic to its position, only the rotational part of the transformation affects its components. |

Transformations compose with `*` and have an `inverse()` ([code example](examples/transformation.cpp), [DESIGN.md](DESIGN.md#composition-and-inverse-of-transformations)).
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

Rotations and Lorentz transformations check that they are valid, e.g. that the matrix of a rotation is orthogonal with determinant 1, in debug builds, like [Drake's](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html), and throw `std::invalid_argument` otherwise (see [DESIGN.md](DESIGN.md#validity-of-rotations-and-lorentz-transformations)):

```cpp
Rotation{1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}  // Throws in debug builds: a shear is not a rotation.
Rotation::makeUnchecked(matrix)                        // OK: not checked, e.g. in hot paths.
Rotation::projectToRotation(matrix)                    // OK: the nearest rotation, e.g. after rounding errors.
rotation.inverse()                                     // OK: the transpose, with every backend.
```

## Installation & Usage

*wuzp* integrates with the following unit and linear algebra libraries:

**Units:** 

* *wuzp*' own implementation
* [Boost](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units.html)
* [mp-units](https://mpusz.github.io/mp-units/)

**Linear Algebra:**

* [Eigen](http://eigen.tuxfamily.org/)
* [Blitz++](https://github.com/blitzpp/blitz)

*wuzp* is header-only and requires a C++20 compiler and CMake 3.22 or newer.
It is tested on Linux with GCC 11 to 15 and Clang 14 to 21, on macOS with Apple Clang, and on Windows with MSVC (on a best-effort basis); the `mp_units` component only on Linux, with GCC 15 and Clang 21.
Each backend is an optional component with its own dependency:

| Component | Target | Dependency (supported versions) |
| --- | --- | --- |
| core, including *wuzp*' own units | `wuzp::wuzp` | none |
| `eigen` | `wuzp::eigen` | Eigen 3.4 and 5 |
| `blitz` | `wuzp::blitz` | Blitz++ 1.0.2 or newer |
| `boost` | `wuzp::boost` | Boost 1.74 or newer |
| `mp_units` | `wuzp::mp_units` | mp-units 2.5 or newer, which needs GCC 12, Clang 16 (but not 19) or newer |

**Using CMake's FetchContent:**

```cmake
include(FetchContent)
FetchContent_Declare(wuzp GIT_REPOSITORY https://github.com/remod/wuzp.git GIT_TAG master)
FetchContent_MakeAvailable(wuzp)
target_link_libraries(my_target PRIVATE wuzp::wuzp wuzp::eigen)
```

**Installing and using `find_package`:**

```bash
git clone https://github.com/remod/wuzp.git
cmake -S wuzp -B wuzp/build
cmake --install wuzp/build  # Installs the components whose dependencies were found.
```

```cmake
find_package(wuzp CONFIG REQUIRED COMPONENTS eigen)
target_link_libraries(my_target PRIVATE wuzp::wuzp wuzp::eigen)
```

By default, a backend is enabled if its dependency is found.
Use `-DWUZP_WITH_EIGEN=ON|OFF` (and likewise `BLITZ`, `BOOST`, `MP_UNITS`) to require or disable one.

In the code, include `<wuzp/wuzp.hpp>` (the core and the transformations), a units backend and a value matrix backend:

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>  // Or <wuzp_blitz/ValueMatrix.hpp>.
#include <wuzp_units/Unit.hpp>         // Or <wuzp_boost/Unit.hpp>, <wuzp_mp_units/Unit.hpp>.

struct A {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;
using Force = wuzp_eigen::QuantityD<wuzp_units::Force>;
using Position = wuzp::PositionVector3<wuzp_eigen::ValueMatrix, double, CoordinateSystemA>;
```

The value matrix backend defines the quantities with a value type, e.g. `wuzp_eigen::QuantityD<Unit>` (also `QuantityI` and `QuantityF`), and the tensor aliases take it as a parameter, e.g. `wuzp_eigen::ValueMatrix`.
A program can use several value matrix backends, also in one translation unit, but values of different backends cannot be combined.
A program uses exactly one units backend: using two of them, also in different translation units, violates the [one definition rule](https://en.cppreference.com/w/cpp/language/definition#One_Definition_Rule).

To adapt another library, specialize the traits of the core like the existing backends do: the concepts `ValueMatrixBackend`, `InvertibleValueMatrixBackend` and `UnitSystem` in [`wuzp/core/concepts.hpp`](wuzp/include/wuzp/core/concepts.hpp) list what the core needs (see "The backend interface" in [DESIGN.md](DESIGN.md#the-backend-interface)).
Check them in the tests of the backend, e.g. `static_assert(wuzp::ValueMatrixBackend<my_backend::ValueMatrix>);` or `static_assert(wuzp::UnitSystem<wuzp::AccessUnits<wuzp::AccessImplementation<double>>>);`, rather than in its headers, where the check would cost compile time in every translation unit of its users.

## Examples

Examples demonstrating how to use *wuzp* can be found in the `examples` folder.
They are built together with the project if the Eigen backend is enabled, e.g.:

```bash
cmake --preset gcc-wuzp-eigen
cmake --build --preset gcc-wuzp-eigen
./build/gcc-wuzp-eigen/examples/example_transformation
```

For more example code, have a look at the unit tests in the `tests` folder.

## Performance

All checks happen at compile time.
With Eigen, *wuzp* is as fast as plain Eigen; with Blitz++, it is slower for now ([#58](https://github.com/remod/wuzp/issues/58)).
With Eigen, the same computations take about 20-45% longer to compile.
See the [benchmarks](benchmarks/README.md) for the numbers.

## Limitations

Tensors have rank 0, 1 or 2, e.g. the outer product of two vectors is supported, but not that of a vector and a matrix.

Angles are dimensionless with every units backend, so *wuzp* cannot tell `rad/s` from `Hz`, nor torque from energy (see [DESIGN.md](DESIGN.md#angles-are-dimensionless)).

*wuzp* does not convert units: quantities with different units, e.g. `m` and `km`, cannot be added.
With mp-units, `wuzp_mp_units::UnitType` accepts named units such as `si::newton`, but only coherent SI units, e.g. not kilometres, grams or degrees.

The results of operations are evaluated, so they can be stored in `auto` variables, also when they are computed from temporaries.
Only the expressions of the value matrix libraries themselves, e.g. Eigen expressions built from `getImplementation()`, can refer to temporaries (see [Eigen's pitfalls](https://eigen.tuxfamily.org/dox/TopicPitfalls.html)).

Blitz++ cannot be installed with `libblitz0-dev` when compiled with Clang, because the Debian package does not contain a required header file.
Instead, it needs to be cloned from GitHub and then built and installed manually (see [GitHub issue](https://github.com/blitzpp/blitz/issues/177)).

## Contributing

Contributions are welcome! [CONTRIBUTING.md](CONTRIBUTING.md) describes the development environment, the tests and the CI, and [CHANGELOG.md](CHANGELOG.md) lists the changes.

[^1]: Inspired by [Boost units](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units/Examples.html#boost_units.Examples.AbsoluteRelativeTemperatureExample).

[^2]: The difference between active and passive transformations is explained [here](https://en.wikipedia.org/wiki/Active_and_passive_transformation).