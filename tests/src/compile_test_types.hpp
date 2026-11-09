#pragma once

// Types for compile tests (tests/compile_tests/cases).

#include "wuzp/core/Tensor.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

// Tensors with a single length quantity, so that operands only differ in the property a test is about: the tensor indices,
// the coordinate system, or whether they are absolute.
template <int ContravariantIndices, int CovariantIndices, typename AbsoluteT = wuzp::Relative,
          typename CoordinateSystemT = CoordinateSystemA>
using TestTensor = wuzp::Tensor<ContravariantIndices, CovariantIndices,
                                wuzp::Quantities<ValueMatrix<double, 1, 1>, wuzp::TypeMatrix<1, 1, Length>>, CoordinateSystemT, AbsoluteT>;

// The matrices of the linear algebra library of the value matrix backend, e.g. Eigen::Array, rather than its value matrices:
// not a value matrix backend.
template <typename ValueT_, int RowsV_, int ColsV_>
using NotAValueMatrixBackend = typename ValueMatrix<ValueT_, RowsV_, ColsV_>::ImplementationT;
