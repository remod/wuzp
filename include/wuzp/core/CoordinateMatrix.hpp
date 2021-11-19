#pragma once

#include <ostream>
#include <utility>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/CoordinateSystemMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"

namespace wuzp {

// template <int ContravariantIndices, int CovariantIndices>
// class Tensor;
//
// using Scalar = Tensor<0, 0>;
// using EuclideanVector = Tensor<1, 0>;       // R * v
// using LinearTransformation = Tensor<1, 1>;  // R^-1 * A * R
// using RotationMatrix = Tensor<1, 1>;        // R^-1 * A * R
// using InertiaTensor = Tensor<0, 2>;         // R^T * A * R

// Orthogonal matrix: R^-1 == R^T

/**
 * Class representing a coordinate matrix.
 * A coordinate matrix is a matrix of quantities and coordinate systems.
 * A coordinate matrix does _not_ include any information about how it is transformed - for this, have a look at the Tensor class.
 * @tparam QuantityMatrix_         Type of the underlying quantity matrix.
 * @tparam CoordinateSystemMatrix_ Matrix of coordinate systems.
 */
template <typename QuantityMatrix_, typename CoordinateSystemMatrix_>
class CoordinateMatrix {
 public:
  using QuantityMatrixT = QuantityMatrix_;
  using CoordinateSystemMatrixT = CoordinateSystemMatrix_;
  using ValueMatrixT = typename QuantityMatrixT::ValueMatrixT;
  using This = CoordinateMatrix<QuantityMatrixT, CoordinateSystemMatrixT>;

  static_assert(Rows<QuantityMatrixT>::value == Rows<CoordinateSystemMatrixT>::value, "Number of rows do not match.");
  static_assert(Cols<QuantityMatrixT>::value == Cols<CoordinateSystemMatrixT>::value, "Number of columns do not match.");

  CoordinateMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar coordinate matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromCoordinateMatrix>) CoordinateMatrix(const T0& t0, const Ts&... ts)
      : CoordinateMatrix(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar coordinate matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromCoordinateMatrix>) CoordinateMatrix(T0&& t0, Ts&&... ts)
      : CoordinateMatrix(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  ~CoordinateMatrix() = default;

  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  This& operator=(const CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>& other) {
    quantityMatrix_ = other.getQuantityMatrix();
    return *this;
  }

  static This zero() { return This(QuantityMatrixT::zero()); }
  static This random() { return This(QuantityMatrixT::random()); }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  void setQuantityMatrix(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& quantityMatrix) {
    quantityMatrix_ = quantityMatrix;
  }
  const QuantityMatrixT& getQuantityMatrix() const { return quantityMatrix_; }

  template <typename ValueMatrixOther>
  void setValueMatrix(const ValueMatrixOther& values) {
    quantityMatrix_.setValueMatrix(values);
  }
  const ValueMatrixT& getValueMatrix() const { return quantityMatrix_.getValueMatrix(); }

  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  bool isApprox(const CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<CoordinateSystemMatrixT, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(quantityMatrix_.transpose()); }

  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  auto dot(const CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>& other) const {
    return typename DotProduct<This, CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>>::type(
        quantityMatrix_.dot(other.getQuantityMatrix()));
  }

  auto inverse() const { return typename Inverse<This>::type(quantityMatrix_.inverse()); }

 private:
  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  CoordinateMatrix(FromCoordinateMatrix /*tag*/, const CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>& other)
      : quantityMatrix_(other.getQuantityMatrix()) {
    static_assert(std::is_same<CoordinateSystemMatrixT, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
  }
  template <typename QuantityMatrixOther, typename CoordinateSystemMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  CoordinateMatrix(FromCoordinateMatrix /*tag*/, CoordinateMatrix<QuantityMatrixOther, CoordinateSystemMatrixOther>&& other)
      : quantityMatrix_(std::move(other.getQuantityMatrix())) {
    static_assert(std::is_same<CoordinateSystemMatrixT, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
  }

  // Allow other value types to support lazy evaluation.
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  CoordinateMatrix(FromQuantityMatrix /*tag*/, const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& quantityMatrix)
      : quantityMatrix_(quantityMatrix) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  CoordinateMatrix(FromQuantityMatrix /*tag*/, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>&& quantityMatrix)
      : quantityMatrix_(std::move(quantityMatrix)) {}

  template <typename... Ts>
  CoordinateMatrix(FromQuantityMatrixArgs /*tag*/, const Ts&... ts) : quantityMatrix_(ts...) {}
  template <typename... Ts>
  CoordinateMatrix(FromQuantityMatrixArgs /*tag*/, Ts&&... ts) : quantityMatrix_(std::forward<Ts>(ts)...) {}

  QuantityMatrixT quantityMatrix_;
};

template <typename... Xs, typename... Ts>
struct GetTag<CoordinateMatrix<Xs...>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename Chose<
      sizeof...(Ts) == 1,
      typename Chose<
          IsSpecialization<first, CoordinateMatrix>::value, FromCoordinateMatrix,
          typename Chose<(IsSpecialization<Ts, QuantityMatrix>::value && ...), FromQuantityMatrix, FromQuantityMatrixArgs>::type>::type,
      typename Chose<(IsSpecialization<Ts, QuantityMatrix>::value && ...), FromQuantityMatrix, FromQuantityMatrixArgs>::type>::type;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct Dimension<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  static constexpr int value = Dimension<QuantityMatrix>::value;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct Rows<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  static constexpr int value = Rows<QuantityMatrix>::value;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct Cols<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  static constexpr int value = Cols<QuantityMatrix>::value;
};

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Sum<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>, CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = CoordinateMatrix<typename Sum<QuantityMatrixLhs, QuantityMatrixRhs>::type,
                                typename Sum<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;
};

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Difference<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>,
                  CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = CoordinateMatrix<typename Difference<QuantityMatrixLhs, QuantityMatrixRhs>::type,
                                typename Difference<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;
};

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename Product<QuantityMatrixLhs, Quantity<ValueRhs, UnitRhs>>::type, CoordinateSystemMatrixLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = typename Product<CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
// Use case: Rotate a vector.
template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Product<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>,
               CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>> {
  // NEW: The outcome is a coordinate matrix (cross product, rotation, etc.)
  using type = CoordinateMatrix<typename Product<QuantityMatrixLhs, QuantityMatrixRhs>::type,
                                typename Product<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;

  // TODO(remod): Remove if unused.
  // OLD: The outcome is a quantity matrix (scalar product).
  // public:
  //  using type = QuantityMatrix<typename template Product<ValueMatrixLhs, ValueMatrixRhs>::type,
  //                              typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;
  //
  // private:
  //  using CoordinateSystemMatrixProduct = typename Product<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type;
  //  static_assert(IsHomogeneous<CoordinateSystemMatrixProduct>::value &&
  //                    std::is_same<typename Element<CoordinateSystemMatrixProduct, 0, 0>::type, NoCoordinateSystem>::value,
  //                "Coordinate systems do not match.");
};

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename Division<QuantityMatrixLhs, Quantity<ValueRhs, UnitRhs>>::type, CoordinateSystemMatrixLhs>;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct Transpose<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename Transpose<QuantityMatrix>::type, typename Transpose<CoordinateSystemMatrix>::type>;
};

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
struct DotProduct<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>,
                  CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = typename DotProduct<QuantityMatrixLhs, QuantityMatrixRhs>::type;

 private:
  using assert_frames_match = typename DotProduct<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = CoordinateMatrix<typename Block<QuantityMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<CoordinateSystemMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct GetCoordinateSystem<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  static_assert(IsHomogeneous<CoordinateSystemMatrix>::value, "Coordinate systems are heterogeneous.");
  using type = typename std::tuple_element<0, typename GetTypes<CoordinateSystemMatrix>::type>::type;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct IsAbsolute<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  static constexpr bool value = IsAbsolute<QuantityMatrix>::value;
};
template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct MakeRelative<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename MakeRelative<QuantityMatrix>::type, CoordinateSystemMatrix>;
  static type make(const CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>& object) { return type(object.getValueMatrix()); }
};
template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct MakeAbsolute<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename MakeAbsolute<QuantityMatrix>::type, CoordinateSystemMatrix>;
  static type make(const CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>& object) { return type(object.getValueMatrix()); }
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix, typename NewCoordinateSystem>
struct CastCoordinateSystem<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>, NewCoordinateSystem> {
  using type = CoordinateMatrix<QuantityMatrix, typename CastCoordinateSystem<CoordinateSystemMatrix, NewCoordinateSystem>::type>;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
struct Evaluate<CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename Evaluate<QuantityMatrix>::type, CoordinateSystemMatrix>;
};

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
auto operator+(const CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>& coordinateMatrix) {
  return coordinateMatrix;
}
template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator+(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return
      typename Sum<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>,
                   CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getQuantityMatrix() + rhs.getQuantityMatrix());
}

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
auto operator-(const CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>& coordinateMatrix) {
  return CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>(-coordinateMatrix.getQuantityMatrix());
}
template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator-(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Difference<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>,
                             CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getQuantityMatrix() -
                                                                                                   rhs.getQuantityMatrix());
}

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs>
auto operator*(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const ValueLhs lhs, const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getQuantityMatrix() * rhs);
}
template <typename ValueLhs, typename UnitLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Product<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>,
                          CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getQuantityMatrix() *
                                                                                                rhs.getQuantityMatrix());
}

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs>
auto operator/(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getQuantityMatrix() / rhs);
}

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
bool operator==(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
                const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  static_assert(std::is_same<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::value, "Coordinate systems do not match.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename QuantityMatrixLhs, typename CoordinateSystemMatrixLhs, typename QuantityMatrixRhs, typename CoordinateSystemMatrixRhs>
bool operator!=(const CoordinateMatrix<QuantityMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
                const CoordinateMatrix<QuantityMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename QuantityMatrix, typename CoordinateSystemMatrix>
std::ostream& operator<<(std::ostream& stream, const CoordinateMatrix<QuantityMatrix, CoordinateSystemMatrix>& coordinateMatrix) {
  stream << coordinateMatrix.getQuantityMatrix();
  return stream;
}

}  // namespace wuzp
