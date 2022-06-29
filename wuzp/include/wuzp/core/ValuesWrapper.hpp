#pragma once

#include "wuzp/core/MatrixSize.hpp"

namespace wuzp {

/**
 * Wrapper for the actual implementation of a value container.
 * This is required as some libraries, e.g. Eigen, use different types for matrices and expressions, but we want a single trait
 * specialization to catch all of them. We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam ImplementationT_ ImplementationT of the values.
 */
template <typename MatrixSizeT_, typename ImplementationT_>
class ValuesWrapper {
 public:
  using MatrixSizeT = MatrixSizeT_;
  using ImplementationT = std::decay_t<ImplementationT_>;
  using ThisT = ValuesWrapper<MatrixSizeT, ImplementationT>;
  // TODO(remod): Use member types instead of traits for value, dim, rows, cols, in all classes?
  using ValueT = typename GetValue<ThisT>::type;

  ValuesWrapper() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromSimilar>) ValuesWrapper(T0&& t0, Ts&&... ts)
      : ValuesWrapper(typename GetTag<ThisT, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return Zero<ThisT>::value(); }
  static ThisT random() { return Random<ThisT>::value(); }

  template <int RowId, int ColId>
  auto get() const {
    return implementation_(RowId, ColId);
  }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  auto get() const {
    return get<0, 0>();
  }

  auto get(int rowId, int colId) const { return implementation_(rowId, colId); }

  template <int RowId, int ColId, typename ValueT>
  ThisT& set(ValueT value) {
    implementation_(RowId, ColId) = value;
    return *this;
  }

  template <typename ValueT, int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  ThisT& set(ValueT value) {
    return set<0, 0>(value);
  }

  template <typename ValueT>
  ThisT& set(int rowId, int colId, ValueT value) {
    implementation_(rowId, colId) = value;
    return *this;
  }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator typename GetValue<ThisT>::type() const {
    return implementation_(0, 0);
  }

  template <typename MatrixSizeOtherT_, typename ImplementationOther_>
  bool isApprox(const ValuesWrapper<MatrixSizeOtherT_, ImplementationOther_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    return implementation_.isApprox(other.getImplementation(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto getBlock() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        Block<AccessImplementation<ImplementationT>, BlockRowId, BlockColId, BlockRows, BlockCols>::value(implementation_));
  }

  auto transpose() const {
    return typename Transpose<ThisT>::type(Transpose<AccessImplementation<ImplementationT>>::value(implementation_));
  }

  template <typename MatrixSizeOtherT_, typename ImplementationOther_>
  auto dotProduct(const ValuesWrapper<MatrixSizeOtherT_, ImplementationOther_>& other) const {
    return typename DotProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ImplementationOther_>>::type(transpose() * other);
  }

  template <typename MatrixSizeOtherT_, typename ImplementationOtherT_>
  auto crossProduct(const ValuesWrapper<MatrixSizeOtherT_, ImplementationOtherT_>& other) const {
    return typename CrossProduct<ThisT, ValuesWrapper<MatrixSizeOtherT_, ImplementationOtherT_>>::type(
        CrossProduct<AccessImplementation<ImplementationT>, AccessImplementation<ImplementationOtherT_>>::value(implementation_,
                                                                                                                other.getImplementation()));
  }

  auto norm() const { return implementation_.norm(); }
  auto squaredNorm() const { return implementation_.squaredNorm(); }
  auto determinant() const { return implementation_.determinant(); }

  auto inverse() const { return typename Inverse<ThisT>::type(Inverse<AccessImplementation<ImplementationT>>::value(implementation_)); }

  ThisT& setImplementation(const ImplementationT& implementation) {
    implementation_ = implementation;
    return *this;
  }
  const ImplementationT& getImplementation() const { return implementation_; }

  auto evaluate() const { return typename Evaluate<ThisT>::type(Evaluate<AccessImplementation<ImplementationT>>::value(implementation_)); }

 private:
  template <typename MatrixSizeOtherT_, typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  ValuesWrapper(FromSimilar /*tag*/, const ValuesWrapper<MatrixSizeOtherT_, ImplementationOther_>& other)
      : implementation_(other.getImplementation()) {}

  // Temporary Eigen objects do not have a default constructor, therefore we must provide this method.
  template <typename T0>
  ValuesWrapper(FromImplementationArgs /*tag*/, T0&& t0) : implementation_(std::forward<T0>(t0)) {}
  template <typename T0, typename... Ts>
  ValuesWrapper(FromImplementationArgs /*tag*/, T0&& t0, Ts&&... ts) {
    initialize(*this, std::forward<T0>(t0), std::forward<Ts>(ts)...);
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

template <typename MatrixSizeT_, typename ImplementationT_>
struct GetValue<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  using type = typename std::decay_t<ImplementationT_>::Scalar;
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Rows<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  static constexpr int value = Rows<MatrixSizeT_>::value;
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Cols<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  static constexpr int value = Cols<MatrixSizeT_>::value;
};

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct Sum<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<typename Sum<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             decltype(std::declval<ImplementationLhsT_>() + std::declval<ImplementationRhsT_>())>;
};

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct Difference<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<typename Difference<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             decltype(std::declval<ImplementationLhsT_>() - std::declval<ImplementationRhsT_>())>;
};

template <typename T_, int DimensionV_>
struct AttemptCastToScalarImpl;
template <typename MatrixSizeT_, typename ImplementationT_, int DimensionV_>
struct AttemptCastToScalarImpl<ValuesWrapper<MatrixSizeT_, ImplementationT_>, DimensionV_> {
  using type = decltype(std::declval<ImplementationT_>());
};
template <typename MatrixSizeT_, typename ImplementationT_>
struct AttemptCastToScalarImpl<ValuesWrapper<MatrixSizeT_, ImplementationT_>, 0> {
  using type = decltype(std::declval<ImplementationT_>()(0, 0));
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct AttemptCastToScalar {
  using type = typename AttemptCastToScalarImpl<ValuesWrapper<MatrixSizeT_, ImplementationT_>,
                                                Dimension<ValuesWrapper<MatrixSizeT_, ImplementationT_>>::value>::type;
};

// Rules to implement product:
// 1. If the LHS is a scalar (dimension == 0), revert the order and reuse the implementation where the scalar is on the RHS.
// 2. To avoid overhead, do not create temporary objects to reuse implementation, e.g. creating a 1x1 matrix from a scalar.
template <typename ImplementationLhsT_, int DimensionLhsV_, typename ImplementationRhsT_, int DimensionRhsV_>
struct ProductImpl;
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_,
          int DimensionRhsV_>
struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, 0, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>,
                   DimensionRhsV_> {
  using type = typename ProductImpl<ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>, DimensionRhsV_,
                                    ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, 0>::type;
};
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, int DimensionLhsV_, typename MatrixSizeRhsT_,
          typename ImplementationRhsT_>
struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, DimensionLhsV_, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>,
                   0> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             decltype(std::declval<ImplementationLhsT_>() * std::declval<ImplementationRhsT_>()(0, 0))>;
};
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, 0, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>, 0> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             decltype(std::declval<ImplementationLhsT_>() * std::declval<ImplementationRhsT_>())>;
};
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, int DimensionLhsV_, typename MatrixSizeRhsT_,
          typename ImplementationRhsT_, int DimensionRhsV_>
struct ProductImpl<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, DimensionLhsV_, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>,
                   DimensionRhsV_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<typename Product<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             decltype(std::declval<ImplementationLhsT_>() * std::declval<ImplementationRhsT_>())>;
};

template <typename ValueLhs_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct Product<ValueLhs_, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  using type = typename Product<typename AccessValues<SpecializationDummy<ValueLhs_>>::template Value<ValueLhs_>,
                                ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type;
};
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename ValueRhs_>
struct Product<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValueRhs_> {
  using type = typename Product<ValueRhs_, ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>>::type;
};
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct Product<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  using type = typename ProductImpl<
      ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, Dimension<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>>::value,
      ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>, Dimension<ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::value>::type;
};

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename ValueRhs_>
struct Division<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<MatrixSizeLhsT_, decltype(std::declval<ImplementationLhsT_>() / std::declval<ValueRhs_>())>;
};
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct Division<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<typename Division<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                             decltype(std::declval<ImplementationLhsT_>() / std::declval<ImplementationRhsT_>())>;
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Transpose<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  using type = ValuesWrapper<typename Transpose<MatrixSizeT_>::type, typename Transpose<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct DotProduct<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  // Do not use the dot-product implementation, as we do not want a scalar as output, but a 1x1 matrix which is our implementation for
  // scalars.
  using type = typename Product<typename Transpose<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>>::type,
                                ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type;
};

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
struct CrossProduct<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>> {
  using type =
      ValuesWrapper<typename CrossProduct<MatrixSizeLhsT_, MatrixSizeRhsT_>::type,
                    typename CrossProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>::type>;
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Inverse<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  using type = ValuesWrapper<typename Inverse<MatrixSizeT_>::type, typename Inverse<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeT_, typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<ValuesWrapper<MatrixSizeT_, ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = ValuesWrapper<typename Block<MatrixSizeT_, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                             typename Block<AccessImplementation<ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Zero<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  static auto value() { return ValuesWrapper(Zero<AccessImplementation<ImplementationT_>>::value()); }
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Random<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  static auto value() { return ValuesWrapper(Random<AccessImplementation<ImplementationT_>>::value()); }
};

template <typename MatrixSizeT_, typename ImplementationT_>
struct Evaluate<ValuesWrapper<MatrixSizeT_, ImplementationT_>> {
  using type = ValuesWrapper<MatrixSizeT_, typename Evaluate<AccessImplementation<ImplementationT_>>::type>;
};

template <typename MatrixSizeT_, typename ImplementationT_, typename... Args>
void initialize(ValuesWrapper<MatrixSizeT_, ImplementationT_>& values, Args&&... args);

template <typename MatrixSizeT_, typename ImplementationT_>
auto operator+(const ValuesWrapper<MatrixSizeT_, ImplementationT_>& rhs) {
  return rhs;
}
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
auto operator+(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>& rhs) {
  return typename Sum<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type(
      lhs.getImplementation() + rhs.getImplementation());
}

template <typename MatrixSizeT_, typename ImplementationT_>
auto operator-(const ValuesWrapper<MatrixSizeT_, ImplementationT_>& rhs) {
  return ValuesWrapper<MatrixSizeT_, ImplementationT_>(-rhs.getImplementation());
}
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
auto operator-(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>& rhs) {
  return
      typename Difference<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type(
          lhs.getImplementation() - rhs.getImplementation());
}

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Product<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValueRhsT_>::type(lhs.getImplementation() * rhs);
}
template <typename ValueLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
auto operator*(const ValueLhsT_ lhs, const ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>& rhs) {
  return rhs * lhs;
}
template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>& rhs) {
  if constexpr (Dimension<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>>::value == 0 &&
                Dimension<ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::value == 0) {
    return typename Product<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type(
        lhs.getImplementation() * rhs.getImplementation());
  } else if constexpr (Dimension<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>>::value == 0) {
    return rhs * lhs;
  } else if constexpr (Dimension<ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::value == 0) {
    return typename Product<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type(
        lhs.getImplementation() * rhs.get());
  } else {
    return typename Product<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>>::type(
        lhs.getImplementation() * rhs.getImplementation());
  }
}

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename ValueRhsT_>
auto operator/(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Division<ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>, ValueRhsT_>::type(lhs.getImplementation() / rhs);
}

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
bool operator==(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs,
                const ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>& rhs) {
  return lhs.getImplementation() == rhs.getImplementation();
}

template <typename MatrixSizeLhsT_, typename ImplementationLhsT_, typename MatrixSizeRhsT_, typename ImplementationRhsT_>
bool operator!=(const ValuesWrapper<MatrixSizeLhsT_, ImplementationLhsT_>& lhs,
                const ValuesWrapper<MatrixSizeRhsT_, ImplementationRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <typename MatrixSizeT_, typename ImplementationT_>
std::ostream& operator<<(std::ostream& lhs, const ValuesWrapper<MatrixSizeT_, ImplementationT_>& rhs) {
  lhs << rhs.getImplementation();
  return lhs;
}

}  // namespace wuzp
