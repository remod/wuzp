# Units and quantities

A *quantity* is a value with a unit, e.g. `wuzp_eigen::QuantityD<wuzp_units::Force>`, and *quantities* (`wuzp::Quantities`) are a matrix of values with a unit per element.
Sums, differences and comparisons require equal units, products and quotients compute the unit of the result:

```cpp
using wuzp_eigen::QuantityD;
const QuantityD<wuzp_units::Mass> mass{2.0};
const QuantityD<wuzp_units::Acceleration> acceleration{3.0};
const QuantityD<wuzp_units::Force> force = mass * acceleration;  // kg m / s^2 = N
// mass + acceleration;  // Error: Units do not match.
```

The units are types, which the compiler removes: they cost nothing at run time, only at compile time.

## Units

The units backend defines the units, e.g. `wuzp_units::Length`, `wuzp_boost::Length` or `wuzp_mp_units::Length`, with the same names in every backend: `Dimensionless`, `Angle`, `Length`, `Area`, `Volume`, `Time`, `Mass`, `Velocity`, `Acceleration`, `AngularVelocity`, `AngularAcceleration`, `Frequency`, `Force`, `Torque` and `Energy` (see [Backends](../backends.md)).
Other units are computed with the traits of the core:

```cpp
using MassLength = wuzp::Product<wuzp_units::Mass, wuzp_units::Length>::type;  // kg m
using LengthPerAngle = wuzp::Division<wuzp_units::Length, wuzp_units::Angle>::type;
```

*wuzp* checks units, it does not convert them: a quantity has the coherent SI unit of its dimension, e.g. `m`, so quantities in `m` and `km` cannot be mixed.
With mp-units, `wuzp_mp_units::UnitType<mp_units::si::newton>` accepts named units, but only coherent SI units.

**Angles are dimensionless**, with every units backend, like the SI treats them ([SI Brochure](https://www.bipm.org/en/publications/si-brochure), 9th edition, section 2.3.3): the angular velocity is `1/s` and the torque `N m`, so `v + ω × r` compiles without dividing by a radian, and code written for one units backend compiles with the others.
The price is that *wuzp* cannot tell `rad/s` from `Hz`, nor a torque from an energy (see [Limitations](../limitations.md) and the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#angles-are-dimensionless)).

## Heterogeneous units

Each element of quantities has its own unit, so a matrix can mix units, as long as the operations on it are consistent.
E.g. the mass matrix of a body with the coordinates `(x, y, φ)` has `kg` for the translations, `kg m^2` for the rotation and `kg m` for their coupling:

```cpp
using Mass = wuzp_units::Mass;
using MassLength = wuzp::Product<Mass, wuzp_units::Length>::type;
using MassLength2 = wuzp::Product<MassLength, wuzp_units::Length>::type;
using MassMatrixUnits = wuzp::TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
using MassMatrixQuantities = wuzp::Quantities<wuzp_eigen::ValueMatrix<double, 3, 3>, MassMatrixUnits>;
```

`TypeMatrix<Rows, Cols, Unit>` with a single unit gives all elements the same unit.
A matrix product requires all summands of an element to have the same unit, which holds for the matrices of linear maps, whose units have the form `M_ij = a_i / b_j` (Hart, [Multidimensional Analysis](https://doi.org/10.1007/978-1-4612-4208-6), 1995).
The inverse of such a matrix has the units `b_i / a_j`, and `inverse()` and `determinant()` reject matrices whose units do not have this form.

## Functions of quantities

* `pow<P>(q)` and `pow<P, Q>(q)` raise a quantity to a rational power, e.g. `pow<2>(length)` is an area and `pow<1, 2>(area)` a length.
  A negative value has a real power if the reduced denominator `Q` is odd, e.g. `pow<1, 3>()` of `-8 m^3` is `-2 m`; otherwise the power of a floating point value is NaN, like with `std::pow`, and that of an integer value throws `std::domain_error`.
* `sin()` and `cos()` take a dimensionless quantity with floating-point values, e.g. an angle, and return a dimensionless quantity.
* `isApprox(other, precision)` compares relatively, like Eigen's `isApprox` (see [Limitations](../limitations.md)).

## Value types

Quantities, tensors and transformations take the value type as a parameter, e.g. `double`, `float` or `int`.
An element of another arithmetic type is accepted if the common type of both is the value type: a vector of `double` can be constructed from `int`s, and `2 * v` is valid, while a vector of `int` rejects `1.5` and `2.5 * v`, which would truncate them.
Operations whose result is not an integer, e.g. the norm or `pow<1, 2>()`, are computed in floating point and truncated for integer values, like Eigen does; the matrix inverse of integer values is rejected.
