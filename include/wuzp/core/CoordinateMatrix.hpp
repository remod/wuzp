#pragma once

#include <ostream>
#include <utility>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/CoordinateSystemMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrixTraits.hpp"

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

// TODO(remod): Just template on QuantityMatrix and CoordinateSystemMatrix?
template <typename ValueMatrix_, typename UnitMatrix_, typename CoordinateSystemMatrix_>
class CoordinateMatrix {
 public:
  using ValueMatrixT = ValueMatrix_;
  using UnitMatrixT = UnitMatrix_;
  using CoordinateSystemMatrixT = CoordinateSystemMatrix_;
  using This = CoordinateMatrix<ValueMatrixT, UnitMatrixT, CoordinateSystemMatrixT>;

  static_assert(ValueMatrixTraits::Rows<ValueMatrixT>::value == Rows<UnitMatrixT>::value &&
                    ValueMatrixTraits::Rows<ValueMatrixT>::value == Rows<CoordinateSystemMatrixT>::value,
                "Number of rows do not match.");
  static_assert(ValueMatrixTraits::Cols<ValueMatrixT>::value == Cols<UnitMatrixT>::value &&
                    ValueMatrixTraits::Cols<ValueMatrixT>::value == Cols<CoordinateSystemMatrixT>::value,
                "Number of columns do not match.");

  CoordinateMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!is_specialization<T0, Tag>::value>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  CoordinateMatrix(const T0& t0, const Ts&... ts) : CoordinateMatrix(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!is_specialization<T0, Tag>::value>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  CoordinateMatrix(T0&& t0, Ts&&... ts)
      : CoordinateMatrix(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  ~CoordinateMatrix() = default;

  template <typename ValueMatrixOther, typename UnitMatrixOther, typename CoordinateSystemMatrixOther>
  This& operator=(const CoordinateMatrix<ValueMatrixOther, UnitMatrixOther, CoordinateSystemMatrixOther>& other) {
    quantityMatrix_ = other.getQuantityMatrix();
    return *this;
  }

  static This zero() { return This(QuantityMatrix<ValueMatrixT, UnitMatrixT>::zero()); }

  const QuantityMatrix<ValueMatrixT, UnitMatrixT>& getQuantityMatrix() const { return quantityMatrix_; }

  void setValueMatrix(const ValueMatrixT& values) { quantityMatrix_.setValueMatrix(values); }
  const ValueMatrixT& getValueMatrix() const { return quantityMatrix_.getValueMatrix(); }

  template <typename ValueMatrixOther, typename UnitMatrixOther, typename CoordinateSystemMatrixOther>
  bool isApprox(const CoordinateMatrix<ValueMatrixOther, UnitMatrixOther, CoordinateSystemMatrixOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<CoordinateSystemMatrixT, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
    return quantityMatrix_.isApprox(other.getQuantityMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        quantityMatrix_.template block<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(getQuantityMatrix().transpose()); }

  template <typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
  auto dot(const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& other) const {
    return getQuantityMatrix().dot(other.getQuantityMatrix());
  }

  auto inverse() const { return typename Inverse<This>::type(quantityMatrix_.inverse()); }

 private:
  template <typename ValueMatrixOther, typename UnitMatrixOther, typename CoordinateSystemMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  explicit CoordinateMatrix(FromCoordinateMatrix,
                            const CoordinateMatrix<ValueMatrixOther, UnitMatrixOther, CoordinateSystemMatrixOther>& other)
      : quantityMatrix_(other.getQuantityMatrix()) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
    static_assert(std::is_same<CoordinateSystemMatrixT, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
  }
  template <typename ValueMatrixOther, typename UnitMatrixOther, typename CoordinateSystemMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  explicit CoordinateMatrix(FromCoordinateMatrix, CoordinateMatrix<ValueMatrixOther, UnitMatrixOther, CoordinateSystemMatrixOther>&& other)
      : quantityMatrix_(std::move(other.getQuantityMatrix())) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
    static_assert(std::is_same<CoordinateSystemMatrixT, CoordinateSystemMatrixOther>::value, "Coordinate systems do not match.");
  }

  // Allow other value types to support lazy evaluation.
  template <typename ValueMatrixOther>
  explicit CoordinateMatrix(FromQuantityMatrix, const QuantityMatrix<ValueMatrixOther, UnitMatrixT>& quantityMatrix)
      : quantityMatrix_(quantityMatrix) {}
  // Allow other value types to support lazy evaluation.
  template <typename ValueMatrixOther>
  explicit CoordinateMatrix(FromQuantityMatrix, QuantityMatrix<ValueMatrixOther, UnitMatrixT>&& quantityMatrix)
      : quantityMatrix_(std::move(quantityMatrix)) {}

  template <typename... Ts>
  explicit CoordinateMatrix(FromQuantityMatrixArgs, const Ts&... ts) : quantityMatrix_(ts...) {}
  template <typename... Ts>
  explicit CoordinateMatrix(FromQuantityMatrixArgs, Ts&&... ts) : quantityMatrix_(std::forward<Ts>(ts)...) {}

  QuantityMatrix<ValueMatrixT, UnitMatrixT> quantityMatrix_;
};

template <typename... Xs, typename... Ts>
struct GetTag<CoordinateMatrix<Xs...>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename Chose<
      sizeof...(Ts) == 1,
      typename Chose<
          is_specialization<first, CoordinateMatrix>::value, FromCoordinateMatrix,
          typename Chose<(is_specialization<Ts, QuantityMatrix>::value && ...), FromQuantityMatrix, FromQuantityMatrixArgs>::type>::type,
      typename Chose<(is_specialization<Ts, QuantityMatrix>::value && ...), FromQuantityMatrix, FromQuantityMatrixArgs>::type>::type;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct Dimension<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  static constexpr int value = ValueMatrixTraits::Dimension<ValueMatrix>::value;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Sum<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
           CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Sum<ValueMatrixLhs, ValueMatrixRhs>::type,
                                typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type,
                                typename Sum<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Difference<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                  CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Difference<ValueMatrixLhs, ValueMatrixRhs>::type,
                                typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type,
                                typename Difference<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueRhs>::type,
                                typename Product<UnitMatrixLhs, UnitRhs>::type, CoordinateSystemMatrixLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type =
      typename Product<CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
// Use case: Rotate a vector.
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
               CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  // NEW: The outcome is a coordinate matrix (cross product, rotation, etc.)
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueMatrixRhs>::type,
                                typename Product<UnitMatrixLhs, UnitMatrixRhs>::type,
                                typename Product<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type>;

  // TODO(remod): Remove if unused.
  // OLD: The outcome is a quantity matrix (scalar product).
  // public:
  //  using type = QuantityMatrix<typename ValueMatrixTraits::template Product<ValueMatrixLhs, ValueMatrixRhs>::type,
  //                              typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;
  //
  // private:
  //  using CoordinateSystemMatrixProduct = typename Product<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type;
  //  static_assert(IsHomogeneous<CoordinateSystemMatrixProduct>::value &&
  //                    std::is_same<typename Element<CoordinateSystemMatrixProduct, 0, 0>::type, NoCoordinateSystem>::value,
  //                "Coordinate systems do not match.");
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Division<ValueMatrixLhs, ValueRhs>::type,
                                typename Division<UnitMatrixLhs, UnitRhs>::type, CoordinateSystemMatrixLhs>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct Transpose<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type,
                                typename Transpose<CoordinateSystemMatrix>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
struct DotProduct<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                  CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>> {
  using type = Quantity<typename ValueMatrixTraits::template DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type,
                        typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
  using assert_frames_match = typename DotProduct<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::type;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix, int BlockRowId, int BlockColId, int BlockRows,
          int BlockCols>
struct Block<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::template Block<ValueMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                                typename Block<CoordinateSystemMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct GetCoordinateSystem<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  static_assert(IsHomogeneous<CoordinateSystemMatrix>::value, "Coordinate systems are heterogeneous.");
  using type = typename std::tuple_element<0, typename GetTypes<CoordinateSystemMatrix>::type>::type;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct IsAbsolute<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  static constexpr bool value = IsAbsolute<UnitMatrix>::value;
};
template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct MakeRelative<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<ValueMatrix, typename MakeRelative<UnitMatrix>::type, CoordinateSystemMatrix>;
  static type make(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& object) {
    return type(object.getValueMatrix());
  }
};
template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct MakeAbsolute<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<ValueMatrix, typename MakeAbsolute<UnitMatrix>::type, CoordinateSystemMatrix>;
  static type make(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& object) {
    return type(object.getValueMatrix());
  }
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix, typename NewCoordinateSystem>
struct CastCoordinateSystem<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>, NewCoordinateSystem> {
  using type = CoordinateMatrix<ValueMatrix, UnitMatrix, typename CastCoordinateSystem<CoordinateSystemMatrix, NewCoordinateSystem>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
struct Evaluate<CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>> {
  using type = CoordinateMatrix<typename ValueMatrixTraits::Evaluate<ValueMatrix>::type, UnitMatrix, CoordinateSystemMatrix>;
};

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
auto operator+(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& coordinateMatrix) {
  return coordinateMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator+(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Sum<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                      CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getQuantityMatrix() +
                                                                                                        rhs.getQuantityMatrix());
}

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
auto operator-(const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& coordinateMatrix) {
  return CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>(-coordinateMatrix.getQuantityMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator-(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Difference<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                             CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getQuantityMatrix() -
                                                                                                               rhs.getQuantityMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const ValueLhs lhs, const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getQuantityMatrix() * rhs);
}
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
auto operator*(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return typename Product<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>,
                          CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>>::type(lhs.getQuantityMatrix() *
                                                                                                            rhs.getQuantityMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs>
auto operator/(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
               const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(
      lhs.getQuantityMatrix() / rhs);
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
bool operator==(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
                const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  static_assert(std::is_same<CoordinateSystemMatrixLhs, CoordinateSystemMatrixRhs>::value, "Coordinate systems do not match.");
  return lhs.getQuantityMatrix() == rhs.getQuantityMatrix();
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename CoordinateSystemMatrixLhs, typename ValueMatrixRhs,
          typename UnitMatrixRhs, typename CoordinateSystemMatrixRhs>
bool operator!=(const CoordinateMatrix<ValueMatrixLhs, UnitMatrixLhs, CoordinateSystemMatrixLhs>& lhs,
                const CoordinateMatrix<ValueMatrixRhs, UnitMatrixRhs, CoordinateSystemMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueMatrix, typename UnitMatrix, typename CoordinateSystemMatrix>
std::ostream& operator<<(std::ostream& stream, const CoordinateMatrix<ValueMatrix, UnitMatrix, CoordinateSystemMatrix>& coordinateMatrix) {
  stream << coordinateMatrix.getQuantityMatrix();
  return stream;
}

}  // namespace wuzp
