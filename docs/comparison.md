# Comparison with mp-units and Boost.Units

[mp-units](https://mpusz.github.io/mp-units/) and [Boost.Units](https://www.boost.org/doc/libs/release/libs/units/) are units libraries: they check the dimensions and units of quantities, and convert between units.
*wuzp* checks units too, but it is not a replacement for them: it reuses them as units backends, and adds what a units library does not model, the linear algebra of quantities in coordinate systems.

| | *wuzp* | mp-units | Boost.Units |
| --- | --- | --- | --- |
| Dimensions and units of scalars | yes (own units, or those of mp-units or Boost.Units) | yes | yes |
| Unit conversions, prefixes, non-SI units | no: coherent SI units only | yes | yes |
| A unit per element of a matrix, e.g. a mass matrix with `kg` and `kg m^2` | yes | no | no |
| Matrices and linear algebra | Eigen or Blitz++, with checked units | quantities of a vector or tensor *character*, with a representation type of their own, one unit for all components ([character of a quantity](https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/character_of_a_quantity/)) | no |
| Points and vectors (affine spaces) | yes, for vectors in any dimension | yes, quantity points with origins ([the affine space](https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/the_affine_space/)) | `absolute<>` quantities |
| Coordinate systems of tensors | yes, checked at compile time | no | no |
| Index variance (vectors vs. covectors) | yes | no | no |
| Rotations, rigid and Lorentz transformations, Jacobians | yes, active and passive | no | no |
| Kinds of quantities, e.g. telling `Hz` from `rad/s` | no: angles are dimensionless | yes, quantity kinds and hierarchies | angles are a dimension of their own |

## When to use which

* For scalar physical quantities with unit conversions, use mp-units or Boost.Units directly.
* For vectors, matrices and transformations in robotics, physics or graphics, where mixing up coordinate systems, points and vectors, or vectors and covectors is the common mistake, *wuzp* adds these checks on top of Eigen or Blitz++.
  If your code already uses mp-units or Boost.Units, use them as the units backend of *wuzp*, so the units are the same types.

## Differences in detail

**Angles.**
Boost.Units treats the plane angle as a dimension of its own, and mp-units keeps the radian as a unit apart from one.
*wuzp* treats angles as dimensionless with every backend, like the SI ([SI Brochure](https://www.bipm.org/en/publications/si-brochure), section 2.3.3), so that formulas such as `v + ω × r` and torque in `N m` compile without dividing by a radian, and code compiles with every units backend.
The price is that *wuzp* cannot tell `rad/s` from `Hz`, nor a torque from an energy (see the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#angles-are-dimensionless)).

**Points.**
mp-units and Boost.Units have points for scalars, e.g. temperatures and time points, with explicit origins.
*wuzp*'s points are tensors with one upper index in a coordinate system, whose origin is that of the coordinate system: a time point is a one-dimensional point, and a change of the origin is a translation.

**Products.**
mp-units defines `*` of vector quantities only with a scalar and names the other products, which *wuzp* does too: `innerProduct`, `outerProduct`, `crossProduct` and `cwiseProduct` (see [Tensors and index variance](concepts/tensors.md)).

**Other libraries.**
[Drake](https://drake.mit.edu/) names the frames of its quantities and transformations in its [monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html), a convention for variable names, and [wave_geometry](https://github.com/wavelab/wave_geometry) checks the frames of transformations in their types, at compile time.
*wuzp* follows Drake's notation for passive transformations and checks the frames at compile time, like wave_geometry, together with units, points and vectors, and index variance.
