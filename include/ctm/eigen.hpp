#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/Translation.hpp"
#include "ctm/traits.hpp"

namespace ctm {

template <typename Scalar, int Rows, int Cols>
struct Sum<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // TODO(remod): Enable lazy evaluation.
  using type = Eigen::Matrix<Scalar, Rows, Cols>;
};

}  // namespace ctm
