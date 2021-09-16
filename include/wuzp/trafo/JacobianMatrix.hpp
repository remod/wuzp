#pragma once

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <int Rows, int Cols, typename Scalar, typename From, typename To, typename... Units>
class JacobianMatrix : public PassiveTransformation<JacobianMatrix<Rows, Cols, Scalar, From, To, Units...>, From, To> {
 public:
  using ValueMatrixImpl = ValueMatrix<Scalar, Rows, Cols>;
  using UnitMatrixImpl = UnitMatrix<Rows, Cols, Units...>;
  using CoordinateSystemMatrixImpl = CoordinateSystemMatrix<Rows, Cols, From>;
  using CoordinateMatrixImpl = CoordinateMatrix<ValueMatrixImpl, UnitMatrixImpl, CoordinateSystemMatrixImpl>;

  explicit JacobianMatrix(CoordinateMatrixImpl impl) : impl_(impl) {}

  const CoordinateMatrixImpl& getCoordinateMatrix() const { return impl_; }

 private:
  CoordinateMatrixImpl impl_;
};

template <int RowsJacobianMatrix, int ColsJacobianMatrix, int RowsObject, int ColsObject, typename Scalar, typename From, typename To,
          typename... UnitsJacobianMatrix, typename... UnitsObject>
struct TransformationTraits<
    JacobianMatrix<RowsJacobianMatrix, ColsJacobianMatrix, Scalar, From, To, UnitsJacobianMatrix...>,
    CoordinateMatrix<ValueMatrix<Scalar, RowsObject, ColsObject>, TypeMatrixImpl<RowsObject, ColsObject, UnitTraits, std::tuple<UnitsObject...>>,
                     CoordinateSystemMatrix<RowsObject, ColsObject, From>>> {
  static auto transform(
      const JacobianMatrix<RowsJacobianMatrix, ColsJacobianMatrix, Scalar, From, To, UnitsJacobianMatrix...>& transformation,
      const CoordinateMatrix<ValueMatrix<Scalar, RowsObject, ColsObject>,
                             UnitMatrix<RowsObject, ColsObject, UnitsObject...>,
                             CoordinateSystemMatrix<RowsObject, ColsObject, From>>& object) {
    return CoordinateMatrix<typename ValueMatrixTraits::Product<ValueMatrix<Scalar, RowsJacobianMatrix, ColsJacobianMatrix>,
                                                                ValueMatrix<Scalar, RowsObject, ColsObject>>::type,
                            typename Product<UnitMatrix<RowsJacobianMatrix, ColsJacobianMatrix, UnitsJacobianMatrix...>,
                                             UnitMatrix<RowsObject, ColsObject, UnitsObject...>>::type,
                            CoordinateSystemMatrix<RowsJacobianMatrix, ColsObject, To>>(
        (transformation.getCoordinateMatrix() * object).getValueMatrix());
  }
};

}  // namespace wuzp
