#pragma once

#include <cmath>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

// TODO(remod): Move to internal namespace (linker error?)
template <typename QuantityT, typename ValueT>
void setQuantityValue(QuantityT& quantity, const ValueT& value);

// TODO(remod): Move to internal namespace (linker error?)
template <typename ValueT, typename QuantityT>
ValueT getQuantityValue(const QuantityT& quantity);

// TODO(remod): Just typedef QuantityMatrix<1,1> instead?
//  Contra:
//  - We will not be able to use boost::Quantity anymore, but for matrices we already can't... (at least in the implementation, the
//    interface does)
//  - Users which need the library only for scalar operations will depend on a matrix library.
//  Pro: Less typedefs, as we do not need to treat the combination of scalars and matrices
template <typename ValueT_, typename UnitT_>
class Quantity {
 public:
  using ValueT = ValueT_;
  using UnitT = UnitT_;
  using ThisT = Quantity<ValueT, UnitT>;
  using ImplementationT = typename GetQuantityImplementation<ValueT, UnitT>::template type<ValueT, typename UnitT::ImplementationT>;

  Quantity() = default;
  explicit Quantity(const ImplementationT& implementation) : implementation_(implementation) {}
  explicit Quantity(ImplementationT&& implementation) : implementation_(std::move(implementation)) {}
  explicit Quantity(const ValueT& value) { setValue(value); }

  ThisT& setValue(const ValueT& value) {
    setQuantityValue(*this, value);
    return *this;
  }
  ValueT getValue() const { return getQuantityValue<ValueT>(*this); }

  auto inverse() const { return pow<-1>(*this); }

  ImplementationT getImplementation() const { return implementation_; }
  void setImplementation(const ImplementationT& implementation) { implementation_ = implementation; }

 private:
  ImplementationT implementation_;
};

template <typename ValueT_, typename UnitT_, typename ExponentT>
struct Power<Quantity<ValueT_, UnitT_>, ExponentT> {
  using type = Quantity<ValueT_, typename Power<UnitT_, ExponentT>::type>;
};

// Exponent first, as value and unit can be derived automatically.
template <typename ExponentT, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  return typename Power<Quantity<Value, Unit>, ExponentT>::type(std::pow(quantity.getValue(), ExponentT::template value<Value>()));
}

// Convenience definition, so one can write pow<2>() to get pow<Integer<2>>().
template <int ExponentV, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  return pow<Integer<ExponentV>>(quantity);
}

// Convenience definition, so one can write pow<1, 2>() to get pow<Rational<1, 2>>().
template <int ExponentDividendV, int ExponentDivisorV, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  return pow<Rational<ExponentDividendV, ExponentDivisorV>>(quantity);
}

template <typename Value>
auto sin(const Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Angle>& angle) {
  return Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Dimensionless>(std::sin(angle.getValue()));
}

template <typename Value>
auto cos(const Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Angle>& angle) {
  return Quantity<Value, typename AccessUnits<SpecializationDummy<Value>>::Dimensionless>(std::cos(angle.getValue()));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
