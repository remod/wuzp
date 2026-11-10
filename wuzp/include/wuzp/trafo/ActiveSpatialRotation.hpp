#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/internal/rigid.hpp"
#include "wuzp/trafo/internal/rotation.hpp"
#include "wuzp/trafo/internal/validity.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * An active rotation in its coordinate system: the dimensionless linear transformation x -> R x whose matrix R is orthogonal
 * with determinant 1, R R^T = I and det(R) = 1. It transforms the same tensors as the active linear transformation with its
 * matrix (see ActiveLinearTransformation). It composes with rotations into rotations, with linear transformations into linear
 * transformations, and with translations and proper rigid transformations into proper rigid transformations (see
 * CompositionTraits). Its inverse is its transpose, which every value matrix backend provides, also for integer matrices, and
 * with which it transforms covectors and tensors with lower indices.
 * In debug builds, i.e. if NDEBUG is not defined, the constructors check that the matrix is a rotation (see isValid()), like
 * Drake's RotationMatrix (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html): they throw
 * std::invalid_argument if it is not, e.g. for a shear, a scaling or a reflection. Release builds do not check. makeUnchecked()
 * skips the check, e.g. in hot paths, and projectToRotation() computes the rotation nearest to a matrix, e.g. to one whose
 * rounding errors accumulated.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_, int DimensionV_>
  requires AnyValueMatrix<ValuesT_>
class ActiveSpatialRotation : public ActiveTransformation<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>> {
 public:
  //! The active linear transformation with the matrix of the rotation, whose transformations the rotation shares.
  using LinearTransformationT = ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_, DimensionV_,
                                                           typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  using ValueT = ValueT_;
  using ValuesT = typename LinearTransformationT::ValuesT;
  using UnitsT = typename LinearTransformationT::UnitsT;
  using QuantitiesT = typename LinearTransformationT::QuantitiesT;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = typename LinearTransformationT::TensorT;

  //! The identity.
  ActiveSpatialRotation() : quantities_{internal::identity<ValuesT>()} {}

  //! The rotation whose matrix is constructed from the arguments, e.g. its elements row by row, or its quantities. Copies and moves
  //! are left to the implicit constructors, see CopiesOrMoves.
  //! @throws std::invalid_argument In debug builds, if the matrix is not a rotation (see isValid()).
  template <typename... Args>
    requires(sizeof...(Args) > 0 && !CopiesOrMoves<ActiveSpatialRotation, Args...>)
  explicit ActiveSpatialRotation(Args&&... args) : quantities_{std::forward<Args>(args)...} {
    internal::checkValidity(*this, "The matrix of a rotation must be orthogonal with determinant 1.");
  }

  //! The rotation whose matrix is constructed from the arguments, without checking it, like Drake's
  //! RotationMatrix::MakeUnchecked(): the caller guarantees that the matrix is a rotation, e.g. a product of rotations.
  template <typename... Args>
  static ActiveSpatialRotation makeUnchecked(Args&&... args) {
    return ActiveSpatialRotation{Unchecked{}, QuantitiesT{std::forward<Args>(args)...}};
  }

  //! The rotation nearest to the matrix constructed from the arguments, e.g. to a rotation whose rounding errors accumulated, by
  //! the Newton-Schulz iteration (see internal::projectToRotation()).
  //! @throws std::invalid_argument In every build, if the matrix A is not close enough to a rotation, i.e. ||I - A^T A|| is not
  //! smaller than 1 in the Frobenius norm, or if its determinant is not positive.
  template <typename... Args>
  static ActiveSpatialRotation projectToRotation(Args&&... args) {
    return makeUnchecked(internal::projectToRotation(QuantitiesT{std::forward<Args>(args)...}.getValues()));
  }

  //! The matrix R, which transforms a vector v into R v.
  const QuantitiesT& getQuantities() const { return quantities_; }
  //! The matrix as a linear map, a tensor with an upper and a lower index.
  TensorT getTensor() const { return TensorT{quantities_}; }

  //! The inverse rotation, whose matrix is the transpose R^T = R^-1: it needs no matrix inverse.
  ActiveSpatialRotation inverse() const { return makeUnchecked(quantities_.transpose()); }

  //! Whether the matrix is a rotation: max_ij |(R R^T - I)_ij| <= tolerance, and det(R) > 0 (see internal::isRotation()). The
  //! constructors check it in debug builds, with the tolerance ValidityTolerance.
  bool isValid(const ValueT tolerance = ValidityTolerance<ValueT>::value()) const {
    return internal::isRotation(quantities_.getValues(), tolerance);
  }

  //! Whether both rotations, which must be expressed in the same coordinate system, are approximately equal: their matrices are
  //! compared like the components of tensors (see Tensor::isApprox()).
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename CoordinateSystemOtherT_,
            int DimensionOtherV_>
  bool isApprox(const ActiveSpatialRotation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionOtherV_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    return quantities_.isApprox(other.getQuantities(), precision);
  }

 private:
  struct Unchecked {};
  ActiveSpatialRotation(Unchecked /*tag*/, QuantitiesT quantities) : quantities_{std::move(quantities)} {}

  //! The matrix R, which transforms a vector v into R v.
  QuantitiesT quantities_;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialRotation2 = ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, 2>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialRotation3 = ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, 3>;

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
struct IsRotation<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>> : std::true_type {};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
struct GetCoordinateSystem<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>> {
  using type = CoordinateSystemT_;
};

// A rotation transforms tensors, and conjugates translations and linear transformations, like the active linear transformation
// with its matrix, but with its transpose as the inverse, e.g. for covectors.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_, typename Object>
struct TransformationTraits<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>, Object>
    : TransformationTraits<typename ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>::LinearTransformationT,
                           Object> {};

// The conjugation R Q R^T of a rotation Q by a rotation R is a rotation.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_,
          template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename CoordinateSystemOtherT_,
          int DimensionOtherV_>
struct TransformationTraits<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>,
                            ActiveSpatialRotation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionOtherV_>> {
  using ObjectT = ActiveSpatialRotation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionOtherV_>;
  static ObjectT transform(const ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>& transformation,
                           const ObjectT& object) {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    const auto& rotation = transformation.getQuantities();
    return ObjectT::makeUnchecked(innerProduct(rotation, innerProduct(object.getQuantities(), rotation.transpose())));
  }
};

// The composition of two rotations is the rotation R_lhs R_rhs.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
struct CompositionTraits<ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
                         ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>> {
  using ResultT = ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>;
  static ResultT compose(const ResultT& lhs,
                         const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
    return ResultT::makeUnchecked(innerProduct(lhs.getQuantities(), rhs.getQuantities()));
  }
};

// A rotation is a linear transformation: its compositions with linear transformations are those of the linear transformation with
// its matrix, the products of the matrices.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionFromRhsV_,
          int DimensionToRhsV_, typename... UnitsRhsT_>
struct CompositionTraits<
    ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
    ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>>
    : CompositionTraits<
          typename ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>::LinearTransformationT,
          ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>> {
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionRhsV_>
struct CompositionTraits<
    ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
    ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>>
    : CompositionTraits<
          ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
          typename ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>::LinearTransformationT> {};

// The compositions of a rotation with a translation or a proper rigid transformation are proper rigid transformations: of the
// rotation and the translation, or of the type of the proper rigid operand (see internal/rigid.hpp).

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_, typename CoordinateSystemRhsT_,
          typename... UnitsRhsT_>
struct CompositionTraits<ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
                         ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>> {
  using LhsT = ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>;
  using RhsT = ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>;
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemLhsT_, LhsT, RhsT>;
  static ResultT compose(const LhsT& lhs, const RhsT& rhs) { return internal::composeRotationAndTranslation<ResultT>(lhs, rhs); }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_,
          int DimensionRhsV_>
struct CompositionTraits<ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>,
                         ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>> {
  using LhsT = ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>;
  using RhsT = ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>;
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemRhsT_, RhsT, LhsT>;
  static ResultT compose(const LhsT& lhs, const RhsT& rhs) { return internal::composeTranslationAndRotation<ResultT>(lhs, rhs); }
};

template <typename CoordinateSystemT_, typename RotationT_, typename TranslationT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
struct CompositionTraits<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>,
                         ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>;
  static ResultT compose(const ResultT& lhs,
                         const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
    return internal::composeRigidAndRotation<ResultT>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          typename CoordinateSystemT_, typename RotationT_, typename TranslationT_>
struct CompositionTraits<ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
                         ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>;
  static ResultT compose(const ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>& lhs,
                         const ResultT& rhs) {
    return internal::composeRotationAndRigid<ResultT>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
bool operator==(const ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>& lhs,
                const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
bool operator!=(const ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>& lhs,
                const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
std::ostream& operator<<(std::ostream& lhs, const ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

}  // namespace wuzp
