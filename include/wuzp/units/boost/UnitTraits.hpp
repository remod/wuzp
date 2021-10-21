#pragma once

#include <boost/units/dimensionless_type.hpp>
#include <boost/units/operators.hpp>
#include <boost/units/static_rational.hpp>
#include <boost/units/unit.hpp>

#include "wuzp/core/traits.hpp"
#include "wuzp/units/boost/Unit.hpp"

namespace wuzp {

struct UnitTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    using type = typename boost::units::add_typeof_helper<Lhs, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    using type = typename boost::units::subtract_typeof_helper<Lhs, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename Lhs, typename Rhs>
  struct Product<Lhs, Rhs> {
    using type = typename boost::units::multiply_typeof_helper<Lhs, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <typename Lhs, typename Rhs>
  struct Division<Lhs, Rhs> {
    using type = typename boost::units::divide_typeof_helper<Lhs, Rhs>::type;
  };

  template <typename Unit, int Exponent>
  struct Power {
    using type = typename boost::units::power_typeof_helper<Unit, boost::units::static_rational<Exponent>>::type;
  };

  template <typename Unit>
  struct Identity;
  template <typename Dim, typename System>
  struct Identity<boost::units::unit<Dim, System>> {
    using type = boost::units::unit<boost::units::dimensionless_type, System>;
  };
  template <typename Dim, typename System>
  struct Identity<boost::units::absolute<boost::units::unit<Dim, System>>> {
    using type = boost::units::absolute<boost::units::unit<boost::units::dimensionless_type, System>>;
  };

  template <typename Unit>
  struct Inverse {
    using type = typename boost::units::power_typeof_helper<Unit, boost::units::static_rational<-1>>::type;
  };

  template <typename Unit>
  struct IsAbsolute : public std::false_type {};
  template <typename Unit>
  struct IsAbsolute<Absolute<Unit>> : public std::true_type {};

  template <typename Unit>
  struct MakeRelative {
    using type = Unit;
  };
  template <typename Unit>
  struct MakeRelative<Absolute<Unit>> {
    using type = Unit;
  };
  template <typename Unit>
  struct MakeAbsolute {
    using type = Absolute<Unit>;
  };
  template <typename Unit>
  struct MakeAbsolute<Absolute<Unit>> {
    static_assert(FakeDependency<MakeAbsolute<Absolute<Unit>>>::value, "Unit is already absolute.");
  };
};

}  // namespace wuzp
