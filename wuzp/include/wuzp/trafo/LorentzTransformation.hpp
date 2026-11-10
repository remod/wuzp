#pragma once

#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/internal/validity.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

namespace internal {

//! The Minkowski metric eta = diag(1, -1, ..., -1) with the signature (+, -, ..., -), with the time component first, as values of
//! the type ValuesT_.
template <typename ValuesT_>
ValuesT_ minkowskiMetric() {
  auto result = identity<ValuesT_>();
  for (int index = 1; index < Rows<ValuesT_>::value; ++index) {
    result.set(index, index, typename ValuesT_::ValueT{-1});
  }
  return result;
}

}  // namespace internal

/**
 * A Lorentz transformation Lambda from the inertial frame From to the inertial frame To: the dimensionless passive linear
 * transformation of spacetime vectors, with the time component first and in length units (x^0 = c t), that preserves the
 * Minkowski metric eta = diag(1, -1, ..., -1): Lambda^T eta Lambda = eta (Carroll, Lecture Notes on General Relativity,
 * https://arxiv.org/abs/gr-qc/9712019, eq. 1.14, whose signature (-, +, +, +) has the metric -eta, which gives the same
 * condition), e.g. a boost (see createBoostMatrix()), a spatial rotation, or their compositions. It transforms the same tensors
 * as the passive linear transformation with its matrix (see PassiveLinearTransformation). It composes with Lorentz
 * transformations into Lorentz transformations, with linear transformations into linear transformations, and with Jacobians into
 * Jacobians (see CompositionTraits). Its inverse is eta Lambda^T eta, which follows from eta = Lambda^T eta Lambda and needs no matrix
 * inverse, and with which it transforms covectors and tensors with lower indices. In debug builds, i.e. if NDEBUG is not defined, the
 * constructors check that the matrix preserves the metric (see isValid()), and throw std::invalid_argument if it does not, like those of
 * the rotations (see ActiveSpatialRotation). Release builds do not check. makeUnchecked() skips the check, e.g. in hot paths.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_,
          int DimensionV_>
  requires AnyValueMatrix<ValuesT_>
class LorentzTransformation
    : public PassiveTransformation<LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>, FromT_, ToT_> {
 public:
  //! The passive linear transformation with the matrix of the Lorentz transformation, whose transformations it shares.
  using LinearTransformationT = PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_,
                                                            typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  using ValueT = ValueT_;
  using ValuesT = typename LinearTransformationT::ValuesT;
  using UnitsT = typename LinearTransformationT::UnitsT;
  using QuantitiesT = typename LinearTransformationT::QuantitiesT;

  //! The identity: From and To are the same inertial frame.
  LorentzTransformation() : quantities_{internal::identity<ValuesT>()} {}

  //! The Lorentz transformation whose matrix is constructed from the arguments, e.g. its elements row by row, or its quantities.
  //! Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  //! @throws std::invalid_argument In debug builds, if the matrix does not preserve the Minkowski metric (see isValid()).
  template <typename... Args>
    requires(sizeof...(Args) > 0 && !CopiesOrMoves<LorentzTransformation, Args...>)
  explicit LorentzTransformation(Args&&... args) : quantities_{std::forward<Args>(args)...} {
    internal::checkValidity(*this, "The matrix of a Lorentz transformation must preserve the Minkowski metric: Lambda^T eta Lambda = eta.");
  }

  //! The Lorentz transformation whose matrix is constructed from the arguments, without checking it: the caller guarantees that
  //! it preserves the Minkowski metric, e.g. for a product of Lorentz transformations.
  template <typename... Args>
  static LorentzTransformation makeUnchecked(Args&&... args) {
    return LorentzTransformation{Unchecked{}, QuantitiesT{std::forward<Args>(args)...}};
  }

  //! The matrix Lambda, which transforms the components v of a spacetime vector in From into its components Lambda v in To.
  const QuantitiesT& getQuantities() const { return quantities_; }

  //! The inverse Lorentz transformation, from To to From, whose matrix is eta Lambda^T eta = Lambda^-1: it needs no matrix inverse.
  auto inverse() const {
    const QuantitiesT metric{internal::minkowskiMetric<ValuesT>()};
    return LorentzTransformation<ValuesT_, ValueT_, ToT_, FromT_, DimensionV_>::makeUnchecked(
        innerProduct(metric, innerProduct(quantities_.transpose(), metric)));
  }

  //! Whether the matrix preserves the Minkowski metric eta: max_ij |(Lambda^T eta Lambda - eta)_ij| <= tolerance. The
  //! constructors check it in debug builds, with the tolerance ValidityTolerance. Like the definition of Lorentz transformations
  //! (Carroll, eq. 1.14), it also accepts the improper and the non-orthochronous ones, e.g. the reflection of time, whose
  //! determinant is -1.
  bool isValid(const ValueT tolerance = ValidityTolerance<ValueT>::value()) const {
    const auto metric = internal::minkowskiMetric<ValuesT>();
    const auto& values = quantities_.getValues();
    return internal::maxAbsDifference(ValuesT{values.transpose().innerProduct(metric.innerProduct(values))}, metric) <= tolerance;
  }

  //! Whether both Lorentz transformations, which must transform between the same inertial frames, are approximately equal: their
  //! matrices are compared like the components of tensors (see Tensor::isApprox()).
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename FromOtherT_, typename ToOtherT_,
            int DimensionOtherV_>
  bool isApprox(const LorentzTransformation<ValuesOtherT_, ValueOtherT_, FromOtherT_, ToOtherT_, DimensionOtherV_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameFrames<LorentzTransformation, std::remove_cvref_t<decltype(other)>>::value);
    return quantities_.isApprox(other.getQuantities(), precision);
  }

 private:
  struct Unchecked {};
  LorentzTransformation(Unchecked /*tag*/, QuantitiesT quantities) : quantities_{std::move(quantities)} {}

  //! The matrix Lambda, which transforms the components v of a spacetime vector in From into its components Lambda v in To.
  QuantitiesT quantities_;
};

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_>
struct IsChangeOfBasis<LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>> : std::true_type {};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using LorentzTransformation2 = LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, 2>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using LorentzTransformation3 = LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, 3>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using LorentzTransformation4 = LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, 4>;

// A Lorentz transformation transforms tensors like the passive linear transformation with its matrix, but with eta Lambda^T eta
// as the inverse, e.g. for covectors.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_,
          typename Object>
struct TransformationTraits<LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>, Object>
    : TransformationTraits<typename LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>::LinearTransformationT, Object> {};

// The composition X_AB * X_BC = X_AC of two Lorentz transformations is the Lorentz transformation Lambda_AB Lambda_BC.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
struct CompositionTraits<LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>,
                         LorentzTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>> {
  static auto compose(const LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>& lhs,
                      const LorentzTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
    return LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromRhsT_, ToLhsT_, DimensionLhsV_>::makeUnchecked(
        innerProduct(lhs.getQuantities(), rhs.getQuantities()));
  }
};

// A Lorentz transformation is a linear transformation: its compositions with linear transformations are those of the linear
// transformation with its matrix, the products of the matrices.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_,
          typename... UnitsRhsT_>
struct CompositionTraits<LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>,
                         PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_, UnitsRhsT_...>>
    : CompositionTraits<typename LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>::LinearTransformationT,
                        PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_, UnitsRhsT_...>> {};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_,
          typename ToRhsT_, int DimensionRhsV_>
struct CompositionTraits<PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_, UnitsLhsT_...>,
                         LorentzTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>>
    : CompositionTraits<
          PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_, UnitsLhsT_...>,
          typename LorentzTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>::LinearTransformationT> {};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
bool operator==(const LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>& lhs,
                const LorentzTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
  static_assert(internal::SameFrames<std::remove_cvref_t<decltype(lhs)>, std::remove_cvref_t<decltype(rhs)>>::value);
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
bool operator!=(const LorentzTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>& lhs,
                const LorentzTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_>
std::ostream& operator<<(std::ostream& lhs, const LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

//! The speed of light, as a quantity of the value matrix backend ValuesT_, e.g. getSpeedOfLight<wuzp_eigen::ValueMatrix, double>().
template <template <typename, int, int> typename ValuesT_, typename ValueT_>
auto getSpeedOfLight() {
  return Quantity<ValuesT_<ValueT_, 1, 1>, typename AccessUnits<AccessImplementation<ValueT_>>::Velocity>{static_cast<ValueT_>(299792458)};
}

//! The Minkowski invariant v_0^2 - v_1^2 - ... - v_n^2 of a vector whose first component is the time component, e.g. of a
//! spacetime vector (c t, x, y, z). All components need the same unit.
//! It takes any vector rather than SpacetimeVector, whose alias template parameters could not be deduced.
template <typename QuantitiesT_, typename CoordinateSystemT_>
auto getInvariant(const Vector<QuantitiesT_, CoordinateSystemT_>& vector) {
  static_assert(internal::RejectBatch<Indices<Up>, QuantitiesT_>::value);
  constexpr int dimension = Rows<QuantitiesT_>::value;
  const auto& quantities = vector.getQuantities();
  return pow<2>(quantities.template get<0, 0>()) - quantities.template block<1, 0, dimension - 1, 1>().squaredNorm();
}

/**
 * The passive Lorentz boost from From to To, in the standard form with the time component first and in length units
 * (x^0 = c t), and the signature (+, -, ..., -): x'^0 = gamma (x^0 - beta . x), with beta = v / c and the Lorentz factor
 * gamma = 1 / sqrt(1 - beta^2) (see the boost matrix B(v) in https://en.wikipedia.org/wiki/Lorentz_transformation#Proper_transformations).
 * The composition of two boosts along the same direction is the boost with the relativistic sum of their velocities, e.g. 0.5 c
 * and 0.5 c give 0.8 c; the composition of boosts along different directions also rotates (Wigner rotation).
 * @param velocity The velocity of To relative to From, i.e. of the origin of To measured in From, expressed in From. Its
 * magnitude must be smaller than the speed of light.
 * @throws std::invalid_argument If the magnitude of the velocity is not smaller than the speed of light, for which there is no
 * Lorentz boost (the Lorentz factor is infinite or not real).
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_>
LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_> createBoostMatrix(
    const VelocityVector<DimensionV_ - 1, ValuesT_, ValueT_, FromT_>& velocity) {
  using QuantitiesT = typename LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>::QuantitiesT;
  using QuantityT = Quantity<ValuesT_<ValueT_, 1, 1>, typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  const QuantityT squaredBeta{velocity.squaredNorm() / pow<2>(getSpeedOfLight<ValuesT_, ValueT_>())};
  // Also rejects NaN.
  if (!(squaredBeta.getValue() < 1)) {
    throw std::invalid_argument("The magnitude of the velocity of a Lorentz boost must be smaller than the speed of light.");
  }
  const auto lorentzFactor{pow<-1, 2>(QuantityT{1.0} - squaredBeta)};
  // The spatial part uses (lorentzFactor - 1) / |v|^2 = lorentzFactor^2 / ((lorentzFactor + 1) c^2). The right-hand side is
  // finite for v = 0, where the left-hand side divides zero by zero, and numerically more stable for small velocities.
  const auto spatialFactor{pow<2>(lorentzFactor) / ((lorentzFactor + QuantityT{1.0}) * pow<2>(getSpeedOfLight<ValuesT_, ValueT_>()))};
  QuantitiesT quantities;
  // Set the lower left matrix inclusive the diagonal.
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = 0; col < row + 1; ++col) {
      QuantityT quantity;
      // For row 0, the column is 0 too.
      if (row == 0) {
        quantity = lorentzFactor;
      } else if (col == 0) {
        quantity = -lorentzFactor / getSpeedOfLight<ValuesT_, ValueT_>() * velocity.getQuantities().get(row - 1, 0);
      } else if (col == row) {
        quantity = QuantityT{1.0} + spatialFactor * pow<2>(velocity.getQuantities().get(row - 1, 0));
      } else {
        quantity = spatialFactor * velocity.getQuantities().get(col - 1, 0) * velocity.getQuantities().get(row - 1, 0);
      }
      quantities.set(row, col, quantity);
    }
  }
  // Upper right matrix is mirrored.
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = row + 1; col < DimensionV_; ++col) {
      quantities.set(row, col, quantities.get(col, row));
    }
  }
  // The boost preserves the metric by construction: it is not checked, which would fail for Lorentz factors so large that the
  // rounding errors of gamma^2 exceed the tolerance.
  return LorentzTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>::makeUnchecked(std::move(quantities));
}

}  // namespace wuzp
