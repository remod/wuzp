#pragma once

#include "ctm/Matrix.hpp"
#include "ctm/Translation.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/traits.hpp"
#include "ctm/boost.hpp"
#include "ctm/eigen.hpp"
#include "ctm/Position.hpp"

namespace ctm {

template <typename Scalar, typename Frame>
using Position = PositionTemplate<Eigen::Matrix<Scalar, 3, 1>, Frame, boost::units::absolute<boost::units::si::length>>;

template <typename Scalar, typename Frame>
using Translation = TranslationTemplate<Eigen::Matrix<Scalar, 3, 1>, Frame, boost::units::si::length>;

template <typename From, typename To, typename Scalar>
using ProperRigidTransformation = ProperRigidTransformationTemplate<From, To, Eigen::Quaternion<Scalar>, Translation<Scalar, To>>;

template <typename From, typename To, typename Scalar>
struct TransformationTraits<ProperRigidTransformation<From, To, Scalar>, Position<Scalar, From>> {
  static auto transform(const ProperRigidTransformation<From, To, Scalar>& transformation,
                        const Position<Scalar, From>& object) {
    return Matrix<Eigen::Matrix<Scalar, 3, 1>, TypeMatrix<3, 1, To>, TypeMatrix<3, 1, boost::units::si::length>>(transformation.getRotation() * object.getValues()) +
                                                                            transformation.getTranslation();
  }
};

}  // namespace ctm
