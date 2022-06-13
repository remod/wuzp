#pragma once

#include <ostream>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
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
// using Scalar = Tensor<0, 0>;
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
template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
class Tensor {
 public:
  using QuantitiesT = QuantitiesT_;
  using CoordinateSystemT = CoordinateSystemT_;
  using UnitsT = typename QuantitiesT::UnitsT;
  using ValuesT = typename QuantitiesT::ValuesT;
  using ValueT = typename QuantitiesT::ValueT;
  using This = Tensor<ContravariantIndices, CovariantIndices, QuantitiesT, CoordinateSystemT>;

  static_assert(CovariantIndices >= 0, "Covariant index must not be negative.");
  static_assert(ContravariantIndices >= 0, "Contravariant index must not be negative.");

  Tensor() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar tensors. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromSimilar>) Tensor(const T0& t0, const Ts&... ts)
      : Tensor(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar tensors. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromSimilar>) Tensor(T0&& t0, Ts&&... ts)
      : Tensor(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static This zero() { return This(QuantitiesT::zero()); }
  static This random() { return This(QuantitiesT::random()); }

  template <typename ValuesOtherT_, typename UnitsOther>
  void setQuantities(const Quantities<ValuesOtherT_, UnitsOther>& component) {
    quantities_ = component;
  }
  const QuantitiesT& getQuantities() const { return quantities_; }

  template <typename ValuesOtherT_>
  void setValues(const ValuesOtherT_& values) {
    quantities_.setValues(values);
  }
  const ValuesT& getValues() const { return quantities_.getValues(); }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantitiesOther, typename CoordinateSystemOther>
  bool isApprox(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    // Note: Allow other tensor types to support lazy evaluation.
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOther>, "Coordinate systems do not match.");
    return quantities_.isApprox(other.getQuantities(), precision);
  }

  template <int Dimension = Dimension<This>::value, typename = std::enable_if_t<Dimension == 0>>
  auto getQuantity() const {
    return quantities_.get();
  }

  // TODO(remod): Remove 'get' from all getters?
  template <int RowId, int ColId>
  auto getQuantity() const {
    return quantities_.template get<RowId, ColId>();
  }

  auto getQuantity(const int rowId, const int colId) const { return quantities_.get(rowId, colId); }

  template <typename QuantityT_, int Dimension = Dimension<This>::value, typename = std::enable_if_t<Dimension == 0>>
  void setQuantity(const QuantityT_& quantity) {
    return quantities_.set(quantity);
  }

  template <int RowId, int ColId, typename QuantityT_>
  void setQuantity(const QuantityT_& quantity) {
    return quantities_.template set<RowId, ColId>(quantity);
  }

  template <typename Quantity>
  void setQuantity(const int rowId, const int colId, const Quantity& quantity) {
    quantities_.set(rowId, colId, quantity);
  }

  // TODO(remod): Rename to "slice"?
  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantities_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(quantities_.transpose()); }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantitiesOther, typename CoordinateSystemOther>
  auto dotProduct(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>& other) const {
    return
        typename DotProduct<This, Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>>::type(
            quantities_.dotProduct(other.getQuantities()));
  }

  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantitiesOther, typename CoordinateSystemOther>
  auto crossProduct(const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>& other) const {
    return
        typename CrossProduct<This, Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>>::type(
            quantities_.crossProduct(other.getQuantities()));
  }

  auto inverse() const { return typename Inverse<This>::type(quantities_.inverse()); }

  template <bool IsHomogeneousV = IsHomogeneous<UnitsT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto norm() const {
    return quantities_.norm();
  }

  template <bool IsHomogeneousV = IsHomogeneous<UnitsT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto squaredNorm() const {
    return quantities_.squaredNorm();
  }

 private:
  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantitiesOther, typename CoordinateSystemOther>
  // Allow other tensor types to support lazy evaluation.
  Tensor(FromSimilar /*tag*/, const Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>& other)
      : quantities_(other.getQuantities()) {
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOther>, "Coordinate systems do not match.");
  }
  template <int ContravariantIndicesOther, int CovariantIndicesOther, typename QuantitiesOther, typename CoordinateSystemOther>
  // Allow other tensor types to support lazy evaluation.
  Tensor(FromSimilar /*tag*/, Tensor<ContravariantIndicesOther, CovariantIndicesOther, QuantitiesOther, CoordinateSystemOther>&& other)
      : quantities_(std::move(other.getQuantities())) {
    static_assert(ContravariantIndices == ContravariantIndicesOther, "Contravariant indices do not match.");
    static_assert(CovariantIndices == CovariantIndicesOther, "Covariant indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOther>, "Coordinate systems do not match.");
  }

  //  template <typename ValuesOtherT_, typename UnitsOther>
  //  Tensor(FromImplementation /*tag*/, const Quantities<ValuesOtherT_, UnitsOther>& quantities) : quantities_(quantities) {}
  //  template <typename ValuesOtherT_, typename UnitsOther>
  //  Tensor(FromImplementation /*tag*/, Quantities<ValuesOtherT_, UnitsOther>&& quantities) : quantities_(std::move(quantities)) {}

  template <typename... Ts>
  explicit Tensor(FromImplementationArgs /*tag*/, const Ts&... ts) : quantities_(ts...) {}
  template <typename... Ts>
  explicit Tensor(FromImplementationArgs /*tag*/, Ts&&... ts) : quantities_(std::forward<Ts>(ts)...) {}

  QuantitiesT quantities_;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_, typename... Ts>
struct GetTag<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<IsSpecializationInts<first, Tensor>::value, FromSimilar,
                          typename IfThenElse<(IsSpecialization<Ts, Quantities>::value && ...), FromImplementationArgs,
                                              FromImplementationArgs>::type>::type,
      typename IfThenElse<(IsSpecialization<Ts, Quantities>::value && ...), FromImplementationArgs, FromImplementationArgs>::type>::type;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct Dimension<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  static constexpr int value = Dimension<QuantitiesT_>::value;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct Rows<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  static constexpr int value = Rows<QuantitiesT_>::value;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct Cols<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  static constexpr int value = Cols<QuantitiesT_>::value;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
struct Sum<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
           Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, typename Sum<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Sum<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
struct Difference<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                  Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, typename Difference<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Difference<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
struct Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
               Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>> {
  // TODO(remod): Do we need to add indices checks?
  using type = Tensor<ContravariantIndicesLhs - CovariantIndicesRhs, CovariantIndicesLhs - ContravariantIndicesRhs,
                      typename Product<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type>;
};
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhsT_, typename UnitRhsT_>
struct Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
               Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs,
                      typename Product<QuantitiesLhsT_, Quantity<ValueRhsT_, UnitRhsT_>>::type, CoordinateSystemLhsT_>;
};
template <typename ValueLhsT_, typename UnitLhsT_, int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_>
struct Product<Quantity<ValueLhsT_, UnitLhsT_>,
               Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>> {
  using type = typename Product<Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>,
                                Quantity<ValueLhsT_, UnitLhsT_>>::type;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhsT_, typename UnitRhsT_>
struct Division<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = Tensor<ContravariantIndicesLhs, CovariantIndicesLhs,
                      typename Division<QuantitiesLhsT_, Quantity<ValueRhsT_, UnitRhsT_>>::type, CoordinateSystemLhsT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct Transpose<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename Transpose<QuantitiesT_>::type, CoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_, int BlockRowId,
          int BlockColId, int BlockRows, int BlockCols>
struct Block<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>, BlockRowId, BlockColId, BlockRows,
             BlockCols> {
  using type = Tensor<ContravariantIndices, CovariantIndices,
                      typename Block<QuantitiesT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type, CoordinateSystemT_>;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
struct DotProduct<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                  Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  using type = typename DotProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type;
};

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
struct CrossProduct<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                    Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>> {
  static_assert(ContravariantIndicesLhs == 1 && CovariantIndicesLhs == 0, "Cross product is only defined for vectors.");
  static_assert(ContravariantIndicesRhs == 1 && CovariantIndicesRhs == 0, "Cross product is only defined for vectors.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  using type = Tensor<1, 0, typename CrossProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type, CoordinateSystemLhsT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct Inverse<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  // TODO(remod): Is this correct?
  using type = Tensor<CovariantIndices, ContravariantIndices, typename Inverse<QuantitiesT_>::type, CoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct GetCoordinateSystem<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  using type = CoordinateSystemT_;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct IsAbsolute<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  static constexpr bool value = IsAbsolute<QuantitiesT_>::value;
};
template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct MakeRelative<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename MakeRelative<QuantitiesT_>::type, CoordinateSystemT_>;
  static type make(const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>& object) {
    return type(makeRelative(object.getQuantities()));
  }
};
template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct MakeAbsolute<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename MakeAbsolute<QuantitiesT_>::type, CoordinateSystemT_>;
  static type make(const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>& object) {
    return type(makeAbsolute(object.getQuantities()));
  }
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_,
          typename NewCoordinateSystemT_>
struct CastCoordinateSystem<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>, NewCoordinateSystemT_> {
  using type = Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, NewCoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
struct Evaluate<Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>> {
  using type = Tensor<ContravariantIndices, CovariantIndices, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_>;
};

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
auto operator+(const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>& rhs) {
  return rhs;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
auto operator+(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  return
      typename Sum<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                   Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>>::type(lhs.getQuantities() +
                                                                                                                       rhs.getQuantities());
}

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
auto operator-(const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>& rhs) {
  return Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>(-rhs.getQuantities());
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
auto operator-(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  return typename Difference<
      Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>>::type(lhs.getQuantities() -
                                                                                                          rhs.getQuantities());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const ValueRhsT_ rhs) {
  return lhs * Quantity<typename AccessValues<SpecializationDummy<ValueRhsT_>>::template Value<ValueRhsT_>,
                        typename AccessUnits<SpecializationDummy<ValueRhsT_>>::Dimensionless>(rhs);
}
template <typename ValueLhsT_, int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_>
auto operator*(const ValueLhsT_ lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  return rhs * lhs;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhsT_, typename UnitRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Product<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                          Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getQuantities() * rhs);
}
template <typename ValueLhsT_, typename UnitLhsT_, int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_>
auto operator*(const Quantity<ValueLhsT_, UnitLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  return rhs * lhs;
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  return typename Product<
      Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
      Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>>::type(lhs.getQuantities() *
                                                                                                          rhs.getQuantities());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhsT_>
auto operator/(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const ValueRhsT_ rhs) {
  return lhs / Quantity<typename AccessValues<SpecializationDummy<ValueRhsT_>>::template Value<ValueRhsT_>,
                        typename AccessUnits<SpecializationDummy<ValueRhsT_>>::Dimensionless>(rhs);
}
template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename ValueRhsT_, typename UnitRhsT_>
auto operator/(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
               const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Division<Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>,
                           Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValues() / rhs.getValue());
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
bool operator==(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
                const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  static_assert(ContravariantIndicesLhs == ContravariantIndicesRhs, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhs == CovariantIndicesRhs, "Covariant indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <int ContravariantIndicesLhs, int CovariantIndicesLhs, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          int ContravariantIndicesRhs, int CovariantIndicesRhs, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_>
bool operator!=(const Tensor<ContravariantIndicesLhs, CovariantIndicesLhs, QuantitiesLhsT_, CoordinateSystemLhsT_>& lhs,
                const Tensor<ContravariantIndicesRhs, CovariantIndicesRhs, QuantitiesRhsT_, CoordinateSystemRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_>
std::ostream& operator<<(std::ostream& lhs, const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Vector = Tensor<1, 0, QuantityVectorT_, CoordinateSystemT_>;
template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename Unit, typename CoordinateSystemT_>
using HomogeneousVector = Vector<Quantities<ValuesT_<Scalar, Dimension, 1>, TypeMatrix<Dimension, 1, Unit>>, CoordinateSystemT_>;

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Covector = Tensor<0, 1, QuantityVectorT_, CoordinateSystemT_>;
template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename Unit, typename CoordinateSystemT_>
using HomogeneousCovector = Covector<Quantities<ValuesT_<Scalar, 1, Dimension>, TypeMatrix<1, Dimension, Unit>>, CoordinateSystemT_>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using PositionPoint =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::AbsoluteLength, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using PositionPoint2 = PositionPoint<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using PositionPoint3 = PositionPoint<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using DirectionVector =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Dimensionless, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using DirectionVector2 = DirectionVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using DirectionVector3 = DirectionVector<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using PositionVector =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Length, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using PositionVector2 = PositionVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using PositionVector3 = PositionVector<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using VelocityVector =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Velocity, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using VelocityVector2 = VelocityVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using VelocityVector3 = VelocityVector<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AccelerationVector =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Acceleration, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AccelerationVector2 = AccelerationVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AccelerationVector3 = AccelerationVector<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AngularVelocityVector =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::AngularVelocity, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AngularVelocityVector2 = AngularVelocityVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AngularVelocityVector3 = AngularVelocityVector<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AngularAccelerationVector =
    HomogeneousVector<Dimension, ValuesT_, Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::AngularAcceleration,
                      CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AngularAccelerationVector2 = AngularAccelerationVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using AngularAccelerationVector3 = AngularAccelerationVector<3, ValuesT_, Scalar, CoordinateSystem>;

template <int Dimension, template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using SpacetimePoint = PositionPoint<Dimension, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using SpacetimePoint2 = SpacetimePoint<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using SpacetimePoint3 = SpacetimePoint<3, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using SpacetimePoint4 = SpacetimePoint<4, ValuesT_, Scalar, CoordinateSystem>;
template <int DimensionV, template <typename, int, int> typename ValuesT, typename ValueT, typename CoordinateSystemT>
using SpacetimeVector = PositionVector<DimensionV, ValuesT, ValueT, CoordinateSystemT>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using SpacetimeVector2 = SpacetimeVector<2, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using SpacetimeVector3 = SpacetimeVector<3, ValuesT_, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT, typename ValueT, typename CoordinateSystemT>
using SpacetimeVector4 = SpacetimeVector<4, ValuesT, ValueT, CoordinateSystemT>;

}  // namespace wuzp
