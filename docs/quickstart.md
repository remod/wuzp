# Quickstart

*wuzp* is header-only and needs a C++20 compiler and CMake 3.22 or newer.
The core, including *wuzp*'s own units, has no dependencies; each backend is an optional component with the library it adapts (see [Backends](backends.md)).

## Add wuzp to a CMake project

With CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html), which downloads *wuzp* when the project is configured:

```cmake
include(FetchContent)
FetchContent_Declare(wuzp GIT_REPOSITORY https://github.com/remod/wuzp.git GIT_TAG master)
FetchContent_MakeAvailable(wuzp)
target_link_libraries(my_target PRIVATE wuzp::wuzp wuzp::eigen)
```

Or install it, which installs the components whose dependencies were found, and use [`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html):

```bash
git clone https://github.com/remod/wuzp.git
cmake -S wuzp -B wuzp/build
cmake --install wuzp/build
```

```cmake
find_package(wuzp CONFIG REQUIRED COMPONENTS eigen)
target_link_libraries(my_target PRIVATE wuzp::wuzp wuzp::eigen)
```

By default, a backend is enabled if its dependency is found.
`-DWUZP_WITH_EIGEN=ON` requires it and `OFF` disables it, and likewise `WUZP_WITH_BLITZ`, `WUZP_WITH_BOOST` and `WUZP_WITH_MP_UNITS`.

| Component | Target | Dependency |
| --- | --- | --- |
| core, including *wuzp*'s own units | `wuzp::wuzp` | none |
| `eigen` | `wuzp::eigen` | Eigen 3.4 or 5 |
| `blitz` | `wuzp::blitz` | Blitz++ 1.0.2 or newer |
| `boost` | `wuzp::boost` | Boost 1.74 or newer (Boost.Units) |
| `mp_units` | `wuzp::mp_units` | mp-units 2.5 or newer, which needs GCC 12 or Clang 16 (but not 19) or newer |

## A first program

A program includes `<wuzp/wuzp.hpp>`, the core and the transformations, one units backend and one value matrix backend:

```cpp
#include <iostream>

#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>  // Or <wuzp_blitz/ValueMatrix.hpp>.
#include <wuzp_units/Unit.hpp>         // Or <wuzp_boost/Unit.hpp>, <wuzp_mp_units/Unit.hpp>.

using Mass = wuzp_eigen::QuantityD<wuzp_units::Mass>;
using Acceleration = wuzp_eigen::QuantityD<wuzp_units::Acceleration>;
using Force = wuzp_eigen::QuantityD<wuzp_units::Force>;

int main() {
  const Mass mass{2.0};
  const Acceleration acceleration{3.0};
  // const Force force{mass};  // Error: Units do not match.
  const Force force = mass * acceleration;
  std::cout << "Force: " << force << std::endl;
  return 0;
}
```

A quantity is a value with a unit; `QuantityD<Unit>` has a `double` value, `QuantityF` a `float` and `QuantityI` an `int`.
The value matrix backend defines them, e.g. `wuzp_eigen::QuantityD`, since the value is stored in a matrix of its library.

Vectors, points and the other tensors are expressed in a coordinate system, which is a type named after an identifier of your own:

```cpp
struct World {};
using WorldFrame = wuzp::CoordinateSystem<World>;
using Position = wuzp::PositionPoint3<wuzp_eigen::ValueMatrix, double, WorldFrame>;
using Displacement = wuzp::PositionVector3<wuzp_eigen::ValueMatrix, double, WorldFrame>;

const Position start{1.0, 2.0, 3.0};
const Position end{4.0, 6.0, 3.0};
const Displacement displacement = end - start;  // The difference of two points is a vector.
const auto distance = displacement.norm();      // A length, 5 m.
```

The operations return evaluated results, so storing them in `auto` is safe.

## Examples and tests

The [examples](https://github.com/remod/wuzp/tree/master/examples) are built with the project if the Eigen backend is enabled:

```bash
cmake --preset gcc-wuzp-eigen
cmake --build --preset gcc-wuzp-eigen
./build/gcc-wuzp-eigen/examples/example_transformation
```

The [unit tests](https://github.com/remod/wuzp/tree/master/tests/src) cover every feature, and the [compile tests](https://github.com/remod/wuzp/tree/master/tests/compile_tests/cases) show what does not compile, each with the error message it expects.
