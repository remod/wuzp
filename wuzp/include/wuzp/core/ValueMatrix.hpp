#pragma once

namespace wuzp {

/**
 * Wrapper for all eigen matrices, used to standardize the interface amongst all value matrix implementations.
 * We cannot use CRTP, as we need this wrapper for unambiguous template specialization.
 * @tparam Implementation_ ImplementationT of the value matrix.
 */
template <typename Implementation_>
class GenericValueMatrix {
 public:
  using ImplementationT = Implementation_;
  using ThisT = GenericValueMatrix<ImplementationT>;
  // TODO(remod): Use member types instead of traits for scalar, dim, rows, cols, in all classes?
  using ScalarT = typename Scalar<ThisT>::type;

  explicit GenericValueMatrix(const ImplementationT& implementation) : implementation_(implementation) {}
  // TODO(remod): Is move here correct? Check all constructors.
  explicit GenericValueMatrix(ImplementationT&& implementation) : implementation_(std::move(implementation)) {}

  GenericValueMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromValueMatrix>) GenericValueMatrix(const T0& t0, const Ts&... ts)
      : GenericValueMatrix(typename GetTag<ThisT, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<ThisT, T0, Ts...>::type, FromValueMatrix>) GenericValueMatrix(T0&& t0, Ts&&... ts)
      : GenericValueMatrix(typename GetTag<ThisT, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

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
  bool isApprox(const GenericValueMatrix<ImplementationOther_>& other, const ScalarT precision = Precision<ScalarT>::value()) const {
    return implementation_.isApprox(other.getImplementation(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto getBlock() const {
    return typename Block<ThisT, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        implementation_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename Transpose<ThisT>::type(implementation_.transpose()); }

  template <typename ImplementationOther_>
  auto dotProduct(const GenericValueMatrix<ImplementationOther_>& other) const {
    return typename DotProduct<ThisT, GenericValueMatrix<ImplementationOther_>>::type(implementation_.dot(other.getImplementation()));
  }

  template <typename ImplementationOther_>
  auto crossProduct(const GenericValueMatrix<ImplementationOther_>& other) const {
    return typename CrossProduct<ThisT, GenericValueMatrix<ImplementationOther_>>::type(implementation_.cross(other.getImplementation()));
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
  GenericValueMatrix(FromValueMatrix /*tag*/, const GenericValueMatrix<ImplementationOther_>& other)
      : implementation_(other.getImplementation()) {}
  template <typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  GenericValueMatrix(FromValueMatrix /*tag*/, GenericValueMatrix<ImplementationOther_>&& other)
      : implementation_(std::move(other.getImplementation())) {}

  template <typename T0>
  GenericValueMatrix(FromValueMatrixArgs /*tag*/, T0&& t0) : implementation_(std::forward<T0>(t0)) {}
  template <typename T0, typename... Ts>
  GenericValueMatrix(FromValueMatrixArgs /*tag*/, T0&& t0, Ts&&... ts) {
    initialize(*this, t0, ts...);
  }

  ImplementationT implementation_;
};

template <typename... Xs, typename... Ts>
struct GetTag<GenericValueMatrix<Xs...>, Ts...> {
 private:
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<IsSpecialization<first, GenericValueMatrix>::value, FromValueMatrix, FromValueMatrixArgs>::type,
      FromValueMatrixArgs>::type;
};

template <typename Implementation_>
struct Scalar<GenericValueMatrix<Implementation_>> {
  using type = typename std::decay_t<Implementation_>::Scalar;
};

template <typename Implementation_>
struct Rows<GenericValueMatrix<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::RowsAtCompileTime;
};

template <typename Implementation_>
struct Cols<GenericValueMatrix<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::ColsAtCompileTime;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Sum<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() + std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Difference<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() - std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Product<GenericValueMatrix<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() * std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Product<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() * std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Division<GenericValueMatrix<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() / std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Division<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() / std::declval<ImplementationRhs_>())>;
};

template <typename Implementation_>
struct Transpose<GenericValueMatrix<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<const Implementation_>().transpose())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct DotProduct<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<ImplementationLhs_>().dot(std::declval<ImplementationRhs_>()));
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct CrossProduct<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<ImplementationLhs_>().cross(std::declval<ImplementationRhs_>()))>;
};

template <typename Implementation_>
struct Inverse<GenericValueMatrix<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = GenericValueMatrix<decltype(std::declval<Implementation_>().inverse())>;
};

// template <typename Implementation_, int ElementRowId, int ElementColId>
// struct Element<GenericValueMatrix<Implementation_>, ElementRowId, ElementColId> {
//  static_assert(ElementRowId >= 0 && ElementRowId < Rows<GenericValueMatrix<Implementation_>>::value, "Element row id is out
//  of range."); static_assert(ElementColId >= 0 && ElementColId < Cols<GenericValueMatrix<Implementation_>>::value, "Element
//  columns id is out of range."); using type = typename std::tuple_element<ElementRowId *
//  Cols<GenericValueMatrix<Implementation_>>::value + ElementColId, std::tuple<Types...>>::type;
//};

template <typename Implementation_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<GenericValueMatrix<Implementation_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      GenericValueMatrix<decltype(std::declval<const Implementation_>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId))>;
};

template <typename Implementation_>
struct Zero<GenericValueMatrix<Implementation_>> {
  static auto value() { return GenericValueMatrix(Implementation_::Zero()); }
};

template <typename Implementation_>
struct Random<GenericValueMatrix<Implementation_>> {
  static auto value() { return GenericValueMatrix(Implementation_::Random()); }
};

template <typename Implementation_>
struct Evaluate<GenericValueMatrix<Implementation_>> {
  using type = GenericValueMatrix<std::decay_t<decltype(std::declval<Implementation_>().eval())>>;
};
template <typename Implementation_>
static auto evaluate(GenericValueMatrix<Implementation_> t) {
  return typename Evaluate<GenericValueMatrix<Implementation_>>::type(t.getImplementation());
}

// TODO(remod): Can we use CRTP and only specify these methods once for the base class?

template <typename Implementation_, typename... Args>
void initialize(GenericValueMatrix<Implementation_>& valueMatrix, Args&&... args);

template <typename Implementation_>
auto operator+(const GenericValueMatrix<Implementation_>& valueMatrix) {
  return valueMatrix;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator+(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return typename Sum<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>>::type(lhs.getImplementation() +
                                                                                                            rhs.getImplementation());
}

template <typename Implementation_>
auto operator-(const GenericValueMatrix<Implementation_>& valueMatrix) {
  return GenericValueMatrix<Implementation_>(-valueMatrix.getImplementation());
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator-(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return typename Difference<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>>::type(lhs.getImplementation() -
                                                                                                                   rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhs>
auto operator*(const GenericValueMatrix<ImplementationLhs_>& lhs, const ValueRhs rhs) {
  return typename Product<GenericValueMatrix<ImplementationLhs_>, ValueRhs>::type(lhs.getImplementation() * rhs);
}
template <typename ValueLhs, typename ImplementationRhs_>
auto operator*(const ValueLhs lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return rhs * lhs;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator*(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return typename Product<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>>::type(lhs.getImplementation() *
                                                                                                                rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhs>
auto operator/(const GenericValueMatrix<ImplementationLhs_>& lhs, const ValueRhs rhs) {
  return typename Division<GenericValueMatrix<ImplementationLhs_>, ValueRhs>::type(lhs.getImplementation() / rhs);
}

template <typename ImplementationLhs_, typename ImplementationRhs_>
bool operator==(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return lhs.getImplementation() == rhs.getImplementation();
}

template <typename ImplementationLhs_, typename ImplementationRhs_>
bool operator!=(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return !(lhs == rhs);
}

template <typename Implementation_>
std::ostream& operator<<(std::ostream& stream, const GenericValueMatrix<Implementation_>& valueMatrix) {
  stream << valueMatrix.getImplementation();
  return stream;
}

}  // namespace wuzp
