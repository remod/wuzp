#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/MatrixSize.hpp"

namespace wuzp {

/**
 * Wrapper for the actual implementation of a value container.
 * This is required as some libraries, e.g. Eigen, use different types for matrices and expressions, but we want a single trait
 * specialization to catch all of them. We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam ImplementationT_ ImplementationT of the values.
 */
// TODO(remod): Rename to Values?
template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
class ValuesWrapper {
 public:
  using MatrixSizeT = MatrixSizeT_;
  using ValueT = ValueT_;
  using ImplementationT = std::decay_t<ImplementationT_>;
  using ThisT = ValuesWrapper<MatrixSizeT, ValueT, ImplementationT>;
  // TODO(remod): Use member types instead of traits for value, dim, rows, cols, in all classes?

  ValuesWrapper() { ReserveMemory<ImplementationT_, MatrixSizeT_>::run(implementation_); }

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromSimilar>) ValuesWrapper(T0&& t0, Ts&&... ts)
      : ValuesWrapper(typename GetTag<ThisT, T0, Ts...>::type{}, std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  //! Values that are all zero.
  static ThisT zero() {
    ThisT result;
    Zero<AccessImplementation<ImplementationT>>::run(result.implementation_);
    return result;
  }

  //! Random values, uniformly distributed in [-1, 1].
  static ThisT random() {
    static_assert(std::is_floating_point_v<ValueT>, "Random values are only supported for floating point types.");
    ThisT result;
    Random<AccessImplementation<ImplementationT>>::run(result.implementation_);
    return result;
  }

  template <int RowId, int ColId>
  auto get() const {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    return implementation_(RowId, ColId);
  }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  auto get() const {
    return get<0, 0>();
  }

  auto get(int rowId, int colId) const { return implementation_(rowId, colId); }

  template <int RowId, int ColId, typename ValueT>
  ThisT& set(ValueT value) {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    implementation_(RowId, ColId) = value;
    return *this;
  }

  template <typename ValueT, int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  ThisT& set(ValueT value) {
    return set<0, 0>(value);
  }

  template <typename ValueT>
  ThisT& set(int rowId, int colId, ValueT value) {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    implementation_(rowId, colId) = value;
    return *this;
  }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator typename GetValue<ThisT>::type() const {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    return implementation_(0, 0);
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOther_>
  bool isApprox(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOther_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    return Comparison<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOther_>>::approxEqual(
        implementation_, other.getImplementation(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto getBlock() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        Block<AccessImplementation<ImplementationT>, BlockRowId, BlockColId, BlockRows, BlockCols>::value(implementation_));
  }

  auto transpose() const {
    return typename Transpose<ThisT>::type{Transpose<AccessImplementation<ImplementationT>>::value(implementation_)};
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto tensorProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename TensorProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
        TensorProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(
            implementation_, other.getImplementation()));
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto innerProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename InnerProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
        InnerProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(implementation_,
                                                                                                                other.getImplementation()));
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto outerProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename OuterProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
        OuterProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(implementation_,
                                                                                                                other.getImplementation()));
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto dotProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    // Reuse the innerProduct instead of delegating it to the implementation.
    return typename DotProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
        transpose().innerProduct(other));
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto crossProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename CrossProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
        CrossProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(implementation_,
                                                                                                                other.getImplementation()));
  }

  auto norm() const { return Norm<AccessImplementation<ImplementationT>>::value(implementation_); }
  auto squaredNorm() const { return SquaredNorm<AccessImplementation<ImplementationT>>::value(implementation_); }
  auto determinant() const { return Determinant<AccessImplementation<ImplementationT>>::value(implementation_); }

  auto inverse() const { return typename Inverse<ThisT>::type{Inverse<AccessImplementation<ImplementationT>>::value(implementation_)}; }
  auto tensorInverse() const {
    return typename TensorInverse<ThisT>::type{TensorInverse<AccessImplementation<ImplementationT>>::value(implementation_)};
  }

  ThisT& setImplementation(const ImplementationT& implementation) {
    implementation_ = implementation;
    return *this;
  }
  ImplementationT& implementation() { return implementation_; }
  const ImplementationT& getImplementation() const { return implementation_; }

  auto evaluate() const { return typename Evaluate<ThisT>::type{Evaluate<AccessImplementation<ImplementationT>>::value(implementation_)}; }

 private:
  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  // Allow other implementation types to support lazy evaluation.
  ValuesWrapper(FromSimilar /*tag*/, const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other)
      : implementation_(other.getImplementation()) {
    // TODO(remod): Should we assert that the matrix sizes match?
  }

  //! Constructor for non-arithmetic types, e.g. expressions.
  template <typename T0, typename = std::enable_if_t<!std::is_arithmetic_v<std::remove_const_t<std::remove_reference_t<T0>>>>>
  ValuesWrapper(FromImplementationArgs /*tag*/, T0&& t0) : implementation_(std::forward<T0>(t0)) {}
  //! Constructor for a set of objects to initialize the values.
  template <typename T0, typename... Ts>
  ValuesWrapper(FromImplementationArgs /*tag*/, T0&& t0, Ts&&... ts) {
    static_assert(sizeof...(Ts) + 1 == MatrixSizeT::NumElementsV, "Number of arguments does not match size of matrix.");
    ReserveMemory<ImplementationT_, MatrixSizeT_>::run(implementation_);
    Initialize<ImplementationT_, std::tuple<T0, Ts...>>::run(implementation_, std::forward<T0>(t0), std::forward<Ts>(ts)...);
  }

  ImplementationT implementation_;
};

template <typename... Xs, typename... Ts>
struct GetTag<ValuesWrapper<Xs...>, Ts...> {
 private:
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type =
      typename IfThenElse<sizeof...(Ts) == 1,
                          typename IfThenElse<IsSpecialization<first, ValuesWrapper>::value, FromSimilar, FromImplementationArgs>::type,
                          FromImplementationArgs>::type;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct GetValue<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  using type = ValueT_;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct Rows<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  static constexpr int value = Rows<MatrixSizeT_>::value;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct Cols<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  static constexpr int value = Cols<MatrixSizeT_>::value;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct Sum<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
           ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename Sum<MatrixSizeLhsT_, MatrixSizeRhsT_>::type, typename Sum<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                    typename Sum<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct Difference<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                  ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename Difference<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                    typename Difference<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                    typename Difference<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

// Rules to implement product:
// 1. If the LHS is a scalar (dimension == 0), revert the order and reuse the implementation where the scalar is on the RHS.
// 2. To avoid overhead, do not create temporary objects to reuse implementation, e.g. creating a 1x1 matrix from a scalar.
// template <typename ImplementationLhsT_, int DimensionLhsV_, typename ImplementationRhsT_, int DimensionRhsV_>
// struct ProductImpl;
// template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
//          typename ImplementationRhsT_, int DimensionRhsV_>
// struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, 0,
//                   ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, DimensionRhsV_> {
//  using type = typename ProductImpl<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, DimensionRhsV_,
//                                    ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, 0>::type;
//};
// template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, int DimensionLhsV_, typename MatrixSizeRhsT_,
//          typename ValueRhsT_, typename ImplementationRhsT_>
// struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, DimensionLhsV_,
//                   ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, 0> {
//  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
//                             typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
//                             typename Product<AccessImplementation<ImplementationLhsT_>,
//                             AccessImplementation<ImplementationRhsT_>>::type>;
//};
// template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
//          typename ImplementationRhsT_>
// struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, 0,
//                   ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, 0> {
//  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
//                             typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
//                             typename Product<AccessImplementation<ImplementationLhsT_>,
//                             AccessImplementation<ImplementationRhsT_>>::type>;
//};
// template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, int DimensionLhsV_, typename MatrixSizeRhsT_,
//          typename ValueRhsT_, typename ImplementationRhsT_, int DimensionRhsV_>
// struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, DimensionLhsV_,
//                   ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, DimensionRhsV_> {
//  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
//                             typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
//                             typename Product<AccessImplementation<ImplementationLhsT_>,
//                             AccessImplementation<ImplementationRhsT_>>::type>;
//};

template <typename ValueLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
struct Product<ValueLhsT_, ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type = typename Product<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>, ValueLhsT_>::type;
};
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
struct Product<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhsT_> {
  using type = ValuesWrapper<MatrixSizeLhsT_, typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::type>;
};
template <typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
struct Product<ValuesWrapper<MatrixSize<1, 1>, ValueLhsT_, ImplementationLhsT_>,
               ValuesWrapper<MatrixSize<1, 1>, ValueRhsT_, ImplementationRhsT_>> {
  // If both matrices have size 1x1, we keep the order lhs * rhs.
  using type = ValuesWrapper<typename Product<MatrixSize<1, 1>, MatrixSize<1, 1>>::type,
                             typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::type>;
};
template <typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
struct Product<ValuesWrapper<MatrixSize<1, 1>, ValueLhsT_, ImplementationLhsT_>,
               ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type = typename Product<ValueLhsT_, ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::type;
};
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
struct Product<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
               ValuesWrapper<MatrixSize<1, 1>, ValueRhsT_, ImplementationRhsT_>> {
  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSize<1, 1>>::type,
                             typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::type>;
};
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct Product<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
               ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Product<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct TensorProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                     ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename TensorProduct<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                    typename TensorProduct<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                    typename TensorProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct InnerProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                    ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename InnerProduct<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                    typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                    typename InnerProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct OuterProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                    ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename OuterProduct<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                    typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                    typename OuterProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct DotProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                  ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type = typename InnerProduct<typename Transpose<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>>::type,
                                     ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::type;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct CrossProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                    ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename CrossProduct<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                    typename Product<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                    typename CrossProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhs_>
struct Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhs_> {
  using type = ValuesWrapper<MatrixSizeLhsT_, typename Division<ValueType<ValueLhsT_>, ValueType<ValueRhs_>>::type,
                             typename Division<AccessImplementation<ImplementationLhsT_>, ValueRhs_>::type>;
};
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
struct Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                ValuesWrapper<MatrixSize<1, 1>, ValueRhsT_, ImplementationRhsT_>> {
  using type = ValuesWrapper<typename Division<MatrixSizeLhsT_, MatrixSize<1, 1>>::type,
                             typename Division<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::type>;
};
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
struct Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>> {
  using type = ValuesWrapper<typename Division<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             typename Division<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Division<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct Transpose<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  using type =
      ValuesWrapper<typename Transpose<MatrixSizeT_>::type, ValueT_, typename Transpose<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct Inverse<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  using type = ValuesWrapper<typename Inverse<MatrixSizeT_>::type, ValueT_, typename Inverse<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct TensorInverse<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  using type = ValuesWrapper<typename TensorInverse<MatrixSizeT_>::type, ValueT_,
                             typename TensorInverse<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = ValuesWrapper<typename Block<MatrixSizeT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type, ValueT_,
                             typename Block<AccessImplementation<ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct Evaluate<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  using type = ValuesWrapper<MatrixSizeT_, ValueT_, typename Evaluate<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_, typename... Args>
void initialize(ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>& values, Args&&... args);

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto sum(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
         const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return typename Sum<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                      ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::
      type(Sum<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(lhs.getImplementation(),
                                                                                                            rhs.getImplementation()));
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto operator+(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return sum(lhs, rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto difference(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
                const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return typename Difference<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                             ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::
      type(Difference<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
          lhs.getImplementation(), rhs.getImplementation()));
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto operator-(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return difference(lhs, rhs);
}

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
auto negation(const ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>& rhs) {
  return ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>(
      Negation<AccessImplementation<ImplementationT_>>::value(rhs.getImplementation()));
}
template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
auto operator-(const ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>& rhs) {
  return negation(rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Product<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhsT_>::type(
      Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs));
}
template <typename ValueLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto product(const ValueLhsT_ lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(rhs, lhs);
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
             const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  using Result = typename Product<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                                  ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::type;
  // If both matrices have size 1x1, we keep the order lhs * rhs.
  if constexpr (std::is_same_v<MatrixSizeRhsT_, MatrixSize<1, 1>>) {
    return Result(Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs.get()));
  } else if constexpr (std::is_same_v<MatrixSizeLhsT_, MatrixSize<1, 1>>) {
    return Result(Product<AccessImplementation<ImplementationRhsT_>, ValueLhsT_>::value(rhs.getImplementation(), lhs.get()));
  } else {
    return Result(Product<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
        lhs.getImplementation(), rhs.getImplementation()));
  }
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return product(lhs, rhs);
}

// Eigen::CwiseBinaryOp<Eigen::internal::scalar_product_op<int, int>,
//                     const Eigen::CwiseBinaryOp<
//                         Eigen::internal::scalar_product_op<int, int>, const Eigen::Array<int, 1, 1, 0, 1, 1>,
//                         const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<int>, const Eigen::Array<int, 1, 1, 0, 1, 1>>>,
//                     const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<int>, const Eigen::Array<int, 1, 1, 0, 1, 1>>>::
//    CwiseBinaryOp(
//        Eigen::CwiseBinaryOp<Eigen::internal::scalar_product_op<int, int>, const Eigen::Array<int, 1, 1, 0, 1, 1>,
//                             const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<int>, const Eigen::Array<int, 1, 1, 0, 1,
//                             1>>>)

// Eigen::CwiseBinaryOp<
//     Eigen::internal::scalar_product_op<double, double>,
//     const Eigen::CwiseBinaryOp<
//         Eigen::internal::scalar_sum_op<double, double>,
//         const Eigen::CwiseBinaryOp<
//             Eigen::internal::scalar_product_op<double, double>, const Eigen::Array<double, 1, 1, 0, 1, 1>,
//             const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, const Eigen::Array<double, 1, 1, 0, 1, 1>>>,
//         const Eigen::CwiseBinaryOp<
//             Eigen::internal::scalar_product_op<double, double>, const Eigen::Array<double, 1, 1, 0, 1, 1>,
//             const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, const Eigen::Array<double, 1, 1, 0, 1, 1>>>>,
//     const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, const Eigen::Array<double, 1, 1, 0, 1, 1>>>::
//     CwiseBinaryOp(Eigen::CwiseBinaryOp<
//                   Eigen::internal::scalar_product_op<double, double>, const Eigen::Array<double, 1, 1, 0, 1, 1>,
//                   const Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<double>, const Eigen::Array<double, 1, 1, 0, 1,
//                   1>>>)

template <typename ValueLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto operator*(const ValueLhsT_ lhs, const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto innerProduct(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
                  const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return typename InnerProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                               ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::
      type(InnerProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
          lhs.getImplementation(), rhs.getImplementation()));
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto outerProduct(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
                  const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return typename OuterProduct<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                               ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::
      type(OuterProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
          lhs.getImplementation(), rhs.getImplementation()));
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto division(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhsT_>::type(
      Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs));
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto division(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
              const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  using Result = typename Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                                   ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::type;
  if constexpr (std::is_same_v<MatrixSizeRhsT_, MatrixSize<1, 1>>) {
    return Result(Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs.get()));
  } else {
    return Result(Division<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
        lhs.getImplementation(), rhs.getImplementation()));
  }
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto operator/(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return division(lhs, rhs);
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto operator/(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return division(lhs, rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
bool operator==(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
                const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return Comparison<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::equal(lhs.getImplementation(),
                                                                                                                 rhs.getImplementation());
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
bool operator!=(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
                const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
std::ostream& operator<<(std::ostream& lhs, const ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>& rhs) {
  lhs << rhs.evaluate().getImplementation();
  return lhs;
}

}  // namespace wuzp
