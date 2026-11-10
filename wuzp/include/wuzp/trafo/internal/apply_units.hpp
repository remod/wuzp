#pragma once

#include <tuple>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/helpers.hpp"

namespace wuzp {
namespace internal {

//! TemplateT_ applied to the units of the unit matrix UnitsT_, the way the aliases of the transformations give them: a single
//! unit if they are homogeneous, e.g. Dimensionless for a dimensionless linear transformation, otherwise one unit per element.
//! The result of an operation on transformations, e.g. the product of two dimensionless linear transformations, thus has the
//! type of the alias, rather than that of a transformation with a dimensionless unit per element, which has the same units but
//! is another type.
template <typename UnitsT_, template <typename...> typename TemplateT_>
struct ApplyUnits;
template <int RowsV_, int ColsV_, typename UnitT_, typename... UnitsT_, template <typename...> typename TemplateT_>
struct ApplyUnits<TypeMatrixImpl<RowsV_, ColsV_, std::tuple<UnitT_, UnitsT_...>>, TemplateT_> {
  using type = typename IfThenElse<areSame<UnitT_, UnitsT_...>, TemplateT_<UnitT_>, TemplateT_<UnitT_, UnitsT_...>>::type;
};

}  // namespace internal
}  // namespace wuzp
