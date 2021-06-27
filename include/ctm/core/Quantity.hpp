#pragma once

#include <ostream>

#include "ctm/units/UnitTraits.hpp"

namespace ctm {

template <typename Value, typename Unit>
class Quantity {
 public:
  Quantity() = default;
  explicit Quantity(Value value) : value_(value) {}

  void setValue(Value value) { value_ = value; }
  Value getValue() const { return value_; }

  template <typename TargetValue>
  Quantity<TargetValue, Unit> cast() const {
    return Quantity<TargetValue, Unit>(value_);
  }

  template <typename ValueOther, typename UnitOther>
  bool isApprox(const Quantity<ValueOther, UnitOther>& other) const {
    return value_.isApprox(other.getValue());
  }

  auto inverse() const { return typename Inverse<This>::type(value_.inverse()); }

 private:
  using This = Quantity<Value, Unit>;

  Value value_;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = Quantity<typename ValueSum<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Sum<UnitLhs, UnitRhs>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      Quantity<typename ValueDifference<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Difference<UnitLhs, UnitRhs>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      Quantity<typename ValueProduct<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Product<UnitLhs, UnitRhs>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type =
      Quantity<typename ValueDivision<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Division<UnitLhs, UnitRhs>::type>;
};

template <typename Value, typename Unit>
struct Inverse<Quantity<Value, Unit>> {
  using type = Quantity<typename Inverse<Value>::type, typename Inverse<Unit>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator+(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() + rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator-(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() - rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() * rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
bool operator==(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  static_assert(std::is_same<UnitLhs, UnitRhs>::value, "Unit mismatch.");
  return lhs.getValue() == rhs.getValue();
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
bool operator!=(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename Value, typename Unit>
std::ostream& operator<<(std::ostream& stream, const Quantity<Value, Unit>& quantity) {
  stream << quantity.getValue();
  return stream;
}

}  // namespace ctm
