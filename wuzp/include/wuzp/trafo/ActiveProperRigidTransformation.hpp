#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/trafo/ActiveRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

// First rotate, then translate.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
template <typename CoordinateSystem, typename Rotation_, typename Translation_>
class ActiveProperRigidTransformationTemplate
    : public ActiveTransformation<ActiveProperRigidTransformationTemplate<CoordinateSystem, Rotation_, Translation_>> {
 public:
  using Rotation = Rotation_;
  using Translation = Translation_;

  ActiveProperRigidTransformationTemplate() = default;
  ActiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_(std::move(rotation)), translation_(std::move(translation)) {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

 private:
  Rotation rotation_;
  Translation translation_;
};

namespace internal {

template <typename From, typename To>
struct IntermediateName;
template <typename From, typename To>
using Intermediate = CoordinateSystem<IntermediateName<From, To>>;

}  // namespace internal

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename CoordinateSystem, typename Scalar>
using ActiveProperRigidTransformation =
    ActiveProperRigidTransformationTemplate<CoordinateSystem, ActiveSpatialRotation<ValueMatrix, Scalar, CoordinateSystem, Dimension>,
                                            ActiveSpatialTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem>>;
template <template <typename, int, int> typename ValueMatrix, typename CoordinateSystem, typename Scalar>
using ActiveProperRigidTransformation2 = ActiveProperRigidTransformation<ValueMatrix, 2, CoordinateSystem, Scalar>;
template <template <typename, int, int> typename ValueMatrix, typename CoordinateSystem, typename Scalar>
using ActiveProperRigidTransformation3 = ActiveProperRigidTransformation<ValueMatrix, 3, CoordinateSystem, Scalar>;

template <typename CoordinateSystem, typename Rotation, typename Translation, typename Object>
struct TransformationTraits<ActiveProperRigidTransformationTemplate<CoordinateSystem, Rotation, Translation>, Object> {
  static auto transform(const ActiveProperRigidTransformationTemplate<CoordinateSystem, Rotation, Translation>& transformation,
                        const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
