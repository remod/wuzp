#pragma once

#include <boost/units/absolute.hpp>
#include <boost/units/systems/si.hpp>

#include "ctm/Translation.hpp"
#include "ctm/traits.hpp"

namespace ctm {

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Sum<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>> {
  using type = typename boost::units::add_typeof_helper<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Sum<boost::units::absolute<boost::units::unit<DimLhs, SystemLhs>>, boost::units::unit<DimRhs, SystemRhs>> {
  using type = typename boost::units::add_typeof_helper<boost::units::absolute<boost::units::unit<DimLhs, SystemLhs>>,
                                                        boost::units::unit<DimRhs, SystemRhs>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Sum<boost::units::unit<DimLhs, SystemLhs>, boost::units::absolute<boost::units::unit<DimRhs, SystemRhs>>> {
  using type = typename boost::units::add_typeof_helper<boost::units::unit<DimLhs, SystemLhs>,
                                                        boost::units::absolute<boost::units::unit<DimRhs, SystemRhs>>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Difference<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>> {
  using type =
      typename boost::units::subtract_typeof_helper<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Difference<boost::units::absolute<boost::units::unit<DimLhs, SystemLhs>>, boost::units::unit<DimRhs, SystemRhs>> {
  using type = typename boost::units::subtract_typeof_helper<boost::units::absolute<boost::units::unit<DimLhs, SystemLhs>>,
                                                             boost::units::unit<DimRhs, SystemRhs>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Difference<boost::units::unit<DimLhs, SystemLhs>, boost::units::absolute<boost::units::unit<DimRhs, SystemRhs>>> {
  using type = typename boost::units::subtract_typeof_helper<boost::units::unit<DimLhs, SystemLhs>,
                                                             boost::units::absolute<boost::units::unit<DimRhs, SystemRhs>>>::type;
};

template <typename DimLhs, typename SystemLhs, typename DimRhs, typename SystemRhs>
struct Product<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>> {
  using type =
      typename boost::units::multiply_typeof_helper<boost::units::unit<DimLhs, SystemLhs>, boost::units::unit<DimRhs, SystemRhs>>::type;
};

template <typename Dim, typename System>
struct Inverse<boost::units::unit<Dim, System>> {
  using type = typename boost::units::power_typeof_helper<boost::units::unit<Dim, System>, boost::units::static_rational<-1>>::type;
};

template <typename Dim, typename System>
struct Identity<boost::units::unit<Dim, System>> {
  using type = boost::units::unit<boost::units::dimensionless_type, System>;
};

}  // namespace ctm
