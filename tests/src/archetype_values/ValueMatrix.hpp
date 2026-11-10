#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

// The value matrix archetype: a minimal value matrix backend, which provides exactly what the concepts ValueMatrixBackend and
// InvertibleValueMatrixBackend in wuzp/core/concepts.hpp require, and nothing more, like the archetypes of Boost's Concept Check
// library (https://www.boost.org/doc/libs/release/libs/concept_check/doc/reference.htm#basic-archetype). The presets
// <compiler>-archetype-archetype run the whole test suite against it, which checks that the core uses nothing beyond the
// interface of the value matrix backends. It is test code: it is neither installed nor part of the public headers.
//
// It is deliberately plain: internal::Matrix, a fixed-size array, plays the role of the linear algebra library, with naive loops
// and eager evaluation, i.e. no expression templates, so every trait returns a Matrix.

namespace archetype_values {
namespace internal {

/**
 * The matrices of the archetype: RowsV_ x ColsV_ values, stored row by row. A Matrix can only be default constructed, move
 * constructed, and accessed with at(): it has no copy constructor and no assignment operators, so that the core has to copy and
 * assign matrices with the traits Copy and Move, which give the values value semantics with every backend, e.g. also with
 * Blitz++, whose copy constructor shares the values (see "Results of operations are evaluated" in DESIGN.md). Its element access
 * differs from that of Eigen and Blitz++, matrix(row, col), so the core has to access elements with the trait Coefficient. at()
 * checks the indices.
 */
template <typename ValueT_, int RowsV_, int ColsV_>
class Matrix {
 public:
  Matrix() = default;
  Matrix(const Matrix&) = delete;
  Matrix(Matrix&&) noexcept = default;
  Matrix& operator=(const Matrix&) = delete;
  Matrix& operator=(Matrix&&) = delete;
  ~Matrix() = default;

  ValueT_& at(const int row, const int col) { return elements_.at(index(row, col)); }
  ValueT_ at(const int row, const int col) const { return elements_.at(index(row, col)); }

 private:
  static std::size_t index(const int row, const int col) {
    // Indices out of range would wrap around into another element: at() of the array only checks the flat index.
    if (row < 0 || row >= RowsV_ || col < 0 || col >= ColsV_) {
      throw std::out_of_range("Matrix index out of range.");
    }
    const int flatIndex = row * ColsV_ + col;
    return static_cast<std::size_t>(flatIndex);
  }

  std::array<ValueT_, static_cast<std::size_t>(RowsV_* ColsV_)> elements_{};
};

//! A matrix whose element (row, col) is function(row, col).
template <typename ValueT_, int RowsV_, int ColsV_, typename FunctionT_>
Matrix<ValueT_, RowsV_, ColsV_> generate(const FunctionT_& function) {
  Matrix<ValueT_, RowsV_, ColsV_> result;
  for (int row = 0; row < RowsV_; ++row) {
    for (int col = 0; col < ColsV_; ++col) {
      result.at(row, col) = static_cast<ValueT_>(function(row, col));
    }
  }
  return result;
}

//! The sum of function(row, col) over all elements.
template <typename ValueT_, int RowsV_, int ColsV_, typename FunctionT_>
ValueT_ sumOver(const FunctionT_& function) {
  ValueT_ result{0};
  for (int row = 0; row < RowsV_; ++row) {
    for (int col = 0; col < ColsV_; ++col) {
      result = static_cast<ValueT_>(result + function(row, col));
    }
  }
  return result;
}

//! The matrix without the given row and column.
template <typename ValueT_, int DimensionV_>
Matrix<ValueT_, DimensionV_ - 1, DimensionV_ - 1> minorMatrix(const Matrix<ValueT_, DimensionV_, DimensionV_>& matrix, const int row,
                                                              const int col) {
  return generate<ValueT_, DimensionV_ - 1, DimensionV_ - 1>([&](const int minorRow, const int minorCol) {
    return matrix.at(minorRow < row ? minorRow : minorRow + 1, minorCol < col ? minorCol : minorCol + 1);
  });
}

//! The determinant, by Laplace expansion along the first row: naive, but exact for integers.
template <typename ValueT_, int DimensionV_>
ValueT_ determinant(const Matrix<ValueT_, DimensionV_, DimensionV_>& matrix) {
  if constexpr (DimensionV_ == 1) {
    return matrix.at(0, 0);
  } else {
    return sumOver<ValueT_, 1, DimensionV_>([&](const int /*row*/, const int col) {
      const ValueT_ sign = col % 2 == 0 ? ValueT_{1} : ValueT_{-1};
      return sign * matrix.at(0, col) * determinant(minorMatrix(matrix, 0, col));
    });
  }
}

template <typename ValueT_, int RowsV_, int ColsV_>
std::ostream& operator<<(std::ostream& stream, const Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
  for (int row = 0; row < RowsV_; ++row) {
    for (int col = 0; col < ColsV_; ++col) {
      stream << (col == 0 ? "" : " ") << matrix.at(row, col);
    }
    stream << (row + 1 == RowsV_ ? "" : "\n");
  }
  return stream;
}

//! The trait argument of the matrices of the archetype, e.g. Sum<Access<ValueT_, RowsV_, ColsV_>, ...>.
template <typename ValueT_, int RowsV_, int ColsV_>
using Access = wuzp::AccessImplementation<Matrix<ValueT_, RowsV_, ColsV_>>;

}  // namespace internal

template <typename ValueT_, int RowsV_, int ColsV_>
using ValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<RowsV_, ColsV_>, ValueT_, internal::Matrix<ValueT_, RowsV_, ColsV_>>;

template <int RowsV_, int ColsV_>
using ValueMatrixI = ValueMatrix<int, RowsV_, ColsV_>;
template <int RowsV_, int ColsV_>
using ValueMatrixF = ValueMatrix<float, RowsV_, ColsV_>;
template <int RowsV_, int ColsV_>
using ValueMatrixD = ValueMatrix<double, RowsV_, ColsV_>;

template <typename ValueT_, int RowsV_>
using ValueVector = ValueMatrix<ValueT_, RowsV_, 1>;

template <int RowsV_>
using ValueVectorI = ValueVector<int, RowsV_>;
template <int RowsV_>
using ValueVectorF = ValueVector<float, RowsV_>;
template <int RowsV_>
using ValueVectorD = ValueVector<double, RowsV_>;

template <typename ValueT_>
using Value = ValueMatrix<ValueT_, 1, 1>;

using ValueI = Value<int>;
using ValueF = Value<float>;
using ValueD = Value<double>;

//! A quantity with a value of this backend, e.g. QuantityD<wuzp_units::Length>.
template <typename UnitT_>
using QuantityI = wuzp::Quantity<ValueI, UnitT_>;
template <typename UnitT_>
using QuantityF = wuzp::Quantity<ValueF, UnitT_>;
template <typename UnitT_>
using QuantityD = wuzp::Quantity<ValueD, UnitT_>;

}  // namespace archetype_values

namespace wuzp {

template <typename ValueT_, int RowsV_, int ColsV_>
struct ReserveMemory<archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>, MatrixSize<RowsV_, ColsV_>> {
  static void run(archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& /*matrix*/) {}
};

// Exactly RowsV_ * ColsV_ values of the value type, given row by row.
template <typename ValueT_, int RowsV_, int ColsV_, typename... ArgsT_>
  requires(sizeof...(ArgsT_) == RowsV_ * ColsV_ && (std::same_as<ArgsT_, ValueT_> && ...))
struct Initialize<archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>, std::tuple<ArgsT_...>> {
  static void run(archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix, ArgsT_... values) {
    const std::array<ValueT_, sizeof...(ArgsT_)> elements{values...};
    for (int index = 0; index < RowsV_ * ColsV_; ++index) {
      matrix.at(index / ColsV_, index % ColsV_) = elements.at(static_cast<std::size_t>(index));
    }
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Copy<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using MatrixT = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static MatrixT value(const MatrixT& matrix) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>([&](const int row, const int col) { return matrix.at(row, col); });
  }
};

// The move constructor moves, and assign() copies the elements: the matrices cannot be assigned.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Move<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using MatrixT = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static MatrixT construct(MatrixT& source) { return std::move(source); }
  static void assign(MatrixT& target, MatrixT& source) {
    for (int row = 0; row < RowsV_; ++row) {
      for (int col = 0; col < ColsV_; ++col) {
        target.at(row, col) = source.at(row, col);
      }
    }
  }
};

// Every result is evaluated already.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Evaluate<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& matrix) { return Copy<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>>::value(matrix); }
};

// Only other sizes of the same value type.
template <typename ValueT_, int RowsV_, int ColsV_, int RowsOtherV_, int ColsOtherV_>
struct Rebind<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, ValueT_, RowsOtherV_, ColsOtherV_> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsOtherV_, ColsOtherV_>;
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Coefficient<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using MatrixT = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static ValueT_ get(const MatrixT& matrix, const int row, const int col) { return matrix.at(row, col); }
  static void set(MatrixT& matrix, const int row, const int col, const ValueT_ value) { matrix.at(row, col) = value; }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Zero<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  static void run(archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
    for (int row = 0; row < RowsV_; ++row) {
      for (int col = 0; col < ColsV_; ++col) {
        matrix.at(row, col) = ValueT_{0};
      }
    }
  }
};

// Uniformly distributed in [-1, 1], like Eigen's setRandom(). Only for floating point values.
template <std::floating_point ValueT_, int RowsV_, int ColsV_>
struct Random<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  static void run(archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
    for (int row = 0; row < RowsV_; ++row) {
      for (int col = 0; col < ColsV_; ++col) {
        matrix.at(row, col) =
            static_cast<ValueT_>(-1) + static_cast<ValueT_>(2) * static_cast<ValueT_>(std::rand()) / static_cast<ValueT_>(RAND_MAX);
      }
    }
  }
};

// Only matrices of the same type.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Comparison<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>,
                  archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using MatrixT = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static bool equal(const MatrixT& lhs, const MatrixT& rhs) {
    return archetype_values::internal::sumOver<int, RowsV_, ColsV_>(
               [&](const int row, const int col) { return lhs.at(row, col) == rhs.at(row, col) ? 0 : 1; }) == 0;
  }
  // Relative, like Eigen's isApprox(): ||lhs - rhs||^2 <= tolerance^2 min(||lhs||^2, ||rhs||^2), with the Frobenius norm.
  static bool approxEqual(const MatrixT& lhs, const MatrixT& rhs, const ValueT_ tolerance) {
    const auto squaredNorm = [](const auto& function) { return archetype_values::internal::sumOver<ValueT_, RowsV_, ColsV_>(function); };
    const ValueT_ difference = squaredNorm(
        [&](const int row, const int col) { return (lhs.at(row, col) - rhs.at(row, col)) * (lhs.at(row, col) - rhs.at(row, col)); });
    const ValueT_ lhsNorm = squaredNorm([&](const int row, const int col) { return lhs.at(row, col) * lhs.at(row, col); });
    const ValueT_ rhsNorm = squaredNorm([&](const int row, const int col) { return rhs.at(row, col) * rhs.at(row, col); });
    return difference <= tolerance * tolerance * (lhsNorm < rhsNorm ? lhsNorm : rhsNorm);
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Sum<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& lhs, const type& rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return lhs.at(row, col) + rhs.at(row, col); });
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Difference<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>,
                  archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& lhs, const type& rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return lhs.at(row, col) - rhs.at(row, col); });
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Negation<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& matrix) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return -matrix.at(row, col); });
  }
};

// The elementwise product, and the product with a scalar of the value type.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Product<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& lhs, const type& rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return lhs.at(row, col) * rhs.at(row, col); });
  }
};
template <typename ValueT_, int RowsV_, int ColsV_>
struct Product<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, ValueT_> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& lhs, const ValueT_ rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return lhs.at(row, col) * rhs; });
  }
};

// The elementwise quotient, and the quotient by a scalar of the value type.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Division<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& lhs, const type& rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return lhs.at(row, col) / rhs.at(row, col); });
  }
};
template <typename ValueT_, int RowsV_, int ColsV_>
struct Division<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, ValueT_> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& lhs, const ValueT_ rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return lhs.at(row, col) / rhs; });
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct CwiseInverse<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const type& matrix) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return ValueT_{1} / matrix.at(row, col); });
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct Transpose<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, ColsV_, RowsV_>;
  static type value(const archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
    return archetype_values::internal::generate<ValueT_, ColsV_, RowsV_>([&](const int row, const int col) { return matrix.at(col, row); });
  }
};

template <typename ValueT_, int RowsV_, int ColsV_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  using type = archetype_values::internal::Matrix<ValueT_, BlockRowsV_, BlockColsV_>;
  static type value(const archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
    return archetype_values::internal::generate<ValueT_, BlockRowsV_, BlockColsV_>(
        [&](const int row, const int col) { return matrix.at(BlockRowIdV_ + row, BlockColIdV_ + col); });
  }
};

// The matrix product.
template <typename ValueT_, int RowsV_, int InnerV_, int ColsV_>
struct InnerProduct<archetype_values::internal::Access<ValueT_, RowsV_, InnerV_>,
                    archetype_values::internal::Access<ValueT_, InnerV_, ColsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>;
  static type value(const archetype_values::internal::Matrix<ValueT_, RowsV_, InnerV_>& lhs,
                    const archetype_values::internal::Matrix<ValueT_, InnerV_, ColsV_>& rhs) {
    return archetype_values::internal::generate<ValueT_, RowsV_, ColsV_>([&](const int row, const int col) {
      return archetype_values::internal::sumOver<ValueT_, 1, InnerV_>(
          [&](const int /*zero*/, const int inner) { return lhs.at(row, inner) * rhs.at(inner, col); });
    });
  }
};

// The outer product of matrices, at least one of which has a single row and at least one of which has a single column: its
// element (row, col) is the product of the elements of both, whose index is 0 along a dimension of size 1.
template <typename ValueT_, int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct OuterProduct<archetype_values::internal::Access<ValueT_, RowsLhsV_, ColsLhsV_>,
                    archetype_values::internal::Access<ValueT_, RowsRhsV_, ColsRhsV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, RowsLhsV_ + RowsRhsV_ - 1, ColsLhsV_ + ColsRhsV_ - 1>;
  static type value(const archetype_values::internal::Matrix<ValueT_, RowsLhsV_, ColsLhsV_>& lhs,
                    const archetype_values::internal::Matrix<ValueT_, RowsRhsV_, ColsRhsV_>& rhs) {
    return archetype_values::internal::generate<ValueT_, RowsLhsV_ + RowsRhsV_ - 1, ColsLhsV_ + ColsRhsV_ - 1>(
        [&](const int row, const int col) {
          return lhs.at(RowsLhsV_ == 1 ? 0 : row, ColsLhsV_ == 1 ? 0 : col) * rhs.at(RowsRhsV_ == 1 ? 0 : row, ColsRhsV_ == 1 ? 0 : col);
        });
  }
};

template <typename ValueT_>
struct CrossProduct<archetype_values::internal::Access<ValueT_, 3, 1>, archetype_values::internal::Access<ValueT_, 3, 1>> {
  using type = archetype_values::internal::Matrix<ValueT_, 3, 1>;
  static type value(const type& lhs, const type& rhs) {
    return archetype_values::internal::generate<ValueT_, 3, 1>([&](const int row, const int /*col*/) {
      const int next = (row + 1) % 3;
      const int afterNext = (row + 2) % 3;
      return lhs.at(next, 0) * rhs.at(afterNext, 0) - lhs.at(afterNext, 0) * rhs.at(next, 0);
    });
  }
};

template <typename ValueT_, int RowsV_, int ColsV_>
struct SquaredNorm<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  static ValueT_ value(const archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
    return archetype_values::internal::sumOver<ValueT_, RowsV_, ColsV_>(
        [&](const int row, const int col) { return matrix.at(row, col) * matrix.at(row, col); });
  }
};

// The norm keeps the value type, like Eigen's: the norm of an integer matrix is truncated.
template <typename ValueT_, int RowsV_, int ColsV_>
struct Norm<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>> {
  static ValueT_ value(const archetype_values::internal::Matrix<ValueT_, RowsV_, ColsV_>& matrix) {
    return static_cast<ValueT_>(std::sqrt(SquaredNorm<archetype_values::internal::Access<ValueT_, RowsV_, ColsV_>>::value(matrix)));
  }
};

// The determinant, also of integer matrices.
template <typename ValueT_, int DimensionV_>
struct Determinant<archetype_values::internal::Access<ValueT_, DimensionV_, DimensionV_>> {
  static ValueT_ value(const archetype_values::internal::Matrix<ValueT_, DimensionV_, DimensionV_>& matrix) {
    return archetype_values::internal::determinant(matrix);
  }
};

// The matrix inverse of a floating point matrix, the adjugate divided by the determinant (Cramer's rule): (A^-1)_ij = (-1)^(i+j)
// det(A without row j and column i) / det(A). It is not finite for a singular matrix.
template <std::floating_point ValueT_, int DimensionV_>
struct Inverse<archetype_values::internal::Access<ValueT_, DimensionV_, DimensionV_>> {
  using type = archetype_values::internal::Matrix<ValueT_, DimensionV_, DimensionV_>;
  static type value(const type& matrix) {
    const ValueT_ determinant = archetype_values::internal::determinant(matrix);
    return archetype_values::internal::generate<ValueT_, DimensionV_, DimensionV_>([&](const int row, const int col) {
      if constexpr (DimensionV_ == 1) {
        return ValueT_{1} / determinant;
      } else {
        const ValueT_ sign = (row + col) % 2 == 0 ? ValueT_{1} : ValueT_{-1};
        return sign * archetype_values::internal::determinant(archetype_values::internal::minorMatrix(matrix, col, row)) / determinant;
      }
    });
  }
};

}  // namespace wuzp
