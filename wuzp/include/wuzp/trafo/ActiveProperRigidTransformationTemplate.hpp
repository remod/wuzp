#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/internal/rigid.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * An active proper rigid transformation, which first rotates, then translates: x -> R x + t. The rotation R is a rotation, e.g.
 * an ActiveSpatialRotation, which checks that it is orthogonal with determinant 1 (see ActiveSpatialRotation::isValid()): the
 * inverse, (R^T, -R^T t), relies on it. ActiveProperRigidTransformation is the transformation of a rotation and a spatial
 * translation. Its compositions with rotations are defined in ActiveSpatialRotation.hpp.
 * TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
 */
template <AnyCoordinateSystem CoordinateSystemT_, AnyRotation RotationT_, typename TranslationT_>
class ActiveProperRigidTransformationTemplate
    : public ActiveTransformation<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>> {
 public:
  using Rotation = RotationT_;
  using Translation = TranslationT_;
  using ValueT = typename Rotation::ValueT;

  ActiveProperRigidTransformationTemplate() = default;
  ActiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_{std::move(rotation)}, translation_{std::move(translation)} {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

  //! The inverse transformation, (R^T, -R^T t), which only needs the transpose of the rotation (see internal::invertRigid()).
  ActiveProperRigidTransformationTemplate inverse() const { return internal::invertRigid<ActiveProperRigidTransformationTemplate>(*this); }

  //! Whether both transformations, which must be expressed in the same coordinate system, are approximately equal: their rotations
  //! and their translations are compared with their isApprox().
  template <typename CoordinateSystemOtherT_, typename RotationOtherT_, typename TranslationOtherT_>
  bool isApprox(const ActiveProperRigidTransformationTemplate<CoordinateSystemOtherT_, RotationOtherT_, TranslationOtherT_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    return rotation_.isApprox(other.getRotation(), precision) && translation_.isApprox(other.getTranslation(), precision);
  }

 private:
  Rotation rotation_;
  Translation translation_;
};

template <typename CoordinateSystemT_, typename RotationT_, typename TranslationT_>
struct GetCoordinateSystem<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>> {
  using type = CoordinateSystemT_;
};

template <typename CoordinateSystemT_, typename RotationT_, typename TranslationT_, typename Object>
struct TransformationTraits<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>, Object> {
  static auto transform(const ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>& transformation,
                        const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

// The compositions of proper rigid transformations with each other and with translations are proper rigid transformations of the
// type of the proper rigid operand (see internal/rigid.hpp).

template <typename CoordinateSystemLhsT_, typename RotationLhsT_, typename TranslationLhsT_, typename CoordinateSystemRhsT_,
          typename RotationRhsT_, typename TranslationRhsT_>
struct CompositionTraits<ActiveProperRigidTransformationTemplate<CoordinateSystemLhsT_, RotationLhsT_, TranslationLhsT_>,
                         ActiveProperRigidTransformationTemplate<CoordinateSystemRhsT_, RotationRhsT_, TranslationRhsT_>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemLhsT_, RotationLhsT_, TranslationLhsT_>;
  static ResultT compose(const ResultT& lhs,
                         const ActiveProperRigidTransformationTemplate<CoordinateSystemRhsT_, RotationRhsT_, TranslationRhsT_>& rhs) {
    return internal::composeRigid<ResultT>(lhs, rhs);
  }
};

template <typename CoordinateSystemT_, typename RotationT_, typename TranslationT_, template <typename, int, int> typename ValuesRhsT_,
          int DimensionRhsV_, typename ValueRhsT_, typename CoordinateSystemRhsT_, typename... UnitsRhsT_>
struct CompositionTraits<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>,
                         ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>;
  static ResultT compose(const ResultT& lhs,
                         const ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>& rhs) {
    return internal::composeRigidAndTranslation<ResultT>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, typename CoordinateSystemT_, typename RotationT_, typename TranslationT_>
struct CompositionTraits<ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>,
                         ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>;
  static ResultT compose(const ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>& lhs,
                         const ResultT& rhs) {
    return internal::composeTranslationAndRigid<ResultT>(lhs, rhs);
  }
};

}  // namespace wuzp
