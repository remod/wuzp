#pragma once

#include "wuzp/core/traits.hpp"

namespace wuzp {

// TODO(remod): Move to internal namespace (linker error?)
template <typename QuantityT, typename ValueT>
void setQuantityValue(QuantityT& quantity, const ValueT& value);

// TODO(remod): Move to internal namespace (linker error?)
template <typename ValueT, typename QuantityT>
ValueT getQuantityValue(const QuantityT& quantity);

// TODO(remod): Introduce scaling factor (Ratio).
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

}  // namespace wuzp
