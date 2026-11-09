#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/internal/operator_traits.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_eigen {

template <typename ValueT_, int RowsV_, int ColsV_>
using ValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<RowsV_, ColsV_>, ValueT_, Eigen::Array<ValueT_, RowsV_, ColsV_>>;

template <int RowsV_, int ColsV_>
using ValueMatrixI = ValueMatrix<int, RowsV_, ColsV_>;
template <int RowsV_, int ColsV_>
using ValueMatrixF = ValueMatrix<float, RowsV_, ColsV_>;
template <int RowsV_, int ColsV_>
using ValueMatrixD = ValueMatrix<double, RowsV_, ColsV_>;

template <typename ValueT_, int RowsV_>
using ValueVector = ValueMatrix<ValueT_, RowsV_, 1>;

template <int RowsV_>
using ValueVectorI = ValueVector<int, RowsV_>;
template <int RowsV_>
using ValueVectorF = ValueVector<float, RowsV_>;
template <int RowsV_>
using ValueVectorD = ValueVector<double, RowsV_>;

template <typename ValueT_>
using Value = ValueMatrix<ValueT_, 1, 1>;

using ValueI = Value<int>;
using ValueF = Value<float>;
using ValueD = Value<double>;

//! A quantity with a value of this backend, e.g. QuantityD<wuzp_units::Length>.
template <typename UnitT_>
using QuantityI = wuzp::Quantity<ValueI, UnitT_>;
template <typename UnitT_>
using QuantityF = wuzp::Quantity<ValueF, UnitT_>;
template <typename UnitT_>
using QuantityD = wuzp::Quantity<ValueD, UnitT_>;

//! The implementation types of the backend: Eigen's arrays and the expressions of arrays, which all derive from Eigen::ArrayBase
//! (see https://eigen.tuxfamily.org/dox/TopicClassHierarchy.html). The traits of the backend are only specialized for them, so
//! that other value matrix backends can be used in the same program.
template <typename T>
concept EigenArray = std::is_base_of_v<Eigen::ArrayBase<T>, T>;

}  // namespace wuzp_eigen

namespace wuzp {

template <wuzp_eigen::EigenArray ImplementationT_, int... SizesV_>
struct ReserveMemory<ImplementationT_, MatrixSize<SizesV_...>> {
  static void run(ImplementationT_& /*impl*/) {}
};

template <wuzp_eigen::EigenArray ImplementationT_, typename Arg0T_, typename... ArgsT_>
struct Initialize<ImplementationT_, std::tuple<Arg0T_, ArgsT_...>> {
  static void run(ImplementationT_& impl, Arg0T_&& arg0, ArgsT_&&... args) {
    applyCommas((impl << std::forward<Arg0T_>(arg0)), std::forward<ArgsT_>(args)...);
  }
};

// The evaluated arrays have Eigen's default storage order, whatever the storage order of the expression, e.g. of a transpose,
// so that results have one type: Eigen::Array<ValueT_, RowsV_, ColsV_>, which stores matrices column by column and row vectors
// row by row (see https://eigen.tuxfamily.org/dox/group__TopicStorageOrders.html).
template <wuzp_eigen::EigenArray ImplementationT_, typename ValueT_, int RowsV_, int ColsV_>
struct Rebind<AccessImplementation<ImplementationT_>, ValueT_, RowsV_, ColsV_> {
  using type = Eigen::Array<ValueT_, RowsV_, ColsV_>;
};

// Eigen accesses elements with the parenthesis operator (see "Accessing values inside an Array" in
// https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html).
template <wuzp_eigen::EigenArray ImplementationT_>
struct Coefficient<AccessImplementation<ImplementationT_>> {
  static auto get(const ImplementationT_& impl, const int row, const int col) { return impl(row, col); }
  static void set(ImplementationT_& impl, const int row, const int col, const typename ImplementationT_::Scalar value) {
    impl(row, col) = value;
  }
};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct Comparison<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static bool equal(const ImplementationLhsT_& lhs, const ImplementationRhsT_& rhs) { return lhs.matrix() == rhs.matrix(); }
  static bool approxEqual(const ImplementationLhsT_& lhs, const ImplementationRhsT_& rhs,
                          const typename ImplementationLhsT_::Scalar tolerance) {
    return lhs.matrix().isApprox(rhs.matrix(), tolerance);
  }
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Transpose<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.transpose(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct Sum<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorSum<ImplementationLhsT_, ImplementationRhsT_> {};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct Difference<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorDifference<ImplementationLhsT_, ImplementationRhsT_> {};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Negation<AccessImplementation<ImplementationT_>> : internal::OperatorNegation<ImplementationT_> {};

template <wuzp_eigen::EigenArray ImplementationLhsT_, ArithmeticValue ValueRhsT_>
struct Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_> : internal::OperatorProduct<ImplementationLhsT_, ValueRhsT_> {};
template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct Product<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorProduct<ImplementationLhsT_, ImplementationRhsT_> {};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct TensorProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static_assert(
      FailOnInstantiation<TensorProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>>::value,
      "Tensor product is not implemented for eigen.");
};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct InnerProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    return (implLhs.matrix() * implRhs.matrix()).array();
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct OuterProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    if constexpr (ImplementationLhsT_::RowsAtCompileTime == 1 && ImplementationLhsT_::ColsAtCompileTime == 1) {
      return implLhs(0, 0) * implRhs;
    } else if constexpr (ImplementationRhsT_::RowsAtCompileTime == 1 && ImplementationRhsT_::ColsAtCompileTime == 1) {
      return implLhs * implRhs(0, 0);
    } else if constexpr (ImplementationLhsT_::RowsAtCompileTime == 1) {
      return (implRhs.matrix() * implLhs.matrix()).array();
    } else {
      return (implLhs.matrix() * implRhs.matrix()).array();
    }
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct CrossProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    // Evaluate expression to avoid returning a temporary object.
    return (implLhs.matrix().cross(implRhs.matrix())).array().eval();
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <wuzp_eigen::EigenArray ImplementationLhsT_, wuzp_eigen::EigenArray ImplementationRhsT_>
struct Division<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorDivision<ImplementationLhsT_, ImplementationRhsT_> {};
template <wuzp_eigen::EigenArray ImplementationLhsT_, ArithmeticValue ValueRhsT_>
struct Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_> : internal::OperatorDivision<ImplementationLhsT_, ValueRhsT_> {};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Norm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.matrix().norm(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct SquaredNorm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.matrix().squaredNorm(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Determinant<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.matrix().determinant(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Inverse<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.inverse(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct TensorInverse<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.matrix().inverse().array(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationT_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<AccessImplementation<ImplementationT_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  static auto value(const ImplementationT_& impl) { return impl.template block<BlockRowsV_, BlockColsV_>(BlockRowIdV_, BlockColIdV_); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Zero<AccessImplementation<ImplementationT_>> {
  static void run(ImplementationT_& impl) { impl.setZero(); }
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Random<AccessImplementation<ImplementationT_>> {
  // Uniformly distributed in [-1, 1].
  static void run(ImplementationT_& impl) { impl.setRandom(); }
};

// Eigen's matrices have value semantics: their copies do not share the values. Copies of expressions refer to the same
// operands, like the expressions themselves.
template <wuzp_eigen::EigenArray ImplementationT_>
struct Copy<AccessImplementation<ImplementationT_>> {
  static ImplementationT_ value(const ImplementationT_& impl) { return impl; }
};

template <wuzp_eigen::EigenArray ImplementationT_>
struct Move<AccessImplementation<ImplementationT_>> {
  static ImplementationT_ construct(ImplementationT_& source) { return std::move(source); }
  static void assign(ImplementationT_& target, ImplementationT_& source) { target = std::move(source); }
};

// Evaluates an array or an expression into an array with the default storage order (see Rebind). Eigen's eval() keeps the
// storage order of the expression instead, e.g. the transpose of a column-major matrix is row-major.
template <wuzp_eigen::EigenArray ImplementationT_>
struct Evaluate<AccessImplementation<ImplementationT_>> {
  using type = typename Rebind<AccessImplementation<ImplementationT_>, typename ImplementationT_::Scalar,
                               ImplementationT_::RowsAtCompileTime, ImplementationT_::ColsAtCompileTime>::type;
  static type value(const ImplementationT_& impl) { return type(impl); }
};

}  // namespace wuzp
