#pragma once

#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename QuantityT, typename ValueT>
void setQuantityValue(QuantityT& quantity, const ValueT& value);

template <typename ValueT, typename QuantityT>
ValueT getQuantityValue(const QuantityT& quantity);

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

  void setValue(const ValueT& value) { setQuantityValue(*this, value); }
  ValueT getValue() const { return getQuantityValue<ValueT>(*this); }

  const ImplementationT& getImplementation() const { return implementation_; }
  void setImplementation(const ImplementationT& implementation) { implementation_ = implementation; }

 private:
  ImplementationT implementation_;
};

}  // namespace wuzp
