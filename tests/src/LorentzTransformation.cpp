#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/LorentzTransformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

using TimeQuantityD = QuantityD<Time>;
using LengthQuantityD = QuantityD<Length>;
using AreaQuantityD = QuantityD<Area>;

using LorentzBoost = LorentzTransformation4<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
using Velocity3 = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
using SpacetimeVectorA = SpacetimeVector4<ValueMatrix, double, CoordinateSystemA>;
using SpacetimeVectorB = SpacetimeVector4<ValueMatrix, double, CoordinateSystemB>;

LorentzBoost createBoost(const Velocity3& velocity) {
  return createBoostMatrix<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 4>(velocity);
}

TEST(TestLorentzTransformation, TransformSpacetimeVector) {  // NOLINT
  const SpacetimeVectorA object{getSpeedOfLight<double>() * TimeQuantityD{0.0}, LengthQuantityD{1.0e1}, LengthQuantityD{1.0e2},
                                LengthQuantityD{1.0e3}};
  const SpacetimeVectorB transformedObject{-346.143, 27.9594, 135.919, 1053.88};
  const double c = getSpeedOfLight<double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{0.1 * c, 0.2 * c, 0.3 * c})};
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-5));
}

TEST(TestLorentzTransformation, BoostWithZeroVelocityIsIdentity) {  // NOLINT
  const LorentzBoost transformation{createBoost(Velocity3{0.0, 0.0, 0.0})};
  const LorentzBoost identity{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  EXPECT_TRUE(transformation.getTensor().isApprox(identity.getTensor()));
}

TEST(TestLorentzTransformation, BoostWithSmallVelocity) {  // NOLINT
  // For small velocities, the boost is the identity plus first order terms: x0' = x0 - v x / c, x' = x - v x0 / c.
  const double c = getSpeedOfLight<double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{1.0e-6 * c, 0.0, 0.0})};
  const SpacetimeVectorA object{1.0, 2.0, 3.0, 4.0};
  const SpacetimeVectorB transformedObject{1.0 - 2.0e-6, 2.0 - 1.0e-6, 3.0, 4.0};
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-9));
}

TEST(TestLorentzTransformation, Invariant) {  // NOLINT
  // x0 = c * 1 us = 299.792458 m: x0^2 - x1^2 - x2^2 - x3^2 = 89875.51787 m^2 - 1010100 m^2.
  const SpacetimeVectorA object{getSpeedOfLight<double>() * TimeQuantityD{1.0e-6}, LengthQuantityD{1.0e1}, LengthQuantityD{1.0e2},
                                LengthQuantityD{1.0e3}};
  const auto invariant = getInvariant<4, ValueMatrix, double, CoordinateSystemA>(object);
  EXPECT_TRUE(invariant.isApprox(AreaQuantityD{-920224.48212}, 1e-9));
}

TEST(TestLorentzTransformation, InvariantIsPreserved) {  // NOLINT
  const SpacetimeVectorA object{getSpeedOfLight<double>() * TimeQuantityD{1.0e-6}, LengthQuantityD{1.0e1}, LengthQuantityD{1.0e2},
                                LengthQuantityD{1.0e3}};
  const double c = getSpeedOfLight<double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{0.1 * c, 0.2 * c, 0.3 * c})};
  const auto invariant = getInvariant<4, ValueMatrix, double, CoordinateSystemA>(object);
  const auto transformedInvariant = getInvariant<4, ValueMatrix, double, CoordinateSystemB>(transformation.transform(object));
  EXPECT_TRUE(invariant.isApprox(transformedInvariant, 1e-9));
}
