#pragma once

#include "wuzp/core/traits.hpp"

namespace wuzp {

template <bool Value_>
struct AbsoluteImpl {
  static constexpr bool value = Value_;
};

using Absolute = AbsoluteImpl<true>;
using Relative = AbsoluteImpl<false>;

template <bool ValueLhs_, bool ValueRhs_>
struct Sum<AbsoluteImpl<ValueLhs_>, AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ || !ValueRhs_, "Two absolute objects cannot be summed.");
  using type = typename IfThenElse<ValueLhs_ || ValueRhs_, Absolute, Relative>::type;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Difference<AbsoluteImpl<ValueLhs_>, AbsoluteImpl<ValueRhs_>> {
  static_assert(!(!ValueLhs_ && ValueRhs_), "An absolute object cannot be subtracted from a relative object.");
  using type = typename IfThenElse<ValueLhs_, typename IfThenElse<ValueRhs_, Relative, Absolute>::type, Relative>::type;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Product<AbsoluteImpl<ValueLhs_>, AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ && !ValueRhs_, "An absolute object cannot appear in a multiplication.");
  using type = Relative;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Division<AbsoluteImpl<ValueLhs_>, AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ && !ValueRhs_, "An absolute object cannot appear in a division.");
  using type = Relative;
};

template <bool Value_, typename ExponentT>
struct Power<AbsoluteImpl<Value_>, ExponentT> {
  static_assert(!Value_, "An absolute object cannot be powered.");
  using type = Relative;
};

template <bool Value_>
struct Inverse<AbsoluteImpl<Value_>> {
  using type = AbsoluteImpl<Value_>;
};

template <bool Value_>
struct IsAbsolute<AbsoluteImpl<Value_>> {
  static constexpr bool value = Value_;
};

template <bool Value_>
struct MakeRelative<AbsoluteImpl<Value_>> {
  static_assert(Value_, "Unit is already relative.");
  using type = Relative;
};

template <bool Value_>
struct MakeAbsolute<AbsoluteImpl<Value_>> {
  static_assert(!Value_, "Unit is already absolute.");
  using type = Absolute;
};

}  // namespace wuzp
