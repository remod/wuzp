#pragma once

#include <ostream>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/Unit.hpp"

namespace wuzp {

// TODO(remod): Just typedef CoordinateMatrix<1,1> instead.
/**
 * Class representing a coordinate.
 * A coordinate is a quantity expressed in a coordinate system.
 * @tparam Quantity_         Type of the underlying quantity.
 * @tparam CoordinateSystem_ Coordinate system.
 */
template <typename Quantity_, typename CoordinateSystem_>
class Coordinate {
 public:
  using QuantityT = Quantity_;
  using CoordinateSystemT = CoordinateSystem_;
  using ValueT = typename QuantityT::ValueT;
  using ThisT = Coordinate<QuantityT, CoordinateSystemT>;

  Coordinate() = default;
  explicit Coordinate(const QuantityT& quantity) : quantity_(quantity) {}
  explicit Coordinate(const ValueT& value) : quantity_(value) {}

  ThisT& setQuantity(const QuantityT& quantity) {
    quantity_ = quantity;
    return *this;
  }
  QuantityT getQuantity() const { return quantity_; }

  ThisT& setValue(const ValueT& value) {
    quantity_.setValue(value);
    return *this;
  }
  ValueT getValue() const { return quantity_.getValue(); }

  auto inverse() const { return typename Inverse<ThisT>::type(quantity_.inverse()); }

 private:
  QuantityT quantity_;
};

template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
struct Sum<Coordinate<QuantityLhs, CoordinateSystemLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>> {
  using type = Coordinate<typename Sum<QuantityLhs, QuantityRhs>::type, typename Sum<CoordinateSystemLhs, CoordinateSystemRhs>::type>;
};

template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
struct Difference<Coordinate<QuantityLhs, CoordinateSystemLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>> {
  using type =
      Coordinate<typename Difference<QuantityLhs, QuantityRhs>::type, typename Difference<CoordinateSystemLhs, CoordinateSystemRhs>::type>;
};

template <typename QuantityLhs, typename CoordinateSystemLhs, typename ValueRhs, typename UnitRhs>
struct Product<Coordinate<QuantityLhs, CoordinateSystemLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = Coordinate<typename Product<QuantityLhs, Quantity<ValueRhs, UnitRhs>>::type, CoordinateSystemLhs>;
};
template <typename ValueLhs, typename UnitLhs, typename QuantityRhs, typename CoordinateSystemRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>> {
  using type = typename Product<Coordinate<QuantityRhs, CoordinateSystemRhs>, Quantity<ValueLhs, UnitLhs>>::type;
};
template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
struct Product<Coordinate<QuantityLhs, CoordinateSystemLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>> {
  using type =
      Coordinate<typename Product<QuantityLhs, QuantityRhs>::type, typename Product<CoordinateSystemLhs, CoordinateSystemRhs>::type>;
};

template <typename QuantityLhs, typename CoordinateSystemLhs, typename ValueRhs, typename UnitRhs>
struct Division<Coordinate<QuantityLhs, CoordinateSystemLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = Coordinate<typename Division<QuantityLhs, Quantity<ValueRhs, UnitRhs>>::type, CoordinateSystemLhs>;
};

// template <typename Quantity, typename CoordinateSystem, typename ExponentT>
// struct Power<Coordinate<Quantity, CoordinateSystem>, ExponentT> {
//  using type = Coordinate<typename Power<Quantity, ExponentT>::type, CoordinateSystem>;
//};

template <typename Quantity, typename CoordinateSystem>
struct Identity<Coordinate<Quantity, CoordinateSystem>> {
  static const Coordinate<typename Identity<Quantity>::type, typename Identity<CoordinateSystem>::type> value_{Identity<Quantity>::value_};
};

template <typename Quantity, typename CoordinateSystem>
struct Inverse<Coordinate<Quantity, CoordinateSystem>> {
  using type = Coordinate<typename Inverse<Quantity>::type, CoordinateSystem>;
};

template <typename Quantity, typename CoordinateSystem>
struct GetCoordinateSystem<Coordinate<Quantity, CoordinateSystem>> {
  using type = CoordinateSystem;
};

template <typename Quantity, typename CoordinateSystem>
struct IsAbsolute<Coordinate<Quantity, CoordinateSystem>> {
  static constexpr bool value = IsAbsolute<Quantity>::value;
};
template <typename Quantity, typename CoordinateSystem>
struct MakeRelative<Coordinate<Quantity, CoordinateSystem>> {
  using type = Coordinate<typename MakeRelative<Quantity>::type, CoordinateSystem>;
  static type make(const Coordinate<Quantity, CoordinateSystem>& object) { return type(object.getValue()); }
};
template <typename Quantity, typename CoordinateSystem>
struct MakeAbsolute<Coordinate<Quantity, CoordinateSystem>> {
  using type = Coordinate<typename MakeAbsolute<Quantity>::type, CoordinateSystem>;
  static type make(const Coordinate<Quantity, CoordinateSystem>& object) { return type(object.getValue()); }
};

template <typename Quantity, typename CoordinateSystem, typename NewCoordinateSystem>
struct CastCoordinateSystem<Coordinate<Quantity, CoordinateSystem>, NewCoordinateSystem> {
  using type = Coordinate<Quantity, typename CastCoordinateSystem<CoordinateSystem, NewCoordinateSystem>::type>;
};

template <typename Quantity, typename CoordinateSystem>
auto operator+(const Coordinate<Quantity, CoordinateSystem>& Coordinate) {
  return Coordinate;
}
template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
auto operator+(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  return typename Sum<Coordinate<QuantityLhs, CoordinateSystemLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>>::type(lhs.getQuantity() +
                                                                                                                        rhs.getQuantity());
}

template <typename Quantity, typename CoordinateSystem>
auto operator-(const Coordinate<Quantity, CoordinateSystem>& coordinate) {
  return Coordinate<Quantity, CoordinateSystem>(-coordinate.getQuantity());
}
template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
auto operator-(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  return typename Difference<Coordinate<QuantityLhs, CoordinateSystemLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>>::type(
      lhs.getQuantity() - rhs.getQuantity());
}

template <typename QuantityLhs, typename CoordinateSystemLhs, typename ValueRhs>
auto operator*(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<typename QuantityLhs::UnitT, 0, 0>::type>::type;
  return lhs * Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename ValueLhs, typename QuantityRhs, typename CoordinateSystemRhs>
auto operator*(const ValueLhs lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  return rhs * lhs;
}
template <typename QuantityLhs, typename CoordinateSystemLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<Coordinate<QuantityLhs, CoordinateSystemLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getQuantity() * rhs);
}
template <typename ValueLhs, typename UnitLhs, typename QuantityRhs, typename CoordinateSystemRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  return rhs * lhs;
}
template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
auto operator*(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  return typename Product<Coordinate<QuantityLhs, CoordinateSystemLhs>, Coordinate<QuantityRhs, CoordinateSystemRhs>>::type(
      lhs.getQuantity() * rhs.getQuantity());
}

template <typename QuantityLhs, typename CoordinateSystemLhs, typename ValueRhs>
auto operator/(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const ValueRhs rhs) {
  using Dimensionless = typename Identity<typename Element<typename QuantityLhs::UnitT, 0, 0>::type>::type;
  return lhs / Quantity<ValueRhs, Dimensionless>(rhs);
}
template <typename QuantityLhs, typename CoordinateSystemLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<Coordinate<QuantityLhs, CoordinateSystemLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getQuantity() / rhs);
}

template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
bool operator==(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  static_assert(std::is_same<CoordinateSystemLhs, CoordinateSystemRhs>::value, "Coordinate systems do not match.");
  return lhs.getQuantity() == rhs.getQuantity();
}

template <typename QuantityLhs, typename CoordinateSystemLhs, typename QuantityRhs, typename CoordinateSystemRhs>
bool operator!=(const Coordinate<QuantityLhs, CoordinateSystemLhs>& lhs, const Coordinate<QuantityRhs, CoordinateSystemRhs>& rhs) {
  return !(lhs == rhs);
}

// TODO(remod): Are those valid for coordinates or only for quantities?
//// Exponent first, as value and unit can be derived automatically.
// template <typename ExponentT, typename Quantity, typename CoordinateSystem>
// auto pow(const Coordinate<Quantity, CoordinateSystem>& coordinate) {
//  return typename Power<Coordinate<Quantity, CoordinateSystem>, ExponentT>::type(pow<ExponentT>(coordinate.getQuantity()));
//}
//
//// Convenience definition, so one can write pow<2>() to get pow<Integer<2>>().
// template <int ExponentV, typename Quantity, typename CoordinateSystem>
// auto pow(const Coordinate<Quantity, CoordinateSystem>& coordinate) {
//  return pow<Integer<ExponentV>>(coordinate);
//}
//
//// Convenience definition, so one can write pow<1, 2>() to get pow<Rational<1, 2>>().
// template <int ExponentDividendV, int ExponentDivisorV, typename Quantity, typename CoordinateSystem>
// auto pow(const Coordinate<Quantity, CoordinateSystem>& coordinate) {
//  return pow<Rational<ExponentDividendV, ExponentDivisorV>>(coordinate);
//}

// template <typename Value, typename CoordinateSystem>
// auto sin(const Coordinate<Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Angle>, CoordinateSystem>& angle) {
//  return Coordinate<Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Angle>,
//  CoordinateSystem>(sin(angle.getQuantity()));
//}
//
// template <typename Value, typename CoordinateSystem>
// auto cos(const Coordinate<Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Angle>, CoordinateSystem>& angle) {
//  return Coordinate<Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Angle>,
//  CoordinateSystem>(cos(angle.getQuantity()));
//}

template <typename Quantity, typename CoordinateSystem>
std::ostream& operator<<(std::ostream& stream, const Coordinate<Quantity, CoordinateSystem>& coordinate) {
  stream << coordinate.getQuantity();
  return stream;
}

}  // namespace wuzp
