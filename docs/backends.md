# Backends

*wuzp* does not do arithmetic itself: a *value matrix backend* stores and computes the values, and a *units backend* defines the units.
The core only adds the checks, through traits that each backend specializes for its library.

| Kind | Backend | Header | Namespace | Library |
| --- | --- | --- | --- | --- |
| Units | *wuzp*'s own units | `<wuzp_units/Unit.hpp>` | `wuzp_units` | none: seven SI exponents as reduced rationals |
| Units | Boost.Units | `<wuzp_boost/Unit.hpp>` | `wuzp_boost` | [Boost.Units](https://www.boost.org/doc/libs/release/libs/units/), 1.74 or newer |
| Units | mp-units | `<wuzp_mp_units/Unit.hpp>` | `wuzp_mp_units` | [mp-units](https://mpusz.github.io/mp-units/), 2.5 or newer |
| Values | Eigen | `<wuzp_eigen/ValueMatrix.hpp>` | `wuzp_eigen` | [Eigen](https://eigen.tuxfamily.org/), 3.4 or 5 |
| Values | Blitz++ | `<wuzp_blitz/ValueMatrix.hpp>` | `wuzp_blitz` | [Blitz++](https://github.com/blitzpp/blitz), 1.0.2 or newer |

Every feature works with every combination of a units and a value matrix backend: the tests run against all of them, which also proves that the core does not depend on one implementation.

## Choosing a backend

**Units.**
*wuzp*'s own units need no dependency and are the default.
Use Boost.Units or mp-units if your code already uses them: the units of *wuzp* then wrap the units of that library, e.g. `wuzp_boost::Length` is `wuzp::Unit<boost::units::si::length>`.
mp-units needs newer compilers (GCC 12 or Clang 16, but not 19), and is tested on Linux only.

A program uses exactly one units backend: using two of them, also in different translation units, violates the [one definition rule](https://en.cppreference.com/w/cpp/language/definition#One_Definition_Rule).

**Values.**
Eigen is the choice for performance: with it, *wuzp* is as fast as plain Eigen (see the [benchmarks](https://github.com/remod/wuzp/blob/master/benchmarks/README.md)).
With Blitz++, every evaluated result allocates an array, so it is slower ([#58](https://github.com/remod/wuzp/issues/58)); it computes matrix inverses and determinants by Gauss-Jordan elimination with partial pivoting, without LAPACK.

A program can use several value matrix backends, also in one translation unit, but values of different backends cannot be combined.
The value matrix backend defines the quantities with a value type, e.g. `wuzp_eigen::QuantityD<Unit>`, and the tensor aliases and transformations take its `ValueMatrix` as a parameter:

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_blitz/ValueMatrix.hpp>
#include <wuzp_boost/Unit.hpp>

using Force = wuzp_blitz::QuantityD<wuzp_boost::Force>;
using Position = wuzp::PositionVector3<wuzp_blitz::ValueMatrix, double, WorldFrame>;
```

## Platforms

*wuzp* is tested on Linux with GCC 11 to 15 and Clang 14 to 21, on macOS with Apple Clang, and on Windows with MSVC (on a best-effort basis), with every backend combination except mp-units, which is only tested on Linux with GCC 15 and Clang 21.

## Writing a backend

To adapt another library, specialize the traits of the core like the existing backends do.
The concepts in [`wuzp/core/concepts.hpp`](https://github.com/remod/wuzp/blob/master/wuzp/include/wuzp/core/concepts.hpp) list what the core needs: `ValueMatrixBackend`, `InvertibleValueMatrixBackend` and `UnitSystem`.
[Writing a backend](https://github.com/remod/wuzp/blob/master/CONTRIBUTING.md#writing-a-backend) explains what a backend provides and how to check it, with minimal example backends, the archetypes, which provide exactly what the concepts require.

Check the concepts in the tests of the backend, rather than in its headers, where the check would cost compile time in every translation unit of its users:

```cpp
static_assert(wuzp::ValueMatrixBackend<my_backend::ValueMatrix>);
static_assert(wuzp::UnitSystem<wuzp::AccessUnits<wuzp::AccessImplementation<double>>>);
```
