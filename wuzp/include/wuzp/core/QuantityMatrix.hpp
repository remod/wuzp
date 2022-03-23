#pragma once

#include <ostream>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/UnitMatrix.hpp"
#include "wuzp/core/ValueMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename ValueMatrix_, typename UnitMatrix_>
class QuantityMatrix {
 public:
  using ValueMatrixT = ValueMatrix_;
  using UnitMatrixT = UnitMatrix_;
  using ScalarT = typename ValueMatrixT::ScalarT;
  using This = QuantityMatrix<ValueMatrixT, UnitMatrixT>;

  static_assert(Rows<ValueMatrixT>::value == Rows<UnitMatrixT>::value, "Number of rows do not match.");
  static_assert(Cols<ValueMatrixT>::value == Cols<UnitMatrixT>::value, "Number of columns do not match.");

  QuantityMatrix() = default;

  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromQuantityMatrix>) QuantityMatrix(const T0& t0, const Ts&... ts)
      : QuantityMatrix(typename GetTag<This, T0, Ts...>::type(), t0, ts...) {}
  template <typename T0, typename... Ts, typename = std::enable_if_t<!IsSpecialization<T0, Tag>::value>>
  // Allow implicit construction from similar quantity matrices. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!std::is_same_v<typename GetTag<This, T0, Ts...>::type, FromQuantityMatrix>) QuantityMatrix(T0&& t0, Ts&&... ts)
      : QuantityMatrix(typename GetTag<This, T0, Ts...>::type(), std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static This zero() { return This(ValueMatrixT::zero()); }
  static This random() { return This(ValueMatrixT::random()); }

  template <int RowId, int ColId>
  auto getQuantity() const {
    return typename Element<This, RowId, ColId>::type(valueMatrix_.template getValue<RowId, ColId>());
  }

  auto getQuantity(const int rowId, const int colId) const { return typename GetType<This>::type(valueMatrix_.getValue(rowId, colId)); }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity& quantity) {
    static_assert(std::is_same<typename Element<UnitMatrixT, RowId, ColId>::type, typename Quantity::UnitT>::value, "Units do not match.");
    valueMatrix_.template setValue<RowId, ColId>(quantity.getValue());
  }

  template <typename Quantity>
  void setQuantity(const int rowId, const int colId, const Quantity& quantity) {
    static_assert(IsHomogeneous<This>::value && std::is_same<typename Element<UnitMatrixT, 0, 0>::type, typename Quantity::UnitT>::value,
                  "Units do not match.");
    valueMatrix_.setValue(rowId, colId, quantity.getValue());
  }

  /**
   * Set the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @param valueMatrix Value matrix.
   */
  template <typename ValueMatrixOther>
  void setValueMatrix(const ValueMatrixOther& valueMatrix) {
    valueMatrix_ = valueMatrix;
  }

  /**
   * Get the values of the quantity matrix.
   * WARNING: This method circumvents quantity type safety.
   * @return ValueMatrix of the quantity matrix.
   */
  const ValueMatrixT& getValueMatrix() const { return valueMatrix_; }

  template <typename ValueMatrixOtherT, typename UnitMatrixOtherT>
  bool isApprox(const QuantityMatrix<ValueMatrixOtherT, UnitMatrixOtherT>& other,
                const ScalarT precision = Precision<ScalarT>::value()) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOtherT>::value, "Units do not match.");
    return valueMatrix_.isApprox(other.getValueMatrix(), precision);
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        valueMatrix_.template getBlock<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(valueMatrix_.transpose()); }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  auto dotProduct(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& other) const {
    return
        typename DotProduct<This, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>>::type(valueMatrix_.dotProduct(other.getValueMatrix()));
  }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  auto crossProduct(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& other) const {
    return typename CrossProduct<This, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>>::type(
        valueMatrix_.crossProduct(other.getValueMatrix()));
  }

  auto inverse() const { return typename Inverse<This>::type(valueMatrix_.inverse()); }

  //  template <bool IsHomogeneousV = IsHomogeneous<UnitMatrixT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto norm() const {
    static_assert(!IsAbsolute<This>::value, "Quantity matrix is not relative.");
    return typename GetType<This>::type(valueMatrix_.norm());
  }

  //  template <bool IsHomogeneousV = IsHomogeneous<UnitMatrixT>::value, typename = std::enable_if_t<IsHomogeneousV>>
  auto squaredNorm() const {
    static_assert(!IsAbsolute<This>::value, "Quantity matrix is not relative.");
    return typename Power<typename GetType<This>::type, Integer<2>>::type(valueMatrix_.squaredNorm());
  }

 private:
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  QuantityMatrix(FromQuantityMatrix /*tag*/, const QuantityMatrix<ValueMatrixOther, UnitMatrixOther>& other)
      : valueMatrix_(other.getValueMatrix()) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  QuantityMatrix(FromQuantityMatrix /*tag*/, QuantityMatrix<ValueMatrixOther, UnitMatrixOther>&& other)
      : valueMatrix_(std::move(other.getValueMatrix())) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(FromQuantities /*tag*/, const Quantity<typename Scalar<ValueMatrixT>::type, Units>&... quantities)
      : QuantityMatrix(FromValueMatrixArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  explicit QuantityMatrix(FromQuantities /*tag*/, Quantity<typename Scalar<ValueMatrixT>::type, Units>&&... quantities)
      : QuantityMatrix(FromValueMatrixArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  template <typename... Ts>
  explicit QuantityMatrix(FromValueMatrixArgs /*tag*/, const Ts&... ts) : valueMatrix_(ValueMatrixT(ts...)) {}
  template <typename... Ts>
  explicit QuantityMatrix(FromValueMatrixArgs /*tag*/, Ts&&... ts) : valueMatrix_(ValueMatrixT(std::forward<Ts>(ts)...)) {}

  ValueMatrixT valueMatrix_;
};

template <typename ValueMatrix_, typename UnitMatrix_, typename... Ts>
struct GetTag<QuantityMatrix<ValueMatrix_, UnitMatrix_>, Ts...> {
 private:
  template <typename T>
  struct IsQuantity : std::false_type {};
  template <typename Value, typename Unit>
  struct IsQuantity<Quantity<Value, Unit>> : std::true_type {};
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename IfThenElse<
      sizeof...(Ts) == 1,
      typename IfThenElse<
          IsSpecialization<first, QuantityMatrix>::value, FromQuantityMatrix,
          typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantities, FromValueMatrixArgs>::type>::type,
      typename IfThenElse<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantities, FromValueMatrixArgs>::type>::type;
};

template <typename ValueMatrix, typename UnitMatrix>
struct GetType<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = Quantity<typename QuantityMatrix<ValueMatrix, UnitMatrix>::ScalarT, typename GetType<UnitMatrix>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Dimension<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr int value = Dimension<ValueMatrix>::value;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Rows<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr int value = Rows<ValueMatrix>::value;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Cols<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr int value = Cols<ValueMatrix>::value;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename Sum<ValueMatrixLhs, ValueMatrixRhs>::type, typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type =
      QuantityMatrix<typename Difference<ValueMatrixLhs, ValueMatrixRhs>::type, typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename Product<ValueMatrixLhs, ValueRhs>::type, typename Product<UnitMatrixLhs, UnitRhs>::type>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = typename Product<QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type;
};
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename Product<ValueMatrixLhs, ValueMatrixRhs>::type, typename Product<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
struct Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename Division<ValueMatrixLhs, ValueRhs>::type, typename Division<UnitMatrixLhs, UnitRhs>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Transpose<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct DotProduct<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = Quantity<typename DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type, typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
struct CrossProduct<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>> {
  using type = QuantityMatrix<typename CrossProduct<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename CrossProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, int ElementRowId, int ElementColId>
struct Element<QuantityMatrix<ValueMatrix, UnitMatrix>, ElementRowId, ElementColId> {
  using type = Quantity<typename Scalar<ValueMatrix>::type, typename Element<UnitMatrix, ElementRowId, ElementColId>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct Inverse<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename Inverse<ValueMatrix>::type, typename Inverse<UnitMatrix>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<QuantityMatrix<ValueMatrix, UnitMatrix>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename Block<ValueMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type>;
};

template <typename ValueMatrix, typename UnitMatrix>
struct IsAbsolute<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  static constexpr bool value = IsAbsolute<UnitMatrix>::value;
};
template <typename ValueMatrix, typename UnitMatrix>
struct MakeRelative<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<ValueMatrix, typename MakeRelative<UnitMatrix>::type>;
  static auto make(const QuantityMatrix<ValueMatrix, UnitMatrix>& object) { return type(object.getValueMatrix()); }
};
template <typename ValueMatrix, typename UnitMatrix>
struct MakeAbsolute<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<ValueMatrix, typename MakeAbsolute<UnitMatrix>::type>;
  static auto make(const QuantityMatrix<ValueMatrix, UnitMatrix>& object) { return type(object.getValueMatrix()); }
};

template <typename ValueMatrix, typename UnitMatrix>
struct Evaluate<QuantityMatrix<ValueMatrix, UnitMatrix>> {
  using type = QuantityMatrix<typename Evaluate<ValueMatrix>::type, UnitMatrix>;
};

template <typename ValueMatrix, typename UnitMatrix>
auto operator+(const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  return quantityMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator+(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(
      lhs.getValueMatrix() + rhs.getValueMatrix());
}

template <typename ValueMatrix, typename UnitMatrix>
auto operator-(const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  return QuantityMatrix<ValueMatrix, UnitMatrix>(-quantityMatrix.getValueMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator-(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(
      lhs.getValueMatrix() - rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const ValueRhs rhs) {
  return lhs * Quantity<ValueRhs, typename AccessUnits<SpecializationDummy<ValueRhs>>::Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator*(const ValueLhs lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValueMatrix() *
                                                                                                            rhs.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>>::type(
      lhs.getValueMatrix() * rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs>
auto operator/(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<UnitMatrixLhs, 0, 0>::type>::type;
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValueMatrix() /
                                                                                                             rhs.getValue());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
bool operator==(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  static_assert(std::is_same<UnitMatrixLhs, UnitMatrixRhs>::value, "Units do not match.");
  return lhs.getValueMatrix() == rhs.getValueMatrix();
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs>
bool operator!=(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueMatrix, typename UnitMatrix>
std::ostream& operator<<(std::ostream& stream, const QuantityMatrix<ValueMatrix, UnitMatrix>& quantityMatrix) {
  stream << quantityMatrix.getValueMatrix();
  return stream;
}

}  // namespace wuzp
