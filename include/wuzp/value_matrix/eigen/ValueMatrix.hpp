#pragma once

#include <Eigen/Core>

namespace wuzp {

template <typename Scalar, int Rows, int Cols>
using ValueMatrix = Eigen::Matrix<Scalar, Rows, Cols>;

}  // namespace wuzp
