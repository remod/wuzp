#pragma once

#include "wuzp/core/internal/AbsoluteImpl.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

using Absolute = internal::AbsoluteImpl<true>;
using Relative = internal::AbsoluteImpl<false>;

template <bool ValueLhs_, bool ValueRhs_>
struct Sum<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ || !ValueRhs_, "Two absolute objects cannot be summed.");
  using type = typename IfThenElse<ValueLhs_ || ValueRhs_, Absolute, Relative>::type;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Difference<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!(!ValueLhs_ && ValueRhs_), "An absolute object cannot be subtracted from a relative object.");
  using type = typename IfThenElse<ValueLhs_, typename IfThenElse<ValueRhs_, Relative, Absolute>::type, Relative>::type;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Product<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ && !ValueRhs_, "An absolute object cannot appear in a multiplication.");
  using type = Relative;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Division<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ && !ValueRhs_, "An absolute object cannot appear in a division.");
  using type = Relative;
};

template <bool Value_, typename ExponentT>
struct Power<internal::AbsoluteImpl<Value_>, ExponentT> {
  static_assert(!Value_, "An absolute object cannot be powered.");
  using type = Relative;
};

template <bool Value_>
struct Inverse<internal::AbsoluteImpl<Value_>> {
  using type = internal::AbsoluteImpl<Value_>;
};

template <bool Value_>
struct IsAbsolute<internal::AbsoluteImpl<Value_>> {
  static constexpr bool value = Value_;
};

template <bool Value_>
struct MakeRelative<internal::AbsoluteImpl<Value_>> {
  static_assert(Value_, "Unit is already relative.");
  using type = Relative;
};

template <bool Value_>
struct MakeAbsolute<internal::AbsoluteImpl<Value_>> {
  static_assert(!Value_, "Unit is already absolute.");
  using type = Absolute;
};

}  // namespace wuzp
