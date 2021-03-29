#pragma once

#include <Eigen/Core>

#include "ctm/core.hpp"

namespace ctm {

template <typename Derived>
class MatrixBase {
 public:
  template <typename DerivedRhs>
  Derived operator+(const DerivedRhs& rhs) {
    return add(this->derived(), rhs.derived());
  }

 private:
  Derived& derived() {
    return static_cast<Derived&>(*this);
  }

  const Derived& derived() const {
    return static_cast<const Derived&>(*this);
  }
};

template <int Rows, int Cols, typename Scalar, typename Frames, typename Units>
class Matrix;

template <int Rows, int Cols, typename Scalar, typename... Frames, typename... Units>
class Matrix<Rows, Cols, Scalar, CtMatrix<Rows, Cols, Frames...>, CtMatrix<Rows, Cols, Units...>> : MatrixBase<Matrix<Rows, Cols, Scalar, CtMatrix<Rows, Cols, Frames...>, CtMatrix<Rows, Cols, Units...>>> {
 public:
  explicit Matrix(const Eigen::Matrix<Scalar, Rows, Cols>& values) : values_(values) {  }

  void fromEigen(const Eigen::Matrix<Scalar, Rows, Cols>& values) {
    values_ = values;
  }

  const Eigen::Matrix<Scalar, Rows, Cols>& toEigen() const {
    return values_;
  }

 private:
  Eigen::Matrix<Scalar, Rows, Cols> values_;
};

template<int RowsLhs, int ColsLhs, typename ScalarLhs, typename FramesLhs, typename UnitsLhs, int RowsRhs, int ColsRhs, typename ScalarRhs, typename FramesRhs, typename UnitsRhs>
struct Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>> {
  static_assert(RowsLhs == RowsRhs, "Matrix size mismatch.");
  static_assert(ColsLhs == ColsRhs, "Matrix size mismatch.");
  static_assert(std::is_same<ScalarLhs, ScalarRhs>::value, "Matrix scalar mismatch.");
  using type = Matrix<RowsLhs, ColsLhs, ScalarLhs, typename Sum<FramesLhs, FramesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template<int RowsLhs, int ColsLhs, typename ScalarLhs, typename FramesLhs, typename UnitsLhs, int RowsRhs, int ColsRhs, typename ScalarRhs, typename FramesRhs, typename UnitsRhs>
typename Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>>::type add(const Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs> &lhs, const Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs> &rhs) {
  return Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>>::type(lhs.derived().toEigen() + rhs.derived().toEigen());
}

template<int RowsLhs, int ColsLhs, typename ScalarLhs, typename FramesLhs, typename UnitsLhs, int RowsRhs, int ColsRhs, typename ScalarRhs, typename FramesRhs, typename UnitsRhs>
typename Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>>::type
operator+(const Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs> &lhs, const Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs> &rhs) {
  return add(lhs, rhs);
}

}
