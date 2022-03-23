#pragma once

#include <ostream>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

// Tensor (3+) - Hypermatrix
// Tensor (2)  - Matrix
// Tensor (1)  - Vector
// Tensor (0)  - Scalar

// template <int ContravariantIndices, int CovariantIndices>
// class Tensor;
//
// using ScalarT = Tensor<0, 0>;
// using EuclideanVector = Tensor<1, 0>;       // A * v
// using PassiveLinearTransformation = Tensor<1, 1>;  // M' = A^-1 * M * A
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

// TODO(remod): Are two indices sufficient or do we need a boolean per dimension?
template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
class Tensor {
 public:
  using QuantityMatrixT = QuantityMatrixT_;
  using CoordinateSystemT = CoordinateSystemT_;
  using UnitMatrixT = typename QuantityMatrixT::UnitMatrixT;
  using ValueMatrixT = typename QuantityMatrixT::ValueMatrixT;
  // TODO(remod): Use ValueT instead of ScalarT.
  using ScalarT = typename QuantityMatrixT::ScalarT;
  using This = Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT, CoordinateSystemT>;

  static_assert(CovariantIndices >= 0, "Covariant index must not be negative.");
  static_assert(ContravariantIndices >= 0, "Contravariant index must not be negative.");

  Tensor() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar tensors. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromTensor>) Tensor(const T0& t0, const Ts&... ts)
      : Tensor(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar tensors. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromTensor>) Tensor(T0&& t0, Ts&&... ts)
      : Tensor(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static This zero() { return This(QuantityMatrixT::zero()); }
  static This random() { return This(QuantityMatrixT::random()); }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  void setQuantityMatrix(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& component) {
    quantityMatrix_ = component;
  }
  const QuantityMatrixT& getQuantityMatrix() const { return quantityMatrix_; }

  template <typename ValueMatrixOther>
  void setValueMatrix(const ValueMatrixOther& valueMatrix) {
    quantityMatrix_.setValueMatrix(valueMatrix);
  }
  const ValueMatrixT& getValueMatrix() const { return quantityMatrix_.getValueMatrix(); }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantityMatrixOther, typename CoordinateSystemOther>
  bool isApprox(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther, CoordinateSystemOther>& other,
                const ScalarT precision = Precision<ScalarT>::value()) const {
    // Note: Allow other tensor types to support lazy evaluation.
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOther>, "Coordinate systems do not match.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix(), precision);
  }

  // TODO(remod): Remove 'get' from all getters?
  template <int RowId, int ColId>
  auto getQuantity() const {
    return quantityMatrix_.template getQuantity<RowId, ColId>();
  }

  auto getQuantity(const int rowId, const int colId) const { return quantityMatrix_.getQuantity(rowId, colId); }

  template <int RowId, int ColId, typename QuantityT_>
  void setQuantity(const QuantityT_& quantity) {
    return quantityMatrix_.template setQuantity<RowId, ColId>(quantity);
  }

  template <typename Quantity>
  void setQuantity(const int rowId, const int colId, const Quantity& quantity) {
    quantityMatrix_.setQuantity(rowId, colId, quantity);
  }

  // TODO(remod): Rename to "slice"?
  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(quantityMatrix_.transpose()); }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantityMatrixOther, typename CoordinateSystemOther>
  auto dotProduct(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther, CoordinateSystemOther>& other) const {
    return typename DotProduct<This, Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther,
                                            CoordinateSystemOther>>::type(quantityMatrix_.dotProduct(other.getQuantityMatrix()));
  }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantityMatrixOther, typename CoordinateSystemOther>
  auto crossProduct(
      const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther, CoordinateSystemOther>& other) const {
    return typename CrossProduct<This, Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther,
                                              CoordinateSystemOther>>::type(quantityMatrix_.crossProduct(other.getQuantityMatrix()));
  }

  auto inverse() const { return typename Inverse<This>::type(quantityMatrix_.inverse()); }

  template <bool IsHomogeneousV = IsHomogeneous<UnitMatrixT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto norm() const {
    return quantityMatrix_.norm();
  }

  template <bool IsHomogeneousV = IsHomogeneous<UnitMatrixT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto squaredNorm() const {
    return quantityMatrix_.squaredNorm();
  }

 private:
  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantityMatrixOther, typename CoordinateSystemOther>
  // Allow other tensor types to support lazy evaluation.
  Tensor(FromTensor /*tag*/,
         const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther, CoordinateSystemOther>& other)
      : quantityMatrix_(other.getQuantityMatrix()) {
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOther>, "Coordinate systems do not match.");
  }
  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantityMatrixOther, typename CoordinateSystemOther>
  // Allow other tensor types to support lazy evaluation.
  Tensor(FromTensor /*tag*/, Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantityMatrixOther, CoordinateSystemOther>&& other)
      : quantityMatrix_(std::move(other.getQuantityMatrix())) {
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOther>, "Coordinate systems do not match.");
  }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  Tensor(FromQuantityMatrix /*tag*/, const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& quantityMatrix)
      : quantityMatrix_(quantityMatrix) {}
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  Tensor(FromQuantityMatrix /*tag*/, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>&& quantityMatrix)
      : quantityMatrix_(std::move(quantityMatrix)) {}

  template <typename... Ts>
  explicit Tensor(FromQuantityMatrixArgs /*tag*/, const Ts&... ts) : quantityMatrix_(ts...) {}
  template <typename... Ts>
  explicit Tensor(FromQuantityMatrixArgs /*tag*/, Ts&&... ts) : quantityMatrix_(std::forward<Ts>(ts)...) {}

  QuantityMatrixT quantityMatrix_;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_, typename... Ts>
struct GetTag<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<IsSpecializationInts<first, Tensor>::value, FromTensor,
                          typename IfThenElse<(IsSpecialization<Ts, QuantityMatrix>::value && ...), FromQuantityMatrix,
                                              FromQuantityMatrixArgs>::type>::type,
      typename IfThenElse<(IsSpecialization<Ts, QuantityMatrix>::value && ...), FromQuantityMatrix, FromQuantityMatrixArgs>::type>::type;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct Dimension<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  static constexpr int value = Dimension<QuantityMatrixT_>::value;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct Rows<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  static constexpr int value = Rows<QuantityMatrixT_>::value;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct Cols<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  static constexpr int value = Cols<QuantityMatrixT_>::value;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
struct Sum<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
           Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, typename Sum<QuantityMatrixLhsT_, QuantityMatrixRhsT_>::type,
                      typename Sum<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
struct Difference<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
                  Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, typename Difference<QuantityMatrixLhsT_, QuantityMatrixRhsT_>::type,
                      typename Difference<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
struct Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
               Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>> {
  // TODO(remod): Do we need to add indices checks?
  using type = Tensor<ContravariantIndicesLhs - CovariantIndicesRhs, CovariantIndicesLhs - ContravariantIndicesRhs,
                      typename Product<QuantityMatrixLhsT_, QuantityMatrixRhsT_>::type,
                      typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type>;
};
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhs, typename UnitRhs>
struct Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
               Quantity<ValueRhs, UnitRhs>> {
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs,
                      typename Product<QuantityMatrixLhsT_, Quantity<ValueRhs, UnitRhs>>::type, CoordinateSystemLhsT_>;
};
template <typename ValueLhs, typename UnitLhs, int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_,
          typename CoordinateSystemRhsT_>
struct Product<Quantity<ValueLhs, UnitLhs>,
               Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>> {
  using type = typename Product<Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>,
                                Quantity<ValueLhs, UnitLhs>>::type;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhs, typename UnitRhs>
struct Division<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
                Quantity<ValueRhs, UnitRhs>> {
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs,
                      typename Division<QuantityMatrixLhsT_, Quantity<ValueRhs, UnitRhs>>::type, CoordinateSystemLhsT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct Transpose<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename Transpose<QuantityMatrixT_>::type, CoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_, int BlockRowId,
          int BlockColId, int BlockRows, int BlockCols>
struct Block<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>, BlockRowId, BlockColId, BlockRows,
             BlockCols> {
  using type = Tensor<ContravariantIndices, CovariantIndices,
                      typename Block<QuantityMatrixT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type, CoordinateSystemT_>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
struct DotProduct<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
                  Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  using type = typename DotProduct<QuantityMatrixLhsT_, QuantityMatrixRhsT_>::type;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
struct CrossProduct<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
                    Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == 1 && CovariantIndicesLhs == 0, "Cross product is only defined for vectors.");
  static_assert(ContravariantIndicesRhs == 1 && CovariantIndicesRhs == 0, "Cross product is only defined for vectors.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  using type = Tensor<1, 0, typename CrossProduct<QuantityMatrixLhsT_, QuantityMatrixRhsT_>::type, CoordinateSystemLhsT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct Inverse<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  // TODO(remod): Is this correct?
  using type = Tensor<CovariantIndices, ContravariantIndices, typename Inverse<QuantityMatrixT_>::type, CoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct GetCoordinateSystem<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  using type = CoordinateSystemT_;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct IsAbsolute<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  static constexpr bool value = IsAbsolute<QuantityMatrixT_>::value;
};
template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct MakeRelative<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename MakeRelative<QuantityMatrixT_>::type, CoordinateSystemT_>;
  static type make(const Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>& object) {
    return type(makeRelative(object.getQuantityMatrix()));
  }
};
template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct MakeAbsolute<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename MakeAbsolute<QuantityMatrixT_>::type, CoordinateSystemT_>;
  static type make(const Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>& object) {
    return type(makeAbsolute(object.getQuantityMatrix()));
  }
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_,
          typename NewCoordinateSystemT_>
struct CastCoordinateSystem<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>, NewCoordinateSystemT_> {
  using type = Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, NewCoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct Evaluate<Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename Evaluate<QuantityMatrixT_>::type, CoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
auto operator+(const Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>& tensor) {
  return tensor;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
auto operator+(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  return typename Sum<
      Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>>::type(lhs.getQuantityMatrix() +
                                                                                                              rhs.getQuantityMatrix());
}

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
auto operator-(const Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>& tensor) {
  return Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>(-tensor.getQuantityMatrix());
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
auto operator-(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  return typename Difference<
      Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>>::type(lhs.getQuantityMatrix() -
                                                                                                              rhs.getQuantityMatrix());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhs>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, typename AccessUnits<SpecializationDummy<ValueRhs>>::Dimensionless>(rhs);
}
template <typename ValueLhs, int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_,
          typename CoordinateSystemRhsT_>
auto operator*(const ValueLhs lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  return rhs * lhs;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhs, typename UnitRhs>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
                          Quantity<ValueRhs, UnitRhs>>::type(lhs.getQuantityMatrix() * rhs);
}
template <typename ValueLhs, typename UnitLhs, int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_,
          typename CoordinateSystemRhsT_>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  return rhs * lhs;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  return typename Product<
      Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>>::type(lhs.getQuantityMatrix() *
                                                                                                              rhs.getQuantityMatrix());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhs>
auto operator/(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, typename AccessUnits<SpecializationDummy<ValueRhs>>::Dimensionless>(rhs);
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhs, typename UnitRhs>
auto operator/(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
               const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>,
                           Quantity<ValueRhs, UnitRhs>>::type(lhs.getValueMatrix() / rhs.getValue());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
bool operator==(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
                const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantityMatrixLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantityMatrixRhsT_, typename CoordinateSystemRhsT_>
bool operator!=(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantityMatrixLhsT_, CoordinateSystemLhsT_>& lhs,
                const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantityMatrixRhsT_, CoordinateSystemRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <int ContravariantIndices, int CovariantIndices, typename QuantityMatrixT_, typename CoordinateSystemT_>
std::ostream& operator<<(std::ostream& stream,
                         const Tensor<ContravariantIndices, CovariantIndices, QuantityMatrixT_, CoordinateSystemT_>& tensor) {
  stream << tensor.getQuantityMatrix();
  return stream;
}

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Vector = Tensor<1, 0, QuantityVectorT_, CoordinateSystemT_>;
template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename Unit, typename CoordinateSystemT_>
using HomogeneousVector = Vector<QuantityMatrix<ValueMatrix<Scalar, Dimension, 1>, TypeMatrix<Dimension, 1, Unit>>, CoordinateSystemT_>;

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Covector = Tensor<0, 1, QuantityVectorT_, CoordinateSystemT_>;
template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename Unit, typename CoordinateSystemT_>
using HomogeneousCovector = Covector<QuantityMatrix<ValueMatrix<Scalar, 1, Dimension>, TypeMatrix<1, Dimension, Unit>>, CoordinateSystemT_>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using PositionPoint =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::AbsoluteLength, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using PositionPoint2 = PositionPoint<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using PositionPoint3 = PositionPoint<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using DirectionVector =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Dimensionless, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using DirectionVector2 = DirectionVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using DirectionVector3 = DirectionVector<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using PositionVector =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Length, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using PositionVector2 = PositionVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using PositionVector3 = PositionVector<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using VelocityVector =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Velocity, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using VelocityVector2 = VelocityVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using VelocityVector3 = VelocityVector<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AccelerationVector =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Acceleration, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AccelerationVector2 = AccelerationVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AccelerationVector3 = AccelerationVector<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AngularVelocityVector =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::AngularVelocity, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AngularVelocityVector2 = AngularVelocityVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AngularVelocityVector3 = AngularVelocityVector<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AngularAccelerationVector =
    HomogeneousVector<Dimension, ValueMatrix, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::AngularAcceleration,
                      CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AngularAccelerationVector2 = AngularAccelerationVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using AngularAccelerationVector3 = AngularAccelerationVector<3, ValueMatrix, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using SpacetimePoint = PositionPoint<Dimension, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using SpacetimePoint2 = SpacetimePoint<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using SpacetimePoint3 = SpacetimePoint<3, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using SpacetimePoint4 = SpacetimePoint<4, ValueMatrix, Scalar, CoordinateSystem>;
template <int DimensionV, template <typename, int, int> typename ValueMatrixT, typename ScalarT, typename CoordinateSystemT>
using SpacetimeVector = PositionVector<DimensionV, ValueMatrixT, ScalarT, CoordinateSystemT>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using SpacetimeVector2 = SpacetimeVector<2, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using SpacetimeVector3 = SpacetimeVector<3, ValueMatrix, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrixT, typename ScalarT, typename CoordinateSystemT>
using SpacetimeVector4 = SpacetimeVector<4, ValueMatrixT, ScalarT, CoordinateSystemT>;

}  // namespace wuzp
