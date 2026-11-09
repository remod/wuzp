#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"
#include "wuzp/core/helpers.hpp"

// Concepts for the template parameters of the core. They check that a parameter has the right kind of type, e.g. that a
// coordinate system is a coordinate system. Invalid combinations of valid types, e.g. matrix sizes that do not match, are
// rejected with a static_assert and a clear message instead.

namespace wuzp {

//! Any coordinate system: a specialization of `CoordinateSystem`, including the intermediate ones of transformations.
template <typename T>
concept AnyCoordinateSystem = IsSpecialization<T, CoordinateSystem>::value;

//! `Absolute` or `Relative`: whether a tensor is a point or a vector.
template <typename T>
concept AbsoluteOrRelative = std::same_as<T, Absolute> || std::same_as<T, Relative>;

//! Any unit of any units backend: a specialization of `Unit`.
template <typename T>
concept AnyUnit = IsSpecialization<T, Unit>::value;

namespace internal {

template <typename T>
struct IsUnitMatrix : std::false_type {};
template <int RowsV_, int ColsV_, typename... UnitsT_>
struct IsUnitMatrix<TypeMatrixImpl<RowsV_, ColsV_, std::tuple<UnitsT_...>>> : std::bool_constant<(AnyUnit<UnitsT_> && ...)> {};

template <typename T>
struct IsMatrixSize : std::false_type {};
template <int RowsV_, int ColsV_>
struct IsMatrixSize<MatrixSize<RowsV_, ColsV_>> : std::true_type {};

}  // namespace internal

//! Any unit matrix: a `TypeMatrix` of units.
template <typename T>
concept AnyUnitMatrix = internal::IsUnitMatrix<T>::value;

//! Any size of a matrix: a `MatrixSize` with rows and columns.
template <typename T>
concept AnyMatrixSize = internal::IsMatrixSize<T>::value;

//! An arithmetic type, e.g. `int`, `float` or `double`: the type of the elements of value matrices.
template <typename T>
concept ArithmeticValue = std::is_arithmetic_v<T>;

// The following class templates are defined in their own headers, which use the concepts above. Their definitions repeat the
// constraints of these declarations.
template <AnyMatrixSize MatrixSizeT_, ArithmeticValue ValueT_, typename ImplementationT_>
class ValuesWrapper;

//! Any value matrix: a specialization of `ValuesWrapper`.
template <typename T>
concept AnyValuesWrapper = IsSpecialization<T, ValuesWrapper>::value;

template <AnyValuesWrapper ValuesT_, AnyUnitMatrix UnitsT_>
class Quantities;

//! Any quantity matrix: a specialization of `Quantities`, including `Quantity`.
template <typename T>
concept AnyQuantities = IsSpecialization<T, Quantities>::value;

template <int ContravariantIndicesV_, int CovariantIndicesV_, AnyQuantities QuantitiesT_, AnyCoordinateSystem CoordinateSystemT_,
          AbsoluteOrRelative AbsoluteT_>
class Tensor;

//! Any tensor: a specialization of `Tensor`, including points, vectors and their aliases.
template <typename T>
concept AnyTensor = IsSpecializationInts<T, Tensor>::value;

}  // namespace wuzp
