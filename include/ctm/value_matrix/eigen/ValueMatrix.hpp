#pragma once

#include <Eigen/Core>

namespace ctm {
namespace value_matrix {

template <typename Scalar, int Rows, int Cols>
using ValueMatrix = Eigen::Matrix<Scalar, Rows, Cols>;

}
}  // namespace ctm
