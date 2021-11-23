#pragma once

#include <ostream>
#include <type_traits>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename ValueMatrix_, typename UnitMatrix_, template <typename, typename> typename QuantityT_>
class QuantityMatrix {
 public:
  using ValueMatrixT = ValueMatrix_;
  using UnitMatrixT = UnitMatrix_;
  template <typename ValueT_, typename UnitT_>
  using QuantityT = QuantityT_<ValueT_, UnitT_>;
  using This = QuantityMatrix<ValueMatrixT, UnitMatrixT, QuantityT>;

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

  ~QuantityMatrix() = default;

  static This zero() { return This(ValueMatrixT::zero()); }
  static This random() { return This(ValueMatrixT::random()); }

  template <int RowId, int ColId, typename Quantity>
  void setQuantity(const Quantity quantity) {
    static_assert(std::is_same<typename Element<UnitMatrixT, RowId, ColId>::type, typename Quantity::UnitT>::value, "Units do not match.");
    valueMatrix_.template setValue<RowId, ColId>(quantity.getValue());
  }
  template <int RowId, int ColId>
  auto getQuantity() const {
    return QuantityT<typename Scalar<ValueMatrixT>::type, typename Element<UnitMatrixT, RowId, ColId>::type>
           (valueMatrix_.template getValue<RowId, ColId>());
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
  bool isApprox(const QuantityMatrix<ValueMatrixOtherT, UnitMatrixOtherT, QuantityT>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOtherT>::value, "Units do not match.");
    return valueMatrix_.isApprox(other.getValueMatrix());
  }

  template <int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  auto block() const {
    return typename Block<This, BlockRowId, BlockColId, BlockRows, BlockCols>::type(
        valueMatrix_.template getBlock<BlockRowId, BlockColId, BlockRows, BlockCols>());
  }

  auto transpose() const { return typename Transpose<This>::type(valueMatrix_.transpose()); }

  template <typename ValueMatrixOther, typename UnitMatrixOther>
  auto dot(const QuantityMatrix<ValueMatrixOther, UnitMatrixOther, QuantityT>& other) const {
    return typename DotProduct<This, QuantityMatrix<ValueMatrixOther, UnitMatrixOther, QuantityT>>::type(
        valueMatrix_.dot(other.getValueMatrix()));
  }

  auto inverse() const { return typename Inverse<This>::type(valueMatrix_.inverse()); }

 private:
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  QuantityMatrix(FromQuantityMatrix /*tag*/, const QuantityMatrix<ValueMatrixOther, UnitMatrixOther, QuantityT>& other)
      : valueMatrix_(other.getValueMatrix()) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }
  template <typename ValueMatrixOther, typename UnitMatrixOther>
  // Allow other value matrix types to support lazy evaluation.
  QuantityMatrix(FromQuantityMatrix /*tag*/, QuantityMatrix<ValueMatrixOther, UnitMatrixOther, QuantityT>&& other)
      : valueMatrix_(std::move(other.getValueMatrix())) {
    static_assert(std::is_same<UnitMatrixT, UnitMatrixOther>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  QuantityMatrix(FromQuantities /*tag*/, const QuantityT<typename Scalar<ValueMatrixT>::type, Units>&... quantities)
      : QuantityMatrix(FromValueMatrixArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  /**
   * Construct a quantity matrix from quantities.
   * @tparam Units Units of the quantities, order and types must correspond to the units of the quantity matrix.
   * @param quantities Quantities.
   */
  template <typename... Units>
  QuantityMatrix(FromQuantities /*tag*/, QuantityT<typename Scalar<ValueMatrixT>::type, Units>&&... quantities)
      : QuantityMatrix(FromValueMatrixArgs(), quantities.getValue()...) {
    static_assert(std::is_same<std::tuple<Units...>, typename GetTypes<UnitMatrixT>::type>::value, "Units do not match.");
  }

  template <typename... Ts>
  QuantityMatrix(FromValueMatrixArgs /*tag*/, const Ts&... ts) : valueMatrix_(ValueMatrixT(ts...)) {}
  template <typename... Ts>
  QuantityMatrix(FromValueMatrixArgs /*tag*/, Ts&&... ts) : valueMatrix_(ValueMatrixT(std::forward<Ts>(ts)...)) {}

  ValueMatrixT valueMatrix_;
};

template <typename ValueMatrix_, typename UnitMatrix_, template <typename, typename> typename QuantityT_, typename... Ts>
struct GetTag<QuantityMatrix<ValueMatrix_, UnitMatrix_, QuantityT_>, Ts...> {
 private:
  template <typename T>
  struct IsQuantity : std::false_type {};
  template <typename Value, typename Unit>
  struct IsQuantity<QuantityT_<Value, Unit>> : std::true_type {};
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;

 public:
  using type = typename Chose<
      sizeof...(Ts) == 1,
      typename Chose<IsSpecializationQuantityMatrix<first, QuantityMatrix>::value, FromQuantityMatrix,
                     typename Chose<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantities, FromValueMatrixArgs>::type>::type,
      typename Chose<(IsQuantity<std::decay_t<Ts>>::value && ...), FromQuantities, FromValueMatrixArgs>::type>::type;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct Dimension<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  static constexpr int value = Dimension<ValueMatrix>::value;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct Rows<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  static constexpr int value = Rows<ValueMatrix>::value;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct Cols<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  static constexpr int value = Cols<ValueMatrix>::value;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
struct Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>> {
  using type =
      QuantityMatrix<typename Sum<ValueMatrixLhs, ValueMatrixRhs>::type, typename Sum<UnitMatrixLhs, UnitMatrixRhs>::type, QuantityT_>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
struct Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>> {
  using type = QuantityMatrix<typename Difference<ValueMatrixLhs, ValueMatrixRhs>::type,
                              typename Difference<UnitMatrixLhs, UnitMatrixRhs>::type, QuantityT_>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs,
          template <typename, typename> typename QuantityT_>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityT_<ValueRhs, UnitRhs>> {
  using type = QuantityMatrix<typename Product<ValueMatrixLhs, ValueRhs>::type, typename Product<UnitMatrixLhs, UnitRhs>::type, QuantityT_>;
};
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
struct Product<QuantityT_<ValueLhs, UnitLhs>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>> {
  using type = typename Product<QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>, QuantityT_<ValueLhs, UnitLhs>>::type;
};
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
struct Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>> {
  using type = QuantityMatrix<typename Product<ValueMatrixLhs, ValueMatrixRhs>::type, typename Product<UnitMatrixLhs, UnitMatrixRhs>::type,
                              QuantityT_>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs,
          template <typename, typename> typename QuantityT_>
struct Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityT_<ValueRhs, UnitRhs>> {
  using type =
      QuantityMatrix<typename Division<ValueMatrixLhs, ValueRhs>::type, typename Division<UnitMatrixLhs, UnitRhs>::type, QuantityT_>;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct Transpose<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  using type = QuantityMatrix<typename Transpose<ValueMatrix>::type, typename Transpose<UnitMatrix>::type, QuantityT_>;
};

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
struct DotProduct<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>> {
  using type =
      QuantityT_<typename DotProduct<ValueMatrixLhs, ValueMatrixRhs>::type, typename DotProduct<UnitMatrixLhs, UnitMatrixRhs>::type>;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct Inverse<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  using type = QuantityMatrix<typename Inverse<ValueMatrix>::type, typename Inverse<UnitMatrix>::type, QuantityT_>;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_, int BlockRowId, int BlockColId,
          int BlockRows, int BlockCols>
struct Block<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = QuantityMatrix<typename Block<ValueMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type,
                              typename Block<UnitMatrix, BlockRowId, BlockColId, BlockRows, BlockCols>::type, QuantityT_>;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct IsAbsolute<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  static constexpr bool value = IsAbsolute<UnitMatrix>::value;
};
template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct MakeRelative<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  using type = QuantityMatrix<ValueMatrix, typename MakeRelative<UnitMatrix>::type, QuantityT_>;
  static auto make(const QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>& object) { return type(object.getValueMatrix()); }
};
template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct MakeAbsolute<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  using type = QuantityMatrix<ValueMatrix, typename MakeAbsolute<UnitMatrix>::type, QuantityT_>;
  static auto make(const QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>& object) { return type(object.getValueMatrix()); }
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
struct Evaluate<QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>> {
  using type = QuantityMatrix<typename Evaluate<ValueMatrix>::type, UnitMatrix, QuantityT_>;
};

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
auto operator+(const QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>& quantityMatrix) {
  return quantityMatrix;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
auto operator+(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs,
               const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  return typename Sum<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>,
                      QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>>::type(lhs.getValueMatrix() + rhs.getValueMatrix());
}

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
auto operator-(const QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>& quantityMatrix) {
  return QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>(-quantityMatrix.getValueMatrix());
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
auto operator-(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs,
               const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  return typename Difference<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>,
                             QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>>::type(lhs.getValueMatrix() - rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, template <typename, typename> typename QuantityT_, typename ValueRhs>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<UnitMatrixLhs, 0, 0>::type>::type;
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename ValueMatrixRhs, typename UnitMatrixRhs, template <typename, typename> typename QuantityT_>
auto operator*(const ValueLhs lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs,
          template <typename, typename> typename QuantityT_>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs, const QuantityT_<ValueRhs, UnitRhs>& rhs) {
  return typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityT_<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() * rhs.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
auto operator*(const QuantityT_<ValueLhs, UnitLhs>& lhs, const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  return rhs * lhs;
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
auto operator*(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs,
               const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  return typename Product<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>,
                          QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>>::type(lhs.getValueMatrix() * rhs.getValueMatrix());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, template <typename, typename> typename QuantityT_, typename ValueRhs>
auto operator/(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<UnitMatrixLhs, 0, 0>::type>::type;
  return lhs / QuantityT_<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueRhs, typename UnitRhs,
          template <typename, typename> typename QuantityT_>
auto operator/(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs, const QuantityT_<ValueRhs, UnitRhs>& rhs) {
  return typename Division<QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>, QuantityT_<ValueRhs, UnitRhs>>::type(
      lhs.getValueMatrix() / rhs.getValue());
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
bool operator==(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs,
                const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  static_assert(std::is_same<UnitMatrixLhs, UnitMatrixRhs>::value, "Units do not match.");
  return lhs.getValueMatrix() == rhs.getValueMatrix();
}

template <typename ValueMatrixLhs, typename UnitMatrixLhs, typename ValueMatrixRhs, typename UnitMatrixRhs,
          template <typename, typename> typename QuantityT_>
bool operator!=(const QuantityMatrix<ValueMatrixLhs, UnitMatrixLhs, QuantityT_>& lhs,
                const QuantityMatrix<ValueMatrixRhs, UnitMatrixRhs, QuantityT_>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueMatrix, typename UnitMatrix, template <typename, typename> typename QuantityT_>
std::ostream& operator<<(std::ostream& stream, const QuantityMatrix<ValueMatrix, UnitMatrix, QuantityT_>& quantityMatrix) {
  stream << quantityMatrix.getValueMatrix();
  return stream;
}

}  // namespace wuzp
