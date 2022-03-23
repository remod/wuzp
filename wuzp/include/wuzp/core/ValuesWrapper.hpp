#pragma once

namespace wuzp {

/**
 * Wrapper for the actual implementation of a value container.
 * This is required as some libraries, e.g. Eigen, use different types for matrices and expressions, but we want a single trait
 * specialization to catch all of them. We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam Implementation_ ImplementationT of the value matrix.
 */
template <typename Implementation_>
class ValuesWrapper {
 public:
  using ImplementationT = Implementation_;
  using ThisT = ValuesWrapper<ImplementationT>;
  // TODO(remod): Use member types instead of traits for value, dim, rows, cols, in all classes?
  using ValueT = typename Value<ThisT>::type;

  explicit ValuesWrapper(const ImplementationT& implementation) : implementation_(implementation) {}
  // TODO(remod): Is move here correct? Check all constructors.
  explicit ValuesWrapper(ImplementationT&& implementation) : implementation_(std::move(implementation)) {}

  ValuesWrapper() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromValues>) ValuesWrapper(const T0& t0, const Ts&... ts)
      : ValuesWrapper(typename GetTag<ThisT, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromValues>) ValuesWrapper(T0&& t0, Ts&&... ts)
      : ValuesWrapper(typename GetTag<ThisT, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return Zero<ThisT>::value(); }
  static ThisT random() { return Random<ThisT>::value(); }

  template <int RowId, int ColId, typename ValueT>
  ThisT& setValue(ValueT value) {
    implementation_(RowId, ColId) = value;
    return *this;
  }

  template <int RowId, int ColId>
  auto getValue() const {
    return implementation_(RowId, ColId);
  }

  template <typename ValueT>
  ThisT& setValue(int rowId, int colId, ValueT value) {
    implementation_(rowId, colId) = value;
    return *this;
  }

  auto getValue(int rowId, int colId) const { return implementation_(rowId, colId); }

  template <int Dimension = Dimension<ThisT>::value, typename = std::enable_if_t<Dimension == 0>>
  operator typename Value<ThisT>::type() const {
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
    return typename DotProduct<ThisT, ValuesWrapper<ImplementationOther_>>::type(implementation_.dot(other.getImplementation()));
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
  ValuesWrapper(FromValues /*tag*/, const ValuesWrapper<ImplementationOther_>& other) : implementation_(other.getImplementation()) {}
  template <typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  ValuesWrapper(FromValues /*tag*/, ValuesWrapper<ImplementationOther_>&& other) : implementation_(std::move(other.getImplementation())) {}

  template <typename T0>
  ValuesWrapper(FromValuesArgs /*tag*/, T0&& t0) : implementation_(std::forward<T0>(t0)) {}
  template <typename T0, typename... Ts>
  ValuesWrapper(FromValuesArgs /*tag*/, T0&& t0, Ts&&... ts) {
    initialize(*this, t0, ts...);
  }

  ImplementationT implementation_;
};

template <typename... Xs, typename... Ts>
struct GetTag<ValuesWrapper<Xs...>, Ts...> {
 private:
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<sizeof...(Ts) == 1,
                                   typename IfThenElse<IsSpecialization<first, ValuesWrapper>::value, FromValues, FromValuesArgs>::type,
                                   FromValuesArgs>::type;
};

template <typename Implementation_>
struct Value<ValuesWrapper<Implementation_>> {
  using type = typename std::decay_t<Implementation_>::Scalar;
};

template <typename Implementation_>
struct Rows<ValuesWrapper<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::RowsAtCompileTime;
};

template <typename Implementation_>
struct Cols<ValuesWrapper<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::ColsAtCompileTime;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Sum<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>() + std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Difference<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>() - std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Product<ValuesWrapper<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>() * std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Product<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>() * std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Division<ValuesWrapper<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>() / std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Division<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>() / std::declval<ImplementationRhs_>())>;
};

template <typename Implementation_>
struct Transpose<ValuesWrapper<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<const Implementation_>().transpose())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct DotProduct<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<ImplementationLhs_>().dot(std::declval<ImplementationRhs_>()));
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct CrossProduct<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<ImplementationLhs_>().cross(std::declval<ImplementationRhs_>()))>;
};

template <typename Implementation_>
struct Inverse<ValuesWrapper<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<Implementation_>().inverse())>;
};

// template <typename Implementation_, int ElementRowId, int ElementColId>
// struct Element<ValuesWrapper<Implementation_>, ElementRowId, ElementColId> {
//  static_assert(ElementRowId >= 0 && ElementRowId < Rows<ValuesWrapper<Implementation_>>::value, "Element row id is out
//  of range."); static_assert(ElementColId >= 0 && ElementColId < Cols<ValuesWrapper<Implementation_>>::value, "Element
//  columns id is out of range."); using type = typename std::tuple_element<ElementRowId *
//  Cols<ValuesWrapper<Implementation_>>::value + ElementColId, std::tuple<Types...>>::type;
//};

template <typename Implementation_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<ValuesWrapper<Implementation_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValuesWrapper<decltype(std::declval<const Implementation_>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId))>;
};

template <typename Implementation_>
struct Zero<ValuesWrapper<Implementation_>> {
  static auto value() { return ValuesWrapper(Implementation_::Zero()); }
};

template <typename Implementation_>
struct Random<ValuesWrapper<Implementation_>> {
  static auto value() { return ValuesWrapper(Implementation_::Random()); }
};

template <typename Implementation_>
struct Evaluate<ValuesWrapper<Implementation_>> {
  using type = ValuesWrapper<std::decay_t<decltype(std::declval<Implementation_>().eval())>>;
};
template <typename Implementation_>
static auto evaluate(ValuesWrapper<Implementation_> t) {
  return typename Evaluate<ValuesWrapper<Implementation_>>::type(t.getImplementation());
}

template <typename Implementation_, typename... Args>
void initialize(ValuesWrapper<Implementation_>& values, Args&&... args);

template <typename Implementation_>
auto operator+(const ValuesWrapper<Implementation_>& rhs) {
  return rhs;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator+(const ValuesWrapper<ImplementationLhs_>& lhs, const ValuesWrapper<ImplementationRhs_>& rhs) {
  return typename Sum<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>>::type(lhs.getImplementation() +
                                                                                                  rhs.getImplementation());
}

template <typename Implementation_>
auto operator-(const ValuesWrapper<Implementation_>& rhs) {
  return ValuesWrapper<Implementation_>(-rhs.getImplementation());
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator-(const ValuesWrapper<ImplementationLhs_>& lhs, const ValuesWrapper<ImplementationRhs_>& rhs) {
  return typename Difference<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>>::type(lhs.getImplementation() -
                                                                                                         rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhsT_>
auto operator*(const ValuesWrapper<ImplementationLhs_>& lhs, const ValueRhsT_ rhs) {
  return typename Product<ValuesWrapper<ImplementationLhs_>, ValueRhsT_>::type(lhs.getImplementation() * rhs);
}
template <typename ValueLhsT_, typename ImplementationRhs_>
auto operator*(const ValueLhsT_ lhs, const ValuesWrapper<ImplementationRhs_>& rhs) {
  return rhs * lhs;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator*(const ValuesWrapper<ImplementationLhs_>& lhs, const ValuesWrapper<ImplementationRhs_>& rhs) {
  return typename Product<ValuesWrapper<ImplementationLhs_>, ValuesWrapper<ImplementationRhs_>>::type(lhs.getImplementation() *
                                                                                                      rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhsT_>
auto operator/(const ValuesWrapper<ImplementationLhs_>& lhs, const ValueRhsT_ rhs) {
  return typename Division<ValuesWrapper<ImplementationLhs_>, ValueRhsT_>::type(lhs.getImplementation() / rhs);
}

template <typename ImplementationLhs_, typename ImplementationRhs_>
bool operator==(const ValuesWrapper<ImplementationLhs_>& lhs, const ValuesWrapper<ImplementationRhs_>& rhs) {
  return lhs.getImplementation() == rhs.getImplementation();
}

template <typename ImplementationLhs_, typename ImplementationRhs_>
bool operator!=(const ValuesWrapper<ImplementationLhs_>& lhs, const ValuesWrapper<ImplementationRhs_>& rhs) {
  return !(lhs == rhs);
}

template <typename Implementation_>
std::ostream& operator<<(std::ostream& lhs, const ValuesWrapper<Implementation_>& rhs) {
  lhs << rhs.getImplementation();
  return lhs;
}

}  // namespace wuzp
