#pragma once

#include <type_traits>
#include <utility>

#include <Eigen/Core>
#include <Eigen/LU>

namespace wuzp_eigen {
namespace internal {

template <typename T0, typename... Ts>
struct CollapseCommas {
  using type = T0;
};
template <typename T0, typename T1, typename... Ts>
struct CollapseCommas<T0, T1, Ts...> {
  using type = decltype(std::declval<T0>(), std::declval<typename CollapseCommas<T1, Ts...>::type>());
};

template <typename Lhs>
Lhs&& spliceCommas(Lhs&& lhs) {
  return std::forward<Lhs>(lhs);
}
template <typename Lhs, typename Rhs, typename... Tail>
typename CollapseCommas<Lhs, Rhs, Tail...>::type&& spliceCommas(Lhs&& lhs, Rhs&& rhs, Tail&&... tail) {
  auto&& firstTwo = (std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  return spliceCommas(std::forward<decltype(firstTwo)>(firstTwo), std::forward<Tail>(tail)...);
}

}  // namespace internal

// TODO: How can we enforce the same interface for all value matrices?
/**
 * Wrapper for all eigen matrices, used to standardize the interface amongst all value matrix implementations.
 * @tparam Implementation_ Implementation of the value matrix.
 */
template <typename Implementation_>
class GenericValueMatrix {
 public:
  using Implementation = Implementation_;
  using This = GenericValueMatrix<Implementation>;
  // TODO(remod): Use member types instead of traits for scalar, dim, rows, cols, in all classes?
  using Scalar = typename wuzp::Scalar<This>::type;

  explicit GenericValueMatrix(const Implementation& implementation) : implementation_(implementation) {}
  explicit GenericValueMatrix(Implementation&& implementation) : implementation_(std::move(implementation)) {}

  GenericValueMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!wuzp::IsSpecialization<T0, wuzp::Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename wuzp::GetTag<This, T0, Ts...>::type, wuzp::FromValueMatrix>)
      GenericValueMatrix(const T0& t0, const Ts&... ts)
      : GenericValueMatrix(typename wuzp::GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!wuzp::IsSpecialization<T0, wuzp::Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename wuzp::GetTag<This, T0, Ts...>::type, wuzp::FromValueMatrix>) GenericValueMatrix(T0&& t0, Ts&&... ts)
      : GenericValueMatrix(typename wuzp::GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static This zero() { return wuzp::Zero<This>::value(); }
  static This random() { return wuzp::Random<This>::value(); }

  template <int RowId, int ColId, typename ValueT>
  This& setValue(ValueT value) {
    implementation_(RowId, ColId) = value;
    return *this;
  }
  template <int RowId, int ColId>
  auto getValue() const {
    return implementation_(RowId, ColId);
  }

  template <int Dimension = wuzp::Dimension<This>::value, typename = std::enable_if_t<Dimension == 0>>
  operator typename wuzp::Scalar<This>::type() const {
    return implementation_(0, 0);
  }

  template <typename ImplementationOther_>
  bool isApprox(const GenericValueMatrix<ImplementationOther_>& other) const {
    return implementation_.isApprox(other.getImplementation());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto getBlock() const {
    return typename wuzp::Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        implementation_.template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  }

  auto transpose() const { return typename wuzp::Transpose<This>::type(implementation_.transpose()); }

  template <typename ImplementationOther_>
  auto dot(const GenericValueMatrix<ImplementationOther_>& other) const {
    return typename wuzp::DotProduct<This, GenericValueMatrix<ImplementationOther_>>::type(implementation_.dot(other.getImplementation()));
  }

  auto inverse() const { return typename wuzp::Inverse<This>::type(implementation_.inverse()); }

  const Implementation& getImplementation() const { return implementation_; }

 private:
  template <typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  GenericValueMatrix(wuzp::FromValueMatrix /*tag*/, const GenericValueMatrix<ImplementationOther_>& other)
      : implementation_(other.getImplementation()) {}
  template <typename ImplementationOther_>
  // Allow other implementation types to support lazy evaluation.
  GenericValueMatrix(wuzp::FromValueMatrix /*tag*/, GenericValueMatrix<ImplementationOther_>&& other)
      : implementation_(std::move(other.getImplementation())) {}

  template <typename T0>
  GenericValueMatrix(wuzp::FromValueMatrixArgs /*tag*/, T0&& t0) : implementation_(std::forward<T0>(t0)) {}
  template <typename T0, typename... Ts>
  GenericValueMatrix(wuzp::FromValueMatrixArgs /*tag*/, T0&& t0, Ts&&... ts) {
    internal::spliceCommas((implementation_ << std::forward<T0>(t0)), std::forward<Ts>(ts)...);
  }

  Implementation implementation_;
};

template <typename ValueT, int Rows, int Cols>
using ValueMatrix = GenericValueMatrix<Eigen::Matrix<ValueT, Rows, Cols>>;

}  // namespace wuzp_eigen

namespace wuzp {

template <typename... Xs, typename... Ts>
struct GetTag<wuzp_eigen::GenericValueMatrix<Xs...>, Ts...> {
 private:
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename Choose<
      sizeof...(Ts) == 1,
      typename Choose<IsSpecialization<first, wuzp_eigen::GenericValueMatrix>::value, FromValueMatrix, FromValueMatrixArgs>::type,
      FromValueMatrixArgs>::type;
};

template <typename Implementation_>
struct Scalar<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  using type = typename std::decay_t<Implementation_>::Scalar;
};

template <typename Implementation_>
struct Rows<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::RowsAtCompileTime;
};

template <typename Implementation_>
struct Cols<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  static constexpr int value = std::decay_t<Implementation_>::ColsAtCompileTime;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Sum<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, wuzp_eigen::GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      wuzp_eigen::GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() + std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Difference<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, wuzp_eigen::GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      wuzp_eigen::GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() - std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Product<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = wuzp_eigen::GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() * std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Product<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, wuzp_eigen::GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      wuzp_eigen::GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() * std::declval<ImplementationRhs_>())>;
};

template <typename ImplementationLhs_, typename ValueRhs_>
struct Division<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, ValueRhs_> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = wuzp_eigen::GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() / std::declval<ValueRhs_>())>;
};
template <typename ImplementationLhs_, typename ImplementationRhs_>
struct Division<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, wuzp_eigen::GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      wuzp_eigen::GenericValueMatrix<decltype(std::declval<ImplementationLhs_>() / std::declval<ImplementationRhs_>())>;
};

template <typename Implementation_>
struct Transpose<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = wuzp_eigen::GenericValueMatrix<decltype(std::declval<const Implementation_>().transpose())>;
};

template <typename ImplementationLhs_, typename ImplementationRhs_>
struct DotProduct<wuzp_eigen::GenericValueMatrix<ImplementationLhs_>, wuzp_eigen::GenericValueMatrix<ImplementationRhs_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<ImplementationLhs_>().dot(std::declval<ImplementationRhs_>()));
};

template <typename Implementation_>
struct Inverse<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = wuzp_eigen::GenericValueMatrix<decltype(std::declval<Implementation_>().inverse())>;
};

// template <typename Implementation_, int ElementRowId, int ElementColId>
// struct Element<wuzp_eigen::GenericValueMatrix<Implementation_>, ElementRowId, ElementColId> {
//  static_assert(ElementRowId >= 0 && ElementRowId < Rows<wuzp_eigen::GenericValueMatrix<Implementation_>>::value, "Element row id is out
//  of range."); static_assert(ElementColId >= 0 && ElementColId < Cols<wuzp_eigen::GenericValueMatrix<Implementation_>>::value, "Element
//  columns id is out of range."); using type = typename std::tuple_element<ElementRowId *
//  Cols<wuzp_eigen::GenericValueMatrix<Implementation_>>::value + ElementColId, std::tuple<Types...>>::type;
//};

template <typename Implementation_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<wuzp_eigen::GenericValueMatrix<Implementation_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = wuzp_eigen::GenericValueMatrix<decltype(
      std::declval<const Implementation_>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId))>;
};

template <typename Implementation_>
struct Zero<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  static auto value() { return wuzp_eigen::GenericValueMatrix(Implementation_::Zero()); }
};

template <typename Implementation_>
struct Random<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  static auto value() { return wuzp_eigen::GenericValueMatrix(Implementation_::Random()); }
};

template <typename Implementation_>
struct Evaluate<wuzp_eigen::GenericValueMatrix<Implementation_>> {
  using type = wuzp_eigen::GenericValueMatrix<std::decay_t<decltype(std::declval<Implementation_>().eval())>>;
};
template <typename Implementation_>
static auto evaluate(wuzp_eigen::GenericValueMatrix<Implementation_> t) {
  return typename Evaluate<wuzp_eigen::GenericValueMatrix<Implementation_>>::type(t.getImplementation());
}

}  // namespace wuzp

namespace wuzp_eigen {

// Operators must be defined in the same namespace as the objects.

// TODO(remod): Can we use CRTP and only specify these methods once for the base class?

template <typename Implementation_>
auto operator+(const GenericValueMatrix<Implementation_>& valueMatrix) {
  return valueMatrix;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator+(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return typename wuzp::Sum<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>>::type(lhs.getImplementation() +
                                                                                                                  rhs.getImplementation());
}

template <typename Implementation_>
auto operator-(const GenericValueMatrix<Implementation_>& valueMatrix) {
  return GenericValueMatrix<Implementation_>(-valueMatrix.getImplementation());
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator-(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return typename wuzp::Difference<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>>::type(
      lhs.getImplementation() - rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhs>
auto operator*(const GenericValueMatrix<ImplementationLhs_>& lhs, const ValueRhs rhs) {
  return typename wuzp::Product<GenericValueMatrix<ImplementationLhs_>, ValueRhs>::type(lhs.getImplementation() * rhs);
}
template <typename ValueLhs, typename ImplementationRhs_>
auto operator*(const ValueLhs lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return rhs * lhs;
}
template <typename ImplementationLhs_, typename ImplementationRhs_>
auto operator*(const GenericValueMatrix<ImplementationLhs_>& lhs, const GenericValueMatrix<ImplementationRhs_>& rhs) {
  return typename wuzp::Product<GenericValueMatrix<ImplementationLhs_>, GenericValueMatrix<ImplementationRhs_>>::type(
      lhs.getImplementation() * rhs.getImplementation());
}

template <typename ImplementationLhs_, typename ValueRhs>
auto operator/(const GenericValueMatrix<ImplementationLhs_>& lhs, const ValueRhs rhs) {
  return typename wuzp::Division<GenericValueMatrix<ImplementationLhs_>, ValueRhs>::type(lhs.getImplementation() / rhs);
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

}  // namespace wuzp_eigen
