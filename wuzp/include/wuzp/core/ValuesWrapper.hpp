#pragma once

namespace wuzp {

/**
 * Wrapper for the actual implementation of a value container.
 * This is required as some libraries, e.g. Eigen, use different types for matrices and expressions, but we want a single trait
 * specialization to catch all of them. We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam ImplementationT_ ImplementationT of the values.
 */
template <typename ImplementationT_>
class ValuesWrapper {
 public:
  using ImplementationT = std::decay_t<ImplementationT_>;
  using ThisT = ValuesWrapper<ImplementationT>;
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

  template <typename ImplementationOther_>
  bool isApprox(const ValuesWrapper<ImplementationOther_>& other, const ValueT precision = Precision<ValueT>::value()) const {
    return implementation_.isApprox(other.getImplementation(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto getBlock() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        implementation_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<ThisT>::type(implementation_.transpose()); }

  template <typename ImplementationOther_>
  auto dotProduct(const ValuesWrapper<ImplementationOther_>& other) const {
    return typename DotProduct<ThisT, ValuesWrapper<ImplementationOther_>>::type(transpose() * other);
  }

  template <typename ImplementationOther_>
  auto crossProduct(const ValuesWrapper<ImplementationOther_>& other) const {
    return typename CrossProduct<ThisT, ValuesWrapper<ImplementationOther_>>::type(implementation_.cross(other.getImplementation()));
  }

  auto norm() const { return implementation_.norm(); }
  auto squaredNorm() const { return implementation_.squaredNorm(); }

  auto inverse() const { return typename Inverse<ThisT>::type(implementation_.inverse()); }

  ThisT& setImplementation(const ImplementationT& implementation) {
    implementation_ = implementation;
    return *this;
  }
  const ImplementationT& getImplementation() const { return implementation_; }

 private:
  template <typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  ValuesWrapper(FromSimilar /*tag*/, const ValuesWrapper<ImplementationOther_>& other) : implementation_(other.getImplementation()) {}

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

template <typename ImplementationT_>
struct GetValue<ValuesWrapper<ImplementationT_>> {
  using type = typename std::decay_t<ImplementationT_>::Scalar;
};

template <typename ImplementationT_>
struct Rows<ValuesWrapper<ImplementationT_>> {
  static constexpr int value = std::decay_t<ImplementationT_>::RowsAtCompileTime;
};

template <typename ImplementationT_>
struct Cols<ValuesWrapper<ImplementationT_>> {
  static constexpr int value = std::decay_t<ImplementationT_>::ColsAtCompileTime;
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Sum<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() + std::declval<ImplementationRhsT_>())>;
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Difference<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() - std::declval<ImplementationRhsT_>())>;
};

template <typename T_, int DimensionV_>
struct AttemptCastToScalarImpl;
template <typename ImplementationT_, int DimensionV_>
struct AttemptCastToScalarImpl<ValuesWrapper<ImplementationT_>, DimensionV_> {
  using type = decltype(std::declval<ImplementationT_>());
};
template <typename ImplementationT_>
struct AttemptCastToScalarImpl<ValuesWrapper<ImplementationT_>, 0> {
  using type = decltype(std::declval<ImplementationT_>()(0, 0));
};

template <typename ImplementationT_>
struct AttemptCastToScalar {
  using type = typename AttemptCastToScalarImpl<ValuesWrapper<ImplementationT_>, Dimension<ValuesWrapper<ImplementationT_>>::value>::type;
};

// Rules to implement product:
// 1. If the LHS is a scalar (dimension == 0), revert the order and reuse the implementation where the scalar is on the RHS.
// 2. To avoid overhead, do not create temporary objects to reuse implementation, e.g. creating a 1x1 matrix from a scalar.
template <typename ImplementationLhsT_, int DimensionLhsV_, typename ImplementationRhsT_, int DimensionRhsV_>
struct ProductImpl;
template <typename ImplementationLhsT_, typename ImplementationRhsT_, int DimensionRhsV_>
struct ProductImpl<ValuesWrapper<ImplementationLhsT_>, 0, ValuesWrapper<ImplementationRhsT_>, DimensionRhsV_> {
  using type = typename ProductImpl<ValuesWrapper<ImplementationRhsT_>, DimensionRhsV_, ValuesWrapper<ImplementationLhsT_>, 0>::type;
};
template <typename ImplementationLhsT_, int DimensionLhsV_, typename ImplementationRhsT_>
struct ProductImpl<ValuesWrapper<ImplementationLhsT_>, DimensionLhsV_, ValuesWrapper<ImplementationRhsT_>, 0> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() * std::declval<ImplementationRhsT_>()(0, 0))>;
};
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct ProductImpl<ValuesWrapper<ImplementationLhsT_>, 0, ValuesWrapper<ImplementationRhsT_>, 0> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() * std::declval<ImplementationRhsT_>())>;
};
template <typename ImplementationLhsT_, int DimensionLhsV_, typename ImplementationRhsT_, int DimensionRhsV_>
struct ProductImpl<ValuesWrapper<ImplementationLhsT_>, DimensionLhsV_, ValuesWrapper<ImplementationRhsT_>, DimensionRhsV_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() * std::declval<ImplementationRhsT_>())>;
};

template <typename ValueLhs_, typename ImplementationRhsT_>
struct Product<ValueLhs_, ValuesWrapper<ImplementationRhsT_>> {
  using type = typename Product<typename AccessValues<SpecializationDummy<ValueLhs_>>::template Value<ValueLhs_>,
                                ValuesWrapper<ImplementationRhsT_>>::type;
};
template <typename ImplementationLhsT_, typename ValueRhs_>
struct Product<ValuesWrapper<ImplementationLhsT_>, ValueRhs_> {
  using type = typename Product<ValueRhs_, ValuesWrapper<ImplementationLhsT_>>::type;
};
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Product<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>> {
  using type = typename ProductImpl<ValuesWrapper<ImplementationLhsT_>, Dimension<ValuesWrapper<ImplementationLhsT_>>::value,
                                    ValuesWrapper<ImplementationRhsT_>, Dimension<ValuesWrapper<ImplementationRhsT_>>::value>::type;
};

template <typename ImplementationLhsT_, typename ValueRhs_>
struct Division<ValuesWrapper<ImplementationLhsT_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() / std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Division<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>() / std::declval<ImplementationRhsT_>())>;
};

template <typename ImplementationT_>
struct Transpose<ValuesWrapper<ImplementationT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<const ImplementationT_>().transpose())>;
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct DotProduct<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>> {
  // Do not use the dot-product implementation, as we do not want a scalar as output, but a 1x1 matrix which is our implementation for
  // scalars.
  using type = typename Product<typename Transpose<ValuesWrapper<ImplementationLhsT_>>::type, ValuesWrapper<ImplementationRhsT_>>::type;
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct CrossProduct<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhsT_>().cross(std::declval<ImplementationRhsT_>()))>;
};

template <typename ImplementationT_>
struct Inverse<ValuesWrapper<ImplementationT_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationT_>().inverse())>;
};

template <typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<ValuesWrapper<ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<const ImplementationT_>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId))>;
};

template <typename ImplementationT_>
struct Zero<ValuesWrapper<ImplementationT_>> {
  static auto value() { return ValuesWrapper(ImplementationT_::Zero()); }
};

template <typename ImplementationT_>
struct Random<ValuesWrapper<ImplementationT_>> {
  static auto value() { return ValuesWrapper(ImplementationT_::Random()); }
};

template <typename ImplementationT_>
struct Evaluate<ValuesWrapper<ImplementationT_>> {
  using type = ValuesWrapper<std::decay_t<decltype(std::declval<ImplementationT_>().eval())>>;
};
template <typename ImplementationT_>
static auto evaluate(ValuesWrapper<ImplementationT_> t) {
  return typename Evaluate<ValuesWrapper<ImplementationT_>>::type(t.getImplementation());
}

template <typename ImplementationT_, typename... Args>
void initialize(ValuesWrapper<ImplementationT_>& values, Args&&... args);

template <typename ImplementationT_>
auto operator+(const ValuesWrapper<ImplementationT_>& rhs) {
  return rhs;
}
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
auto operator+(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValuesWrapper<ImplementationRhsT_>& rhs) {
  return typename Sum<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>>::type(lhs.getImplementation() +
                                                                                                    rhs.getImplementation());
}

template <typename ImplementationT_>
auto operator-(const ValuesWrapper<ImplementationT_>& rhs) {
  return ValuesWrapper<ImplementationT_>(-rhs.getImplementation());
}
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
auto operator-(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValuesWrapper<ImplementationRhsT_>& rhs) {
  return typename Difference<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>>::type(lhs.getImplementation() -
                                                                                                           rhs.getImplementation());
}

template <typename ImplementationLhsT_, typename ValueRhsT_>
auto operator*(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Product<ValuesWrapper<ImplementationLhsT_>, ValueRhsT_>::type(lhs.getImplementation() * rhs);
}
template <typename ValueLhsT_, typename ImplementationRhsT_>
auto operator*(const ValueLhsT_ lhs, const ValuesWrapper<ImplementationRhsT_>& rhs) {
  return rhs * lhs;
}
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
auto operator*(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValuesWrapper<ImplementationRhsT_>& rhs) {
  if constexpr (Dimension<ValuesWrapper<ImplementationLhsT_>>::value == 0 && Dimension<ValuesWrapper<ImplementationRhsT_>>::value == 0) {
    return typename Product<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>>::type(lhs.getImplementation() *
                                                                                                          rhs.getImplementation());
  } else if constexpr (Dimension<ValuesWrapper<ImplementationLhsT_>>::value == 0) {
    return rhs * lhs;
  } else if constexpr (Dimension<ValuesWrapper<ImplementationRhsT_>>::value == 0) {
    return
        typename Product<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>>::type(lhs.getImplementation() * rhs.get());
  } else {
    return typename Product<ValuesWrapper<ImplementationLhsT_>, ValuesWrapper<ImplementationRhsT_>>::type(lhs.getImplementation() *
                                                                                                          rhs.getImplementation());
  }
}

template <typename ImplementationLhsT_, typename ValueRhsT_>
auto operator/(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValueRhsT_ rhs) {
  return typename Division<ValuesWrapper<ImplementationLhsT_>, ValueRhsT_>::type(lhs.getImplementation() / rhs);
}

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
bool operator==(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValuesWrapper<ImplementationRhsT_>& rhs) {
  return lhs.getImplementation() == rhs.getImplementation();
}

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
bool operator!=(const ValuesWrapper<ImplementationLhsT_>& lhs, const ValuesWrapper<ImplementationRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <typename ImplementationT_>
std::ostream& operator<<(std::ostream& lhs, const ValuesWrapper<ImplementationT_>& rhs) {
  lhs << rhs.getImplementation();
  return lhs;
}

}  // namespace wuzp
