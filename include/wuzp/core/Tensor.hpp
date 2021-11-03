#pragma once

#include <ostream>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/CoordinateSystemMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrixTraits.hpp"

namespace wuzp {

// Tensor (3+) - Hypermatrix
// Tensor (2)  - Matrix
// Tensor (1)  - Vector
// Tensor (0)  - Scalar

// template <int ContravariantIndices, int CovariantIndices>
// class Tensor;
//
// using Scalar = Tensor<0, 0>;
// using EuclideanVector = Tensor<1, 0>;       // A * v
// using LinearTransformation = Tensor<1, 1>;  // M' = A^-1 * M * A
// using RotationMatrix = Tensor<1, 1>;        // M' = A^-1 * M * A
// using InertiaTensor = Tensor<0, 2>;         // M' = A^T * M * A

// n-m-Tensor:
// n = contravariant
// m = covariant

// 1-1-Tensor aka linear transformation in our convention:
// w = M * v
// w' = M' * v'
// w' = A * w = A * M * v = A * M * A^-1 * A * v = A * M * A^-1 * v'
// M' = A * M * A^-1

// 0-2-Tensor aka inertia tensor (aka mass matrix) in our convention:
// E = v^T * M * v
// E' = v^T' * M' * v'
// E' = E
// v^T * M * v = v^T * A^T * M' * A * v
// M = A^T * M' * A
// M' = (A^-1)^T * M * A^-1
// Our convention is confusing as we have two times A^-1, which intuitively corresponds to 2 contravariant indices.
// Probably this is due basis vectors being transformed actively while the measured quantities in these coordinate systems are transformed
// passively.

// 2-0-Tensor in our convention:
// Should be invariant?

// Einstein Summation

// A = A^i_j
// A^T = A_j^i

// https://en.wikipedia.org/wiki/Change_of_basis

// Note: Wiki uses the different convention "old = A * new", we use "new = A * old".

// e^i = A^i_j * e^j' = A * e'
// e^i' = (A^-1)^i_j * e^j = A^-1 * e
// e_i' = e_j * (A^-1)_j^i = e^T * (A^-1)^T

// https://en.wikipedia.org/wiki/Tensor

// Basis vector (row vector!!!)
// e_i' = e_j * A_i^j
// e' = e * A

// In our convention (column vector!!!), redefine f = e^T, f' = e^T', B = A^T:
// f^T' = f^T * B^T
// f' = B * f

// Contravariant vector (or simply "vector"):
// v^i, column vector
// v^i' = (A^-1)^i_j * v^j
// v' = A^-1 * v
// In our convention: v' = A * v

// Covariant:
// w_i, row vector
// w_i' = w_j * A_i^j
// w' = w * A
// In our convention: w' = w * A^-1

// Orthogonal matrix / transformation: A^-1 == A^T

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix_>
class Tensor {
 public:
  using CoordinateMatrixT = CoordinateMatrix_;
  using QuantityMatrixT = typename CoordinateMatrixT::QuantityMatrixT;
  using UnitMatrixT = typename QuantityMatrixT::UnitMatrixT;
  using ValueMatrixT = typename QuantityMatrixT::ValueMatrixT;
  using This = Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrixT>;

  // TODO(remod): This is disabled to support batch processing of vectors, e.g. for point clouds. Remove it at one point.
  //  static_assert(ContravariantIndices + CovariantIndices == Dimension<CoordinateMatrix>::value, "Dimension does not match.");

  Tensor() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromTensor>) Tensor(const T0& t0, const Ts&... ts)
      : Tensor(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromTensor>) Tensor(T0&& t0, Ts&&... ts)
      : Tensor(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  ~Tensor() = default;

  static This zero() { return This(CoordinateMatrixT::zero()); }

  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  void setCoordinateMatrix(const CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>& coordinateMatrix) {
    coordinateMatrix_ = coordinateMatrix;
  }
  const CoordinateMatrixT& getCoordinateMatrix() const { return coordinateMatrix_; }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  void setQuantityMatrix(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& quantityMatrix) {
    coordinateMatrix_.setQuantityMatrix(quantityMatrix);
  }
  const QuantityMatrixT& getQuantityMatrix() const { return coordinateMatrix_.getQuantityMatrix(); }

  template <typename ValueMatrixOther>
  void setValueMatrix(const ValueMatrixOther& valueMatrix) {
    coordinateMatrix_.setValueMatrix(valueMatrix);
  }
  const ValueMatrixT& getValueMatrix() const { return coordinateMatrix_.getValueMatrix(); }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename CoordinateMatrixOther>
  bool isApprox(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, CoordinateMatrixOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
    return coordinateMatrix_.isApprox(other.getCoordinateMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        coordinateMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(coordinateMatrix_.transpose()); }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename CoordinateMatrixOther>
  auto dot(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, CoordinateMatrixOther>& other) const {
    // TODO(remod): Check indices.
    return coordinateMatrix_.dot(other.getCoordinateMatrix());
  }

  auto inverse() const { return typename Inverse<This>::type(coordinateMatrix_.inverse()); }

 private:
  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename CoordinateMatrixOther>
  // Allow other coordinate matrix types to support lazy evaluation. NOLINTNEXTLINE(google-explicit-constructor)
  explicit Tensor(FromTensor /*tag*/, const Tensor<ContravariantIndicesOther, CovariantIndicesOther, CoordinateMatrixOther>& other)
      : coordinateMatrix_(other.getCoordinateMatrix()) {
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
  }
  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename CoordinateMatrixOther>
  // Allow other coordinate matrix types to support lazy evaluation. NOLINTNEXTLINE(google-explicit-constructor)
  explicit Tensor(FromTensor /*tag*/, Tensor<ContravariantIndicesOther, CovariantIndicesOther, CoordinateMatrixOther>&& other)
      : coordinateMatrix_(std::move(other.getCoordinateMatrix())) {
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
  }

  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  explicit Tensor(FromCoordinateMatrix /*tag*/, const CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>& coordinateMatrix)
      : coordinateMatrix_(coordinateMatrix) {}
  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  explicit Tensor(FromCoordinateMatrix /*tag*/, CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>&& coordinateMatrix)
      : coordinateMatrix_(std::move(coordinateMatrix)) {}

  template <typename... Ts>
  explicit Tensor(FromCoordinateMatrixArgs /*tag*/, const Ts&... ts) : coordinateMatrix_(ts...) {}
  template <typename... Ts>
  explicit Tensor(FromCoordinateMatrixArgs /*tag*/, Ts&&... ts) : coordinateMatrix_(std::forward<Ts>(ts)...) {}

  CoordinateMatrixT coordinateMatrix_;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix_, typename... Ts>
struct GetTag<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix_>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename Chose<
      sizeof...(Ts) == 1,
      typename Chose<IsSpecializationInts<first, Tensor>::value, FromTensor,
                     typename Chose<(IsSpecialization<Ts, CoordinateMatrix>::value && ...), FromCoordinateMatrix,
                                    FromCoordinateMatrixArgs>::type>::type,
      typename Chose<(IsSpecialization<Ts, CoordinateMatrix>::value && ...), FromCoordinateMatrix, FromCoordinateMatrixArgs>::type>::type;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct Dimension<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  static constexpr int value = Dimension<CoordinateMatrix>::value;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct Rows<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  static constexpr int value = Rows<CoordinateMatrix>::value;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct Cols<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  static constexpr int value = Cols<CoordinateMatrix>::value;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
struct Sum<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
           Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, typename Sum<CoordinateMatrixLhs, CoordinateMatrixRhs>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
struct Difference<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
                  Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, typename Difference<CoordinateMatrixLhs, CoordinateMatrixRhs>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
struct Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
               Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>> {
  // TODO(remod): Do we need indices checks?
  using type = Tensor<ContravariantIndicesLhs - CovariantIndicesRhs, CovariantIndicesLhs - ContravariantIndicesRhs,
                      typename Product<CoordinateMatrixLhs, CoordinateMatrixRhs>::type>;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct Transpose<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  using type = Tensor<CovariantIndices, ContravariantIndices, typename Transpose<CoordinateMatrix>::type>;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix, int BlockRowId, int BlockColId, int BlockRows,
          int BlockCols>
struct Block<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type =
      Tensor<ContravariantIndices, CovariantIndices, typename Block<CoordinateMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
struct DotProduct<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
                  Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = typename DotProduct<CoordinateMatrixLhs, CoordinateMatrixRhs>::type;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct GetCoordinateSystem<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  using type = typename GetCoordinateSystem<CoordinateMatrix>::type;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct IsAbsolute<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  static constexpr bool value = IsAbsolute<CoordinateMatrix>::value;
};
template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct MakeRelative<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename MakeRelative<CoordinateMatrix>::type>;
  static type make(const Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>& object) {
    return type(MakeRelative<CoordinateMatrix>::make(object.getCoordinateMatrix()));
  }
};
template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct MakeAbsolute<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename MakeAbsolute<CoordinateMatrix>::type>;
  static type make(const Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>& object) {
    return type(MakeAbsolute<CoordinateMatrix>::make(object.getCoordinateMatrix()));
  }
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix, typename NewCoordinateSystem>
struct CastCoordinateSystem<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>, NewCoordinateSystem> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename CastCoordinateSystem<CoordinateMatrix, NewCoordinateSystem>::type>;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
struct Evaluate<Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename Evaluate<CoordinateMatrix>::type>;
};

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
auto operator+(const Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>& tensor) {
  return tensor;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
auto operator+(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>& rhs) {
  return typename Sum<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
                      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>>::type(lhs.getCoordinateMatrix() +
                                                                                                       rhs.getCoordinateMatrix());
}

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
auto operator-(const Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>& tensor) {
  return Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>(-tensor.getCoordinateMatrix());
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
auto operator-(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>& rhs) {
  return typename Sum<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
                      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>>::type(lhs.getCoordinateMatrix() -
                                                                                                       rhs.getCoordinateMatrix());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>& rhs) {
  return typename Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>,
                          Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>>::type(lhs.getCoordinateMatrix() *
                                                                                                           rhs.getCoordinateMatrix());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
bool operator==(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>& lhs,
                const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>& rhs) {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  return lhs.getCoordinateMatrix() == rhs.getCoordinateMatrix();
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename CoordinateMatrixLhs, int ContravariantIndicesRhs,
          int CovariantIndicesRhs, typename CoordinateMatrixRhs>
bool operator!=(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, CoordinateMatrixLhs>& lhs,
                const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, CoordinateMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <int ContravariantIndices, int CovariantIndices, typename CoordinateMatrix>
std::ostream& operator<<(std::ostream& stream, const Tensor<ContravariantIndices, CovariantIndices, CoordinateMatrix>& tensor) {
  stream << tensor.getCoordinateMatrix();
  return stream;
}

template <typename CoordinateMatrix>
using Vector = Tensor<1, 0, CoordinateMatrix>;
template <int Dimension, typename Scalar, typename Unit, typename CoordinateSystem>
using HomogeneousVector = Vector<CoordinateMatrix<QuantityMatrix<ValueMatrix<Scalar, Dimension, 1>, UnitMatrix<Dimension, 1, Unit>>,
                                                  CoordinateSystemMatrix<Dimension, 1, CoordinateSystem>>>;

template <typename CoordinateMatrix>
using Covector = Tensor<0, 1, CoordinateMatrix>;
template <int Dimension, typename Scalar, typename Unit, typename CoordinateSystem>
using HomogeneousCovector = Covector<CoordinateMatrix<QuantityMatrix<ValueMatrix<Scalar, 1, Dimension>, UnitMatrix<1, Dimension, Unit>>,
                                                      CoordinateSystemMatrix<1, Dimension, CoordinateSystem>>>;

template <int Dimension, typename Scalar, typename CoordinateSystem>
using PositionPoint = HomogeneousVector<Dimension, Scalar, Absolute<Length>, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using PositionPoint2D = PositionPoint<2, Scalar, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using PositionPoint3D = PositionPoint<3, Scalar, CoordinateSystem>;

template <int Dimension, typename Scalar, typename CoordinateSystem>
using PositionVector = HomogeneousVector<Dimension, Scalar, Length, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using PositionVector2D = PositionVector<2, Scalar, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using PositionVector3D = PositionVector<3, Scalar, CoordinateSystem>;

template <int Dimension, typename Scalar, typename CoordinateSystem>
using VelocityVector = HomogeneousVector<Dimension, Scalar, Velocity, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using VelocityVector2D = VelocityVector<2, Scalar, CoordinateSystem>;
template <typename Scalar, typename CoordinateSystem>
using VelocityVector3D = VelocityVector<3, Scalar, CoordinateSystem>;

}  // namespace wuzp
