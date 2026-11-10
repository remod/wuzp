#pragma once

#include <concepts>
#include <type_traits>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

/**
 * The Euclidean metric delta_ij of a coordinate system with orthonormal axes, e.g. of space in Cartesian coordinates, and the
 * default metric of coordinate systems. It is the identity matrix, so the components of a vector and of the covector it lowers
 * to are equal (Carroll, Lecture Notes on General Relativity, https://arxiv.org/abs/gr-qc/9712019, the text after eq. 1.62), and
 * dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector compute with the components alone (see
 * Metric).
 */
struct Euclidean {};

/**
 * The Minkowski metric eta = diag(1, -1, ..., -1) of an inertial frame of special relativity, with the signature (+, -, ..., -)
 * and the time component first, in length units (x^0 = c t), like LorentzTransformation. The operations that assume the Euclidean
 * metric are rejected in such a coordinate system; the pairing of a covector and a vector, innerProduct, needs no metric, and
 * getInvariant() computes the interval eta_ij v^i v^j (Carroll, Lecture Notes on General Relativity,
 * https://arxiv.org/abs/gr-qc/9712019, eqs. 1.55 and 1.63).
 */
struct Minkowski {};

//! The metric of a coordinate system: Euclidean or Minkowski.
template <typename T>
concept EuclideanOrMinkowski = std::same_as<T, Euclidean> || std::same_as<T, Minkowski>;

/**
 * A coordinate system: axes, an origin and a metric, as a type without runtime cost. Operations combining tensors require them
 * to be expressed in the same coordinate system.
 * @tparam IdentifierT_ A type that tells coordinate systems apart, e.g. `struct World {};` for CoordinateSystem<World>.
 * @tparam MetricT_ The metric: Euclidean, the default, for space, or Minkowski for spacetime, e.g. CoordinateSystem<Spacetime,
 * Minkowski> (see Metric). The metric is part of the type: coordinate systems with the same identifier and different metrics do
 * not match.
 */
template <typename IdentifierT_, EuclideanOrMinkowski MetricT_ = Euclidean>
struct CoordinateSystem {};

/**
 * The metric of a coordinate system, Euclidean or Minkowski, its second template parameter, which is Euclidean by default. A dot
 * product needs a metric, g_ij v^i w^j (Carroll, Lecture Notes on General Relativity, https://arxiv.org/abs/gr-qc/9712019, eq.
 * 1.55): the Euclidean formula sum_i v_i w_i is only right in orthonormal bases of a Euclidean space, while the pairing w_i v^i of a
 * covector and a vector needs none (eq. 1.35). So dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector
 * or a covector, which lowers or raises its index, require the Euclidean metric, and getInvariant() the Minkowski metric. The
 * metric is constant, the identity for orthonormal axes or eta for an inertial frame. Other metrics are not modeled: those of
 * axes that are not orthonormal, e.g. To of a passive linear transformation that is not orthogonal from a Euclidean coordinate
 * system, and those of curvilinear coordinates, which depend on the position, e.g. of polar coordinates. Such coordinate systems
 * are Euclidean by default, so their dot product is wrong (see "Metric" in DESIGN.md).
 */
template <typename CoordinateSystemT_>
struct Metric;
template <typename IdentifierT_, typename MetricT_>
struct Metric<CoordinateSystem<IdentifierT_, MetricT_>> {
  using type = MetricT_;
};

namespace internal {

//! The coordinate system of a combination of objects expressed in the same coordinate system, e.g. of their sum.
template <typename LhsT_, typename RhsT_>
struct SameCoordinateSystem {
  static_assert(std::is_same_v<LhsT_, RhsT_>, "Coordinate systems do not match.");
  using type = LhsT_;
};

}  // namespace internal

template <typename IdentifierLhsT_, typename MetricLhsT_, typename IdentifierRhsT_, typename MetricRhsT_>
struct Sum<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>>
    : internal::SameCoordinateSystem<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>> {};

template <typename IdentifierLhsT_, typename MetricLhsT_, typename IdentifierRhsT_, typename MetricRhsT_>
struct Difference<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>>
    : internal::SameCoordinateSystem<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>> {};

template <typename IdentifierLhsT_, typename MetricLhsT_, typename IdentifierRhsT_, typename MetricRhsT_>
struct Product<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>>
    : internal::SameCoordinateSystem<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>> {};

template <typename IdentifierLhsT_, typename MetricLhsT_, typename IdentifierRhsT_, typename MetricRhsT_>
struct Division<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>>
    : internal::SameCoordinateSystem<CoordinateSystem<IdentifierLhsT_, MetricLhsT_>, CoordinateSystem<IdentifierRhsT_, MetricRhsT_>> {};

}  // namespace wuzp
