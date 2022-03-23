#pragma once

#include "wuzp/core/ValuesWrapper.hpp"

namespace wuzp_value_matrix {

template <typename ValueT_>
class ScalarImpl {
 private:
  ValueT_ value_;
};

template <typename ValueT, int Rows, int Cols>
using Scalar = wuzp::ValuesWrapper<ScalarImpl<ValueT>>;

}  // namespace wuzp_value_matrix
