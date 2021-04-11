#pragma once

#include <ostream>

#include "ctm/TypeMatrix.hpp"

namespace ctm {

// template <typename Derived>
// class MatrixBase {
// public:
//  template <typename DerivedRhs>
//  Derived operator+(const DerivedRhs& rhs) {
//    return add(this->derived(), rhs.derived());
//  }
//
// private:
//  Derived& derived() {
//    return static_cast<Derived&>(*this);
//  }
//
//  const Derived& derived() const {
//    return static_cast<const Derived&>(*this);
//  }
//};

template <typename Values, typename Frames, typename Units>
class Matrix {
 public:
  explicit Matrix(const Values& values) : values_(values) {}

  void setValues(const Values& values) { values_ = values; }

  const Values& getValues() const { return values_; }

 private:
  Values values_;
};

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
struct Sum<Matrix<ValuesLhs, FramesLhs, UnitsLhs>, Matrix<ValuesRhs, FramesRhs, UnitsRhs>> {
  using type =
      Matrix<typename Sum<ValuesLhs, ValuesRhs>::type, typename Sum<FramesLhs, FramesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
};

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
typename Sum<Matrix<ValuesLhs, FramesLhs, UnitsLhs>, Matrix<ValuesRhs, FramesRhs, UnitsRhs>>::type add(
    const Matrix<ValuesLhs, FramesLhs, UnitsLhs>& lhs, const Matrix<ValuesRhs, FramesRhs, UnitsRhs>& rhs) {
  return Sum<Matrix<ValuesLhs, FramesLhs, UnitsLhs>, Matrix<ValuesRhs, FramesRhs, UnitsRhs>>::type(lhs.toValues() + rhs.toValues());
}

template <typename ValuesLhs, typename FramesLhs, typename UnitsLhs, typename ValuesRhs, typename FramesRhs, typename UnitsRhs>
typename Sum<Matrix<ValuesLhs, FramesLhs, UnitsLhs>, Matrix<ValuesRhs, FramesRhs, UnitsRhs>>::type operator+(
    const Matrix<ValuesLhs, FramesLhs, UnitsLhs>& lhs, const Matrix<ValuesRhs, FramesRhs, UnitsRhs>& rhs) {
  return add(lhs, rhs);
}

// template<int RowsLhs, int ColsLhs, typename ScalarLhs, typename FramesLhs, typename UnitsLhs, int RowsRhs, int ColsRhs, typename
// ScalarRhs, typename FramesRhs, typename UnitsRhs> struct Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs,
// ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>> {
//  static_assert(RowsLhs == RowsRhs, "Matrix size mismatch.");
//  static_assert(ColsLhs == ColsRhs, "Matrix size mismatch.");
//  static_assert(std::is_same<ScalarLhs, ScalarRhs>::value, "Matrix scalar mismatch.");
//  using type = Matrix<RowsLhs, ColsLhs, ScalarLhs, typename Sum<FramesLhs, FramesRhs>::type, typename Sum<UnitsLhs, UnitsRhs>::type>;
//};
//
// template<int RowsLhs, int ColsLhs, typename ScalarLhs, typename FramesLhs, typename UnitsLhs, int RowsRhs, int ColsRhs, typename
// ScalarRhs, typename FramesRhs, typename UnitsRhs> typename Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs,
// ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>>::type add(const Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs> &lhs, const
// Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs> &rhs) {
//  return Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs,
//  UnitsRhs>>::type(lhs.derived().toEigen() + rhs.derived().toEigen());
//}
//
// template<int RowsLhs, int ColsLhs, typename ScalarLhs, typename FramesLhs, typename UnitsLhs, int RowsRhs, int ColsRhs, typename
// ScalarRhs, typename FramesRhs, typename UnitsRhs> typename Sum<Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs>, Matrix<RowsRhs,
// ColsRhs, ScalarRhs, FramesRhs, UnitsRhs>>::type operator+(const Matrix<RowsLhs, ColsLhs, ScalarLhs, FramesLhs, UnitsLhs> &lhs, const
// Matrix<RowsRhs, ColsRhs, ScalarRhs, FramesRhs, UnitsRhs> &rhs) {
//  return add(lhs, rhs);
//}

template <typename Values, typename Frames, typename Units>
struct GetFrame<Matrix<Values, Frames, Units>> {
  static_assert(Frames::isHomogeneous, "Frames are heterogeneous.");
  using type = std::tuple_element<0, typename Frames::AsTuple>;
};

template <typename Values, typename Frames, typename Units>
std::ostream& operator<< (std::ostream& stream, const Matrix<Values, Frames, Units>& matrix) {
  stream << matrix.getValues();
  return stream;
}

}  // namespace ctm
