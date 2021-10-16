#pragma once

#include "wuzp/trafo/LinearTransformation.hpp"

namespace wuzp {

template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units>
using Jacobian = LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;

// template <int Rows, int Cols, typename Scalar, typename From, typename To, typename... Units>
// class Jacobian : public PassiveTransformation<Jacobian<Rows, Cols, Scalar, From, To, Units...>, From, To> {
// public:
//  using ValueMatrixImpl = ValueMatrix<Scalar, Rows, Cols>;
//  using UnitMatrixImpl = UnitMatrix<Rows, Cols, Units...>;
//  using CoordinateSystemMatrixImpl = CoordinateSystemMatrix<Rows, Cols, From>;
//  using CoordinateMatrixImpl = CoordinateMatrix<ValueMatrixImpl, UnitMatrixImpl, CoordinateSystemMatrixImpl>;
//
//  explicit Jacobian(CoordinateMatrixImpl impl) : impl_(impl) {}
//
//  const CoordinateMatrixImpl& getCoordinateMatrix() const { return impl_; }
//
// private:
//  CoordinateMatrixImpl impl_;
//};
//
// template <int RowsJacobian, int ColsJacobian, int RowsObject, int ColsObject, typename Scalar, typename From, typename To,
//          typename... UnitsJacobian, typename... UnitsObject>
// struct TransformationTraits<Jacobian<RowsJacobian, ColsJacobian, Scalar, From, To, UnitsJacobian...>,
//                            CoordinateMatrix<ValueMatrix<Scalar, RowsObject, ColsObject>,
//                                             TypeMatrixImpl<RowsObject, ColsObject, UnitTraits, std::tuple<UnitsObject...>>,
//                                             CoordinateSystemMatrix<RowsObject, ColsObject, From>>> {
//  static auto transform(
//      const Jacobian<RowsJacobian, ColsJacobian, Scalar, From, To, UnitsJacobian...>& transformation,
//      const CoordinateMatrix<ValueMatrix<Scalar, RowsObject, ColsObject>, UnitMatrix<RowsObject, ColsObject, UnitsObject...>,
//                             CoordinateSystemMatrix<RowsObject, ColsObject, From>>& object) {
//    // TODO(remod): Add static assert to make sure we are transforming length/time etc.
//    return CoordinateMatrix<typename ValueMatrixTraits::Product<ValueMatrix<Scalar, RowsJacobian, ColsJacobian>,
//                                                                ValueMatrix<Scalar, RowsObject, ColsObject>>::type,
//                            typename Product<UnitMatrix<RowsJacobian, ColsJacobian, UnitsJacobian...>,
//                                             UnitMatrix<RowsObject, ColsObject, UnitsObject...>>::type,
//                            CoordinateSystemMatrix<RowsJacobian, ColsObject, To>>(
//        (transformation.getCoordinateMatrix() * object).getValueMatrix());
//  }
//};

}  // namespace wuzp
