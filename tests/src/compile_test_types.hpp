#pragma once

// Types for compile tests (tests/compile_tests/cases).

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Tensor.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using wuzp::Down;
using wuzp::Indices;
using wuzp::Up;

// Tensors with a single length quantity, so that operands only differ in the property a test is about: the tensor indices,
// the coordinate system, or whether they are absolute. A 1x1 matrix is the canonical shape of tensors of every rank in one
// dimension.
template <typename IndicesT, typename AbsoluteT = wuzp::Relative, typename CoordinateSystemT = CoordinateSystemA>
using TestTensor =
    wuzp::Tensor<IndicesT, wuzp::Quantities<ValueMatrix<double, 1, 1>, wuzp::TypeMatrix<1, 1, Length>>, CoordinateSystemT, AbsoluteT>;

// A batch of two vectors or points, one per column, e.g. a point cloud.
template <int RowsV, typename AbsoluteT = wuzp::Relative, typename CoordinateSystemT = CoordinateSystemA>
using TestBatch = wuzp::Tensor<Indices<Up>, wuzp::Quantities<ValueMatrix<double, RowsV, 2>, wuzp::TypeMatrix<RowsV, 2, Length>>,
                               CoordinateSystemT, AbsoluteT>;

// The matrices of the linear algebra library of the value matrix backend, e.g. Eigen::Array, rather than its value matrices:
// not a value matrix backend.
template <typename ValueT_, int RowsV_, int ColsV_>
using NotAValueMatrixBackend = typename ValueMatrix<ValueT_, RowsV_, ColsV_>::ImplementationT;
