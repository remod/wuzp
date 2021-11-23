#pragma once

namespace wuzp {

// Note: We need to specify the value type explicitly as we cannot access nested typedefs on incomplete types.
template <typename DerivedT_, typename ValueT_>
class QuantityInterface {
 public:
  using DerivedT = DerivedT_;
  using ValueT = ValueT_;

  void setValue(const ValueT& value) { derived().setValueImpl(value); }
  ValueT getValue() const { return derived().getValueImpl(); }

 private:
  DerivedT& derived() { return static_cast<DerivedT&>(*this); }
  const DerivedT& derived() const { return static_cast<const DerivedT&>(*this); }
};

}  // namespace wuzp
