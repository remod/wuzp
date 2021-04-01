#pragma once

#include <boost/units/systems/si.hpp>

#include "ctm/traits.hpp"

namespace ctm {

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Sum<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>> {
  using type = typename boost::units::add_typeof_helper<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Product<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>> {
  using type =
      typename boost::units::multiply_typeof_helper<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>>::type;
};

}  // namespace ctm
