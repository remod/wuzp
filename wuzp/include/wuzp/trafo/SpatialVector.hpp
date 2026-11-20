#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {
namespace internal {

//! Checks that two spatial vectors are measured in the same frame, if both are motion vectors, i.e. have a MeasuredInT: a spatial
//! force has none. The value is always true, so that a mismatch fails with the message of the static_assert inside.
// The flag selects the specialization, rather than a requires clause on a specialization with the arguments of the primary
// template, which the other traits of wuzp never rely on.
template <typename LhsT_, typename RhsT_,
          bool BothMeasuredInV_ =
              requires {
                typename LhsT_::MeasuredInT;
                typename RhsT_::MeasuredInT;
              }>
struct SameMeasuredIn : std::true_type {};
template <typename LhsT_, typename RhsT_>
struct SameMeasuredIn<LhsT_, RhsT_, true> {
  static_assert(std::is_same_v<typename LhsT_::MeasuredInT, typename RhsT_::MeasuredInT>, "Measured-in frames do not match.");
  static constexpr bool value = true;
};

//! Checks that two spatial vectors have the same tags, except the point they are about: the frame they are measured in, if they
//! are motion vectors, the frame they describe and the coordinate system they are expressed in, e.g. a spatial acceleration and
//! the spatial velocity whose angular velocity shifts it. The value is always true, like that of SameSpatialTags.
template <typename LhsT_, typename RhsT_>
struct SameSpatialTagsExceptPoint {
  static_assert(SameMeasuredIn<LhsT_, RhsT_>::value);
  static_assert(std::is_same_v<typename LhsT_::FrameT, typename RhsT_::FrameT>, "Frames do not match.");
  static_assert(std::is_same_v<typename LhsT_::ExpressedInT, typename RhsT_::ExpressedInT>, "Coordinate systems do not match.");
  static constexpr bool value = true;
};

//! Checks that two spatial vectors have the same tags, e.g. to add or compare them: the frame they are measured in, if they are
//! motion vectors, the frame they describe, the point they are about and the coordinate system they are expressed in, the tags of
//! the monogram notation of Drake, e.g. V_MBp_E (https://drake.mit.edu/doxygen_cxx/group__multibody__notation__basics.html). The
//! value is always true, so that a mismatch fails with the message of the static_assert inside.
template <typename LhsT_, typename RhsT_>
struct SameSpatialTags {
  static_assert(SameSpatialTagsExceptPoint<LhsT_, RhsT_>::value);
  static_assert(std::is_same_v<typename LhsT_::PointT, typename RhsT_::PointT>, "Reference points do not match.");
  static constexpr bool value = true;
};

}  // namespace internal

/**
 * The base class of the spatial vectors SpatialVelocity, SpatialAcceleration and SpatialForce: a rotational and a translational
 * part, two three-dimensional vectors expressed in the same coordinate system, the rotational part first, like the spatial vectors
 * of Drake (https://drake.mit.edu/doxygen_cxx/group__multibody__spatial__vectors.html), Modern Robotics (Lynch and Park, Cambridge
 * University Press, 2017, sections 3.3.2 and 3.4) and Featherstone (Rigid Body Dynamics Algorithms, Springer, 2008). Each part is a
 * tensor with a unit of its own, e.g. an angular velocity and a velocity, so that the operations of tensors check them.
 * The spatial vector DerivedT_ that derives from it, in the curiously recurring template pattern (CRTP), defines the tags: FrameT,
 * PointT and ExpressedInT, and for motion vectors MeasuredInT. Spatial vectors are added, subtracted and compared, with
 * operator+, operator-, operator==, operator!= and isApprox(), only if all their tags match, and fail otherwise with
 * "Measured-in frames do not match.", "Frames do not match.", "Reference points do not match." or "Coordinate systems do not
 * match.". The spatial vectors combine spatial vectors with other tags with their shift() and compositions, e.g.
 * SpatialVelocity::composeWithMovingFrameVelocity(). A passive rotation or a passive proper rigid transformation re-expresses a
 * spatial vector about the same point, through the TransformationTraits of this header, and adjoint() maps spatial velocities and
 * spatial forces by a passive proper rigid transformation. AnySpatialVector tells whether a type is a spatial vector.
 * The destructor is protected and not virtual, like that of Transformation: spatial vectors are values, which are never deleted
 * through a pointer to this class. A declared destructor suppresses the implicit moves, so the copies and the moves are defaulted
 * too: a moved spatial vector moves its parts instead of copying them, which would allocate with e.g. Blitz++.
 */
template <typename DerivedT_, AnyTensor RotationalT_, AnyTensor TranslationalT_>
class SpatialVector {
 public:
  //! The rotational part, e.g. the angular velocity of a spatial velocity.
  using RotationalT = RotationalT_;
  //! The translational part, e.g. the velocity of the point of a spatial velocity.
  using TranslationalT = TranslationalT_;
  //! The value type of both parts, e.g. double.
  using ValueT = typename RotationalT_::ValueT;

  SpatialVector(const SpatialVector&) = default;
  SpatialVector(SpatialVector&&) = default;
  SpatialVector& operator=(const SpatialVector&) = default;
  SpatialVector& operator=(SpatialVector&&) = default;

  //! The rotational part, e.g. the angular velocity of a spatial velocity.
  const RotationalT& getRotational() const& { return rotational_; }
  //! The rotational part of a temporary is moved out of it.
  RotationalT getRotational() && { return std::move(rotational_); }
  //! The translational part, e.g. the velocity of the point of a spatial velocity.
  const TranslationalT& getTranslational() const& { return translational_; }
  //! The translational part of a temporary is moved out of it.
  TranslationalT getTranslational() && { return std::move(translational_); }

  /**
   * Whether both spatial vectors, which must have the same tags, are approximately equal: their rotational and their
   * translational parts are compared separately, like tensors (see Tensor::isApprox()), so that the units of one part do not
   * weigh against those of the other. Like the comparison of tensors, it is relative, so a part cannot be compared with zero: only
   * zero is approximately equal to zero.
   */
  template <typename OtherT_, typename RotationalOtherT_, typename TranslationalOtherT_>
  bool isApprox(const SpatialVector<OtherT_, RotationalOtherT_, TranslationalOtherT_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameSpatialTags<DerivedT_, OtherT_>::value);
    return rotational_.isApprox(other.getRotational(), precision) && translational_.isApprox(other.getTranslational(), precision);
  }

 protected:
  SpatialVector() = default;
  //! Direct-initializes each part, so that a part of the wrong kind, e.g. a velocity as an angular velocity, fails with the message
  //! of the conversion of tensors, e.g. "Units do not match.".
  template <typename RotationalOtherT_, typename TranslationalOtherT_>
  SpatialVector(RotationalOtherT_&& rotational, TranslationalOtherT_&& translational)
      : rotational_(std::forward<RotationalOtherT_>(rotational)), translational_(std::forward<TranslationalOtherT_>(translational)) {}
  ~SpatialVector() = default;

 private:
  RotationalT rotational_;
  TranslationalT translational_;
};

namespace internal {

//! Deduces the spatial vector DerivedT_ from a reference to its base class (see AnySpatialVector). Not defined: it is only used in
//! unevaluated expressions.
template <typename DerivedT_, typename RotationalT_, typename TranslationalT_>
void deduceSpatialVector(const SpatialVector<DerivedT_, RotationalT_, TranslationalT_>& spatialVector);

}  // namespace internal

//! A spatial vector of wuzp, i.e. a type that derives from SpatialVector: SpatialVelocity, SpatialAcceleration or SpatialForce.
template <typename T>
concept AnySpatialVector = requires(const T& spatialVector) { internal::deduceSpatialVector(spatialVector); };

namespace internal {

//! The spatial vector SpatialVectorT_ with the same tags, expressed in NewExpressedInT_, e.g. re-expressed by a passive rotation.
//! Specialized in the headers of the spatial vectors.
template <typename SpatialVectorT_, typename NewExpressedInT_>
struct WithExpressedIn;

//! The spatial vector whose parts are those of the spatial vector, re-expressed in NewExpressedInT_ by the passive transformation
//! from its coordinate system: like any vector, each part is multiplied by the matrix of a rotation, and left unchanged by a
//! translation, so that a passive proper rigid transformation only rotates them (see the TransformationTraits of the spatial
//! vectors below).
template <typename NewExpressedInT_, typename TransformationT_, typename SpatialVectorT_>
auto reexpress(const TransformationT_& transformation, const SpatialVectorT_& spatialVector) {
  return typename WithExpressedIn<SpatialVectorT_, NewExpressedInT_>::type{transformation.transform(spatialVector.getRotational()),
                                                                           transformation.transform(spatialVector.getTranslational())};
}

/**
 * The adjoint map of the passive proper rigid transformation X_AB = (R_AB, p_AoBo_A) from B to A, applied to a spatial velocity or
 * a spatial force about the origin of B, expressed in B: it is re-expressed in A by X_AB, i.e. by R_AB, then shifted to the origin
 * of A by p_BoAo_A = -p_AoBo_A (see adjoint()).
 */
template <typename FromT_, typename ToT_, typename RotationT_, typename TranslationT_, typename SpatialVectorT_>
auto adjointMap(const PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>& transformation,
                const SpatialVectorT_& spatialVector) {
  static_assert(std::is_same_v<typename SpatialVectorT_::PointT, FromT_>,
                "The adjoint map of the rigid transformation X_AB maps spatial vectors about the origin of B, expressed in B, to the "
                "origin of A, expressed in A: shift() them to the origin of B first.");
  static_assert(std::is_same_v<typename SpatialVectorT_::ExpressedInT, FromT_>, "Coordinate systems do not match.");
  return transformation.transform(spatialVector).template shift<ToT_>(-transformation.getTranslation().getTensor());
}

}  // namespace internal

//! Whether two spatial vectors with the same tags are equal: both their parts are equal.
template <typename LhsT_, typename RotationalLhsT_, typename TranslationalLhsT_, typename RhsT_, typename RotationalRhsT_,
          typename TranslationalRhsT_>
bool operator==(const SpatialVector<LhsT_, RotationalLhsT_, TranslationalLhsT_>& lhs,
                const SpatialVector<RhsT_, RotationalRhsT_, TranslationalRhsT_>& rhs) {
  static_assert(internal::SameSpatialTags<LhsT_, RhsT_>::value);
  return lhs.getRotational() == rhs.getRotational() && lhs.getTranslational() == rhs.getTranslational();
}

//! Whether two spatial vectors with the same tags differ: one of their parts differs.
template <typename LhsT_, typename RotationalLhsT_, typename TranslationalLhsT_, typename RhsT_, typename RotationalRhsT_,
          typename TranslationalRhsT_>
bool operator!=(const SpatialVector<LhsT_, RotationalLhsT_, TranslationalLhsT_>& lhs,
                const SpatialVector<RhsT_, RotationalRhsT_, TranslationalRhsT_>& rhs) {
  return !(lhs == rhs);
}

//! The sum of two spatial vectors with the same tags, part by part, e.g. of two spatial forces on the same body about the same
//! point. Spatial vectors with other tags are combined by shift() and the compositions, e.g. composeWithMovingFrameVelocity().
template <typename LhsT_, typename RotationalLhsT_, typename TranslationalLhsT_, typename RhsT_, typename RotationalRhsT_,
          typename TranslationalRhsT_>
LhsT_ operator+(const SpatialVector<LhsT_, RotationalLhsT_, TranslationalLhsT_>& lhs,
                const SpatialVector<RhsT_, RotationalRhsT_, TranslationalRhsT_>& rhs) {
  static_assert(internal::SameSpatialTags<LhsT_, RhsT_>::value);
  return LhsT_{lhs.getRotational() + rhs.getRotational(), lhs.getTranslational() + rhs.getTranslational()};
}

//! The difference of two spatial vectors with the same tags, part by part.
template <typename LhsT_, typename RotationalLhsT_, typename TranslationalLhsT_, typename RhsT_, typename RotationalRhsT_,
          typename TranslationalRhsT_>
LhsT_ operator-(const SpatialVector<LhsT_, RotationalLhsT_, TranslationalLhsT_>& lhs,
                const SpatialVector<RhsT_, RotationalRhsT_, TranslationalRhsT_>& rhs) {
  static_assert(internal::SameSpatialTags<LhsT_, RhsT_>::value);
  return LhsT_{lhs.getRotational() - rhs.getRotational(), lhs.getTranslational() - rhs.getTranslational()};
}

//! Prints the rotational part, then the translational part.
template <typename DerivedT_, typename RotationalT_, typename TranslationalT_>
std::ostream& operator<<(std::ostream& lhs, const SpatialVector<DerivedT_, RotationalT_, TranslationalT_>& rhs) {
  lhs << rhs.getRotational() << '\n' << rhs.getTranslational();
  return lhs;
}

// A passive rotation from E to F re-expresses a spatial vector in F: it rotates both parts, like vectors.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, typename SpatialVectorT_>
  requires AnySpatialVector<SpatialVectorT_>
struct TransformationTraits<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, 3>, SpatialVectorT_> {
  static auto transform(const PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, 3>& rotation, const SpatialVectorT_& spatialVector) {
    return internal::reexpress<ToT_>(rotation, spatialVector);
  }
};

// A passive translation leaves a spatial vector unchanged, like a vector: its point is a tag, not the origin of a coordinate
// system. It only changes the coordinate system, to To, so that a passive proper rigid transformation re-expresses it by its
// rotation alone.
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename FromT_, typename ToT_,
          typename... UnitsT_, typename SpatialVectorT_>
  requires AnySpatialVector<SpatialVectorT_>
struct TransformationTraits<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>, SpatialVectorT_> {
  static auto transform(const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>& translation,
                        const SpatialVectorT_& spatialVector) {
    return internal::reexpress<ToT_>(translation, spatialVector);
  }
};

}  // namespace wuzp
