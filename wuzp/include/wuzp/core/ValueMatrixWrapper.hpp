#pragma once

namespace wuzp {

/**
 * Wrapper for all eigen matrices, used to standardize the interface amongst all value matrix implementations.
 * We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam Implementation_ ImplementationT of the value matrix.
 */
template <typename Implementation_>
class ValueMatrixWrapper {
 public:
  using ImplementationT = Implementation_;
  using ThisT = ValueMatrixWrapper<ImplementationT>;
  // TODO(remod): Use member types instead of traits for scalar, dim, rows, cols, in all classes?
  using ScalarT = typename Scalar<ThisT>::type;

  explicit ValueMatrixWrapper(const ImplementationT& implementation) : implementation_(implementation) {}
  // TODO(remod): Is move here correct? Check all constructors.
  explicit ValueMatrixWrapper(ImplementationT&& implementation) : implementation_(std::move(implementation)) {}

  ValueMatrixWrapper() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromValueMatrix>) ValueMatrixWrapper(const T0& t0, const Ts&... ts)
      : ValueMatrixWrapper(typename GetTag<ThisT, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromValueMatrix>) ValueMatrixWrapper(T0&& t0, Ts&&... ts)
      : ValueMatrixWrapper(typename GetTag<ThisT, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

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
  operator typename Scalar<ThisT>::type() const {
    return implementation_(0, 0);
  }

  template <typename ImplementationOther_>
  bool isApprox(const ValueMatrixWrapper<ImplementationOther_>& other, const ScalarT precision = Precision<ScalarT>::value()) const {
    return implementation_.isApprox(other.getImplementation(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto getBlock() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        implementation_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<ThisT>::type(implementation_.transpose()); }

  template <typename ImplementationOther_>
  auto dotProduct(const ValueMatrixWrapper<ImplementationOther_>& other) const {
    return typename DotProduct<ThisT, ValueMatrixWrapper<ImplementationOther_>>::type(implementation_.dot(other.getImplementation()));
  }

  template <typename ImplementationOther_>
  auto crossProduct(const ValueMatrixWrapper<ImplementationOther_>& other) const {
    return typename CrossProduct<ThisT, ValueMatrixWrapper<ImplementationOther_>>::type(implementation_.cross(other.getImplementation()));
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
  ValueMatrixWrapper(FromValueMatrix /*tag*/, const ValueMatrixWrapper<ImplementationOther_>& other)
      : implementation_(other.getImplementation()) {}
  template <typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  ValueMatrixWrapper(FromValueMatrix /*tag*/, ValueMatrixWrapper<ImplementationOther_>&& other)
      : implementation_(std::move(other.getImplementation())) {}

  template <typename T0>
  ValueMatrixWrapper(FromValueMatrixArgs /*tag*/, T0&& t0) : implementation_(std::forward<T0>(t0)) {}
  template <typename T0, typename... Ts>
  ValueMatrixWrapper(FromValueMatrixArgs /*tag*/, T0&& t0, Ts&&... ts) {
    initialize(*this, t0, ts...);
  }

  ImplementationT implementation_;
};

template <typename... Xs, typename... Ts>
struct GetTag<ValueMatrixWrapper<Xs...>, Ts...> {
 private:
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<IsSpecialization<first, ValueMatrixWrapper>::value, FromValueMatrix, FromValueMatrixArgs>::type,
      FromValueMatrixArgs>::type;
};

template <typename Implementation_>
struct Scalar<ValueMatrixWrapper<Implementation_>> {
  using type = typename std::decay_t<Implementation_>::Scalar;
};

template <typename Implementation_>
struct Rows<ValueMatrixWrapper<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::RowsAtCompileTime;
};

template <typename Implementation_>
struct Cols<ValueMatrixWrapper<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::ColsAtCompileTime;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Sum<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>() + std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Difference<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>() - std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Product<ValueMatrixWrapper<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>() * std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Product<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>() * std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Division<ValueMatrixWrapper<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>() / std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Division<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>() / std::declval<ImplementationRhs_>())>;
};

template <typename Implementation_>
struct Transpose<ValueMatrixWrapper<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<const Implementation_>().transpose())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct DotProduct<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<ImplementationLhs_>().dot(std::declval<ImplementationRhs_>()));
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct CrossProduct<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<ImplementationLhs_>().cross(std::declval<ImplementationRhs_>()))>;
};

template <typename Implementation_>
struct Inverse<ValueMatrixWrapper<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = ValueMatrixWrapper<decltype(std::declval<Implementation_>().inverse())>;
};

// template <typename Implementation_, int ElementRowId, int ElementColId>
// struct Element<ValueMatrixWrapper<Implementation_>, ElementRowId, ElementColId> {
//  static_assert(ElementRowId >= 0 && ElementRowId < Rows<ValueMatrixWrapper<Implementation_>>::value, "Element row id is out
//  of range."); static_assert(ElementColId >= 0 && ElementColId < Cols<ValueMatrixWrapper<Implementation_>>::value, "Element
//  columns id is out of range."); using type = typename std::tuple_element<ElementRowId *
//  Cols<ValueMatrixWrapper<Implementation_>>::value + ElementColId, std::tuple<Types...>>::type;
//};

template <typename Implementation_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<ValueMatrixWrapper<Implementation_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      ValueMatrixWrapper<decltype(std::declval<const Implementation_>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId))>;
};

template <typename Implementation_>
struct Zero<ValueMatrixWrapper<Implementation_>> {
  static auto value() { return ValueMatrixWrapper(Implementation_::Zero()); }
};

template <typename Implementation_>
struct Random<ValueMatrixWrapper<Implementation_>> {
  static auto value() { return ValueMatrixWrapper(Implementation_::Random()); }
};

template <typename Implementation_>
struct Evaluate<ValueMatrixWrapper<Implementation_>> {
  using type = ValueMatrixWrapper<std::decay_t<decltype(std::declval<Implementation_>().eval())>>;
};
template <typename Implementation_>
static auto evaluate(ValueMatrixWrapper<Implementation_> t) {
  return typename Evaluate<ValueMatrixWrapper<Implementation_>>::type(t.getImplementation());
}

template <typename Implementation_, typename... Args>
void initialize(ValueMatrixWrapper<Implementation_>& valueMatrix, Args&&... args);

template <typename Implementation_>
auto operator+(const ValueMatrixWrapper<Implementation_>& valueMatrix) {
  return valueMatrix;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator+(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueMatrixWrapper<ImplementationRhs_>& rhs) {
  return typename Sum<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>>::type(lhs.getImplementation() +
                                                                                                            rhs.getImplementation());
}

template <typename Implementation_>
auto operator-(const ValueMatrixWrapper<Implementation_>& valueMatrix) {
  return ValueMatrixWrapper<Implementation_>(-valueMatrix.getImplementation());
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator-(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueMatrixWrapper<ImplementationRhs_>& rhs) {
  return typename Difference<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>>::type(lhs.getImplementation() -
                                                                                                                   rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhs>
auto operator*(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueRhs rhs) {
  return typename Product<ValueMatrixWrapper<ImplementationLhs_>, ValueRhs>::type(lhs.getImplementation() * rhs);
}
template <typename ValueLhs, typename ImplementationRhs_>
auto operator*(const ValueLhs lhs, const ValueMatrixWrapper<ImplementationRhs_>& rhs) {
  return rhs * lhs;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator*(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueMatrixWrapper<ImplementationRhs_>& rhs) {
  return typename Product<ValueMatrixWrapper<ImplementationLhs_>, ValueMatrixWrapper<ImplementationRhs_>>::type(lhs.getImplementation() *
                                                                                                                rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhs>
auto operator/(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueRhs rhs) {
  return typename Division<ValueMatrixWrapper<ImplementationLhs_>, ValueRhs>::type(lhs.getImplementation() / rhs);
}

template <typename ImplementationLhs_, typename ImplementationRhs_>
bool operator==(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueMatrixWrapper<ImplementationRhs_>& rhs) {
  return lhs.getImplementation() == rhs.getImplementation();
}

template <typename ImplementationLhs_, typename ImplementationRhs_>
bool operator!=(const ValueMatrixWrapper<ImplementationLhs_>& lhs, const ValueMatrixWrapper<ImplementationRhs_>& rhs) {
  return !(lhs == rhs);
}

template <typename Implementation_>
std::ostream& operator<<(std::ostream& stream, const ValueMatrixWrapper<Implementation_>& valueMatrix) {
  stream << valueMatrix.getImplementation();
  return stream;
}

}  // namespace wuzp
