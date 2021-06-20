#pragma once

#include <boost/units/dimensionless_type.hpp>
#include <boost/units/operators.hpp>
#include <boost/units/static_rational.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/unit.hpp>

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"

namespace ctm {

struct UnitTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    using type = typename ::boost::units::add_typeof_helper<Lhs, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    using type = typename ::boost::units::subtract_typeof_helper<Lhs, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename Lhs, typename Rhs>
  struct Product<Lhs, Rhs> {
    using type = typename ::boost::units::multiply_typeof_helper<Lhs, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <typename Lhs, typename Rhs>
  struct Division<Lhs, Rhs> {
    using type = typename ::boost::units::divide_typeof_helper<Lhs, Rhs>::type;
  };

  template <typename Unit>
  struct Inverse {
    using type = typename ::boost::units::power_typeof_helper<Unit, ::boost::units::static_rational<-1>>::type;
  };

  template <typename Unit>
  struct Identity;
  template <typename Dim, typename System>
  struct Identity<::boost::units::unit<Dim, System>> {
    using type = typename ::boost::units::unit<::boost::units::dimensionless_type, System>;
  };
};

template <int Rows, int Cols, typename... Units>
using UnitMatrix = TypeMatrix<Rows, Cols, UnitTraits, Units...>;

}  // namespace ctm
