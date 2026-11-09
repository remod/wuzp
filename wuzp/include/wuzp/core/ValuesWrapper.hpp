#pragma once

#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

/**
 * Wrapper for the actual implementation of a value container.
 * This is required as some libraries, e.g. Eigen, use different types for matrices and expressions, but we want a single trait
 * specialization to catch all of them. We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam ImplementationT_ ImplementationT of the values.
 */
// TODO(remod): Rename to Values?
template <AnyMatrixSize MatrixSizeT_, ArithmeticValue ValueT_, typename ImplementationT_>
class ValuesWrapper {
 public:
  using MatrixSizeT = MatrixSizeT_;
  using ValueT = ValueT_;
  using ImplementationT = std::decay_t<ImplementationT_>;
  using ThisT = ValuesWrapper<MatrixSizeT, ValueT, ImplementationT>;
  // TODO(remod): Use member types instead of traits for value, dim, rows, cols, in all classes?

  ValuesWrapper() { ReserveMemory<ImplementationT_, MatrixSizeT_>::run(implementation_); }

  //! Copies the values. The copy never shares them with the original, also with backends whose matrices share their values
  //! when they are copy constructed, e.g. Blitz++ (see Copy).
  ValuesWrapper(const ValuesWrapper& other) : implementation_(copyImplementation(other.implementation_)) {}
  //! Moves the values. A moved-from object may be empty with some backends, e.g. Blitz++ (see Move): it can only be assigned to
  //! or destroyed.
  ValuesWrapper(ValuesWrapper&& other) noexcept
      : implementation_(Move<AccessImplementation<ImplementationT>>::construct(other.implementation_)) {}
  //! Only evaluated values can be assigned to: an unevaluated value, e.g. the lazy sum of two matrices of the backend, refers to
  //! its operands, so assigning to it would write into them (Blitz++) or does not compile (Eigen).
  ValuesWrapper& operator=(const ValuesWrapper& other) {
    if (this != &other) {
      *this = ValuesWrapper(other);
    }
    return *this;
  }
  ValuesWrapper& operator=(ValuesWrapper&& other) noexcept {
    static_assert(std::is_same_v<ImplementationT, typename Evaluate<AccessImplementation<ImplementationT>>::type>,
                  "Unevaluated values cannot be assigned to. Evaluate them first.");
    if (this != &other) {
      Move<AccessImplementation<ImplementationT>>::assign(implementation_, other.implementation_);
    }
    return *this;
  }
  ~ValuesWrapper() = default;

  // Copies and moves are left to the constructors above, see CopiesOrMoves.
  template <typename T0, typename... Ts>
    requires(!IsSpecialization<T0, Tag>::value && !CopiesOrMoves<ValuesWrapper, T0, Ts...>)
  // Allow implicit construction from similar values of the same size. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!IsValidConversion<ThisT, std::remove_cvref_t<T0>, std::remove_cvref_t<Ts>...>::value) ValuesWrapper(T0&& t0, Ts&&... ts)
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

  template <int RowIdV_, int ColIdV_>
  auto get() const {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    return implementation_(RowIdV_, ColIdV_);
  }

  auto get() const
    requires(Dimension<ThisT>::value == 0)
  {
    return get<0, 0>();
  }

  auto get(int rowId, int colId) const { return implementation_(rowId, colId); }

  template <int RowIdV_, int ColIdV_, typename ValueOtherT_>
  ThisT& set(ValueOtherT_ value) {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    implementation_(RowIdV_, ColIdV_) = value;
    return *this;
  }

  template <typename ValueOtherT_>
  ThisT& set(ValueOtherT_ value)
    requires(Dimension<ThisT>::value == 0)
  {
    return set<0, 0>(value);
  }

  template <typename ValueOtherT_>
  ThisT& set(int rowId, int colId, ValueOtherT_ value) {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    implementation_(rowId, colId) = value;
    return *this;
  }

  // NOLINTNEXTLINE(google-explicit-constructor)
  operator typename GetValue<ThisT>::type() const
    requires(Dimension<ThisT>::value == 0)
  {
    // TODO(remod): This is an assumption which works for Eigen and Blitz, one could generalize it.
    return implementation_(0, 0);
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  bool isApprox(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    return Comparison<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::approxEqual(
        implementation_, other.getImplementation(), precision);
  }

  // The member functions and operators return evaluated results, which do not refer to their operands, so that they can be
  // stored, e.g. in `auto` variables. The traits, e.g. Block<ThisT, ...>::type, are the lazy types they evaluate.

  template <int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
  auto getBlock() const {
    return typename Block<ThisT, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type(
               Block<AccessImplementation<ImplementationT>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::value(implementation_))
        .evaluate();
  }

  auto transpose() const {
    return typename Transpose<ThisT>::type{Transpose<AccessImplementation<ImplementationT>>::value(implementation_)}.evaluate();
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto tensorProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename TensorProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
               TensorProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(
                   implementation_, other.getImplementation()))
        .evaluate();
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto innerProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename InnerProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
               InnerProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(
                   implementation_, other.getImplementation()))
        .evaluate();
  }

  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  auto outerProduct(const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other) const {
    return typename OuterProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>::type(
               OuterProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(
                   implementation_, other.getImplementation()))
        .evaluate();
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
               CrossProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(
                   implementation_, other.getImplementation()))
        .evaluate();
  }

  auto norm() const { return Norm<AccessImplementation<ImplementationT>>::value(implementation_); }
  auto squaredNorm() const { return SquaredNorm<AccessImplementation<ImplementationT>>::value(implementation_); }
  auto determinant() const { return Determinant<AccessImplementation<ImplementationT>>::value(implementation_); }

  auto inverse() const {
    return typename Inverse<ThisT>::type{Inverse<AccessImplementation<ImplementationT>>::value(implementation_)}.evaluate();
  }
  auto tensorInverse() const {
    return typename TensorInverse<ThisT>::type{TensorInverse<AccessImplementation<ImplementationT>>::value(implementation_)}.evaluate();
  }

  ThisT& setImplementation(const ImplementationT& implementation) {
    implementation_ = implementation;
    return *this;
  }
  ImplementationT& implementation() { return implementation_; }
  const ImplementationT& getImplementation() const { return implementation_; }

  auto evaluate() const& { return typename Evaluate<ThisT>::type{Evaluate<AccessImplementation<ImplementationT>>::value(implementation_)}; }
  //! Evaluated values of a temporary, e.g. of the result of an operation: if they are evaluated already, they are moved.
  auto evaluate() && {
    if constexpr (std::is_same_v<typename Evaluate<ThisT>::type, ThisT>) {
      return std::move(*this);
    } else {
      return std::as_const(*this).evaluate();
    }
  }

 private:
  template <typename MatrixSizeOtherT_, typename ValueOtherT_, typename ImplementationOtherT_>
  // Allow other implementation types to support lazy evaluation.
  ValuesWrapper(FromSimilar /*tag*/, const ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>& other)
      : implementation_(copyImplementation(other.getImplementation())) {
    using ConversionT = IsValidConversion<ThisT, ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>>;
    static_assert(ConversionT::RowsMatchV, "Number of rows do not match.");
    static_assert(ConversionT::ColsMatchV, "Number of cols do not match.");
  }

  //! Constructor for non-arithmetic types, e.g. expressions.
  template <typename T0>
    requires(!ArithmeticValue<std::remove_cvref_t<T0>>)
  ValuesWrapper(FromImplementationArgs /*tag*/, T0&& t0) : implementation_(copyImplementation(std::forward<T0>(t0))) {}
  //! Constructor for a set of objects to initialize the values.
  template <typename T0, typename... Ts>
  ValuesWrapper(FromImplementationArgs /*tag*/, T0&& t0, Ts&&... ts) {
    static_assert(sizeof...(Ts) + 1 == MatrixSizeT::NumElementsV, "Number of arguments does not match size of matrix.");
    ReserveMemory<ImplementationT_, MatrixSizeT_>::run(implementation_);
    Initialize<ImplementationT_, std::tuple<T0, Ts...>>::run(implementation_, std::forward<T0>(t0), std::forward<Ts>(ts)...);
  }

  //! An implementation constructed from another one. It copies an object of the same type with Copy, so that it does not share
  //! the values of a matrix, e.g. of another value matrix. Other types, e.g. expressions and temporaries, are converted or moved.
  template <typename ImplementationOtherT_>
  static ImplementationT copyImplementation(ImplementationOtherT_&& other) {
    if constexpr (std::is_lvalue_reference_v<ImplementationOtherT_> &&
                  std::is_same_v<std::remove_cvref_t<ImplementationOtherT_>, ImplementationT>) {
      return Copy<AccessImplementation<ImplementationT>>::value(other);
    } else {
      return ImplementationT(std::forward<ImplementationOtherT_>(other));
    }
  }

  ImplementationT implementation_;
};

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_, typename MatrixSizeOtherT_, typename ValueOtherT_,
          typename ImplementationOtherT_>
struct IsValidConversion<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>,
                         ValuesWrapper<MatrixSizeOtherT_, ValueOtherT_, ImplementationOtherT_>> {
  // The converting constructor checks these with static_asserts, which explain why a conversion fails.
  static constexpr bool RowsMatchV = Rows<MatrixSizeT_>::value == Rows<MatrixSizeOtherT_>::value;
  static constexpr bool ColsMatchV = Cols<MatrixSizeT_>::value == Cols<MatrixSizeOtherT_>::value;
  static constexpr bool value = RowsMatchV && ColsMatchV;
};

template <typename... Xs, typename... Ts>
struct GetTag<ValuesWrapper<Xs...>, Ts...> {
 private:
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type =
      typename IfThenElse<sizeof...(Ts) == 1, typename IfThenElse<AnyValuesWrapper<first>, FromSimilar, FromImplementationArgs>::type,
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

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
struct Negation<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>> {
  using type = ValuesWrapper<MatrixSizeT_, ValueT_, typename Negation<AccessImplementation<ImplementationT_>>::type>;
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
  using type =
      typename Evaluate<typename InnerProduct<typename Transpose<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>>::type,
                                              ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::type>::type;
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

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
struct Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhsT_> {
  using type = ValuesWrapper<MatrixSizeLhsT_, typename Division<ValueType<ValueLhsT_>, ValueType<ValueRhsT_>>::type,
                             typename Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::type>;
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

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_,
          int BlockColsV_>
struct Block<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  using type =
      ValuesWrapper<typename Block<MatrixSizeT_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type, ValueT_,
                    typename Block<AccessImplementation<ImplementationT_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type>;
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
                                                                                                            rhs.getImplementation()))
          .evaluate();
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
      type(Difference<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(lhs.getImplementation(),
                                                                                                                   rhs.getImplementation()))
          .evaluate();
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto operator-(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return difference(lhs, rhs);
}

template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
auto negation(const ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>& rhs) {
  return typename Negation<ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>>::type(
             Negation<AccessImplementation<ImplementationT_>>::value(rhs.getImplementation()))
      .evaluate();
}
template <typename MatrixSizeT_, typename ValueT_, typename ImplementationT_>
auto operator-(const ValuesWrapper<MatrixSizeT_, ValueT_, ImplementationT_>& rhs) {
  return negation(rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Product<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhsT_>::type(
             Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs))
      .evaluate();
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
    return Result(Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs.get())).evaluate();
  } else if constexpr (std::is_same_v<MatrixSizeLhsT_, MatrixSize<1, 1>>) {
    return Result(Product<AccessImplementation<ImplementationRhsT_>, ValueLhsT_>::value(rhs.getImplementation(), lhs.get())).evaluate();
  } else {
    return Result(Product<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
                      lhs.getImplementation(), rhs.getImplementation()))
        .evaluate();
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
  return lhs.innerProduct(rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto outerProduct(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
                  const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return lhs.outerProduct(rhs);
}

template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto division(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>, ValueRhsT_>::type(
             Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs))
      .evaluate();
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_,
          typename ImplementationRhsT_>
auto division(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
              const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  using Result = typename Division<ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>,
                                   ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>>::type;
  if constexpr (std::is_same_v<MatrixSizeRhsT_, MatrixSize<1, 1>>) {
    return Result(Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_>::value(lhs.getImplementation(), rhs.get())).evaluate();
  } else {
    return Result(Division<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::value(
                      lhs.getImplementation(), rhs.getImplementation()))
        .evaluate();
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
