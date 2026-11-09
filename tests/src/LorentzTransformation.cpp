#include <stdexcept>

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
  const SpacetimeVectorA object{getSpeedOfLight<ValueMatrix, double>() * TimeQuantityD{0.0}, LengthQuantityD{1.0e1}, LengthQuantityD{1.0e2},
                                LengthQuantityD{1.0e3}};
  const SpacetimeVectorB transformedObject{-346.143, 27.9594, 135.919, 1053.88};
  const double c = getSpeedOfLight<ValueMatrix, double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{0.1 * c, 0.2 * c, 0.3 * c})};
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-5));
}

TEST(TestLorentzTransformation, BoostWithZeroVelocityIsIdentity) {  // NOLINT
  const LorentzBoost transformation{createBoost(Velocity3{0.0, 0.0, 0.0})};
  const LorentzBoost identity{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  EXPECT_TRUE(transformation.getTensor().isApprox(identity.getTensor(), 1e-12));
}

TEST(TestLorentzTransformation, BoostWithSmallVelocity) {  // NOLINT
  // For small velocities, the boost is the identity plus first order terms: x0' = x0 - v x / c, x' = x - v x0 / c.
  const double c = getSpeedOfLight<ValueMatrix, double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{1.0e-6 * c, 0.0, 0.0})};
  const SpacetimeVectorA object{1.0, 2.0, 3.0, 4.0};
  const SpacetimeVectorB transformedObject{1.0 - 2.0e-6, 2.0 - 1.0e-6, 3.0, 4.0};
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-9));
}

TEST(TestLorentzTransformation, Invariant) {  // NOLINT
  // x0 = c * 1 us = 299.792458 m: x0^2 - x1^2 - x2^2 - x3^2 = 89875.51787 m^2 - 1010100 m^2.
  const SpacetimeVectorA object{getSpeedOfLight<ValueMatrix, double>() * TimeQuantityD{1.0e-6}, LengthQuantityD{1.0e1},
                                LengthQuantityD{1.0e2}, LengthQuantityD{1.0e3}};
  const auto invariant = getInvariant(object);
  EXPECT_TRUE(invariant.isApprox(AreaQuantityD{-920224.48212}, 1e-9));
}

TEST(TestLorentzTransformation, InvariantOfFourVelocity) {  // NOLINT
  // The four-velocity gamma (c, v) of any velocity v has the invariant c^2; for v = 0.6 c, gamma = 1.25.
  const auto c = getSpeedOfLight<ValueMatrix, double>();
  const VelocityVector<4, ValueMatrix, double, CoordinateSystemA> fourVelocity{c * 1.25, c * 0.75, c * 0.0, c * 0.0};
  EXPECT_TRUE(getInvariant(fourVelocity).isApprox(pow<2>(c), 1e-12));
}

TEST(TestLorentzTransformation, InvariantIsPreserved) {  // NOLINT
  const SpacetimeVectorA object{getSpeedOfLight<ValueMatrix, double>() * TimeQuantityD{1.0e-6}, LengthQuantityD{1.0e1},
                                LengthQuantityD{1.0e2}, LengthQuantityD{1.0e3}};
  const double c = getSpeedOfLight<ValueMatrix, double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{0.1 * c, 0.2 * c, 0.3 * c})};
  const auto invariant = getInvariant(object);
  const auto transformedInvariant = getInvariant(transformation.transform(object));
  EXPECT_TRUE(invariant.isApprox(transformedInvariant, 1e-9));
}

// A boost needs a velocity below the speed of light: the Lorentz factor is infinite at the speed of light and not real above
// it (#67).
TEST(TestLorentzTransformation, BoostNeedsVelocityBelowSpeedOfLight) {  // NOLINT
  const double c = getSpeedOfLight<ValueMatrix, double>().getValue();
  EXPECT_THROW(createBoost(Velocity3{c, 0.0, 0.0}), std::invalid_argument);
  EXPECT_THROW(createBoost(Velocity3{0.0, 2.0 * c, 0.0}), std::invalid_argument);
  EXPECT_NO_THROW(createBoost(Velocity3{0.0, 0.0, 0.99 * c}));
}

// The velocity is the one of To relative to From, expressed in From: an event at the origin of To, which moves with v along x
// in From, has the spatial coordinates 0 in To.
TEST(TestLorentzTransformation, VelocityOfToRelativeToFrom) {  // NOLINT
  const double c = getSpeedOfLight<ValueMatrix, double>().getValue();
  const LorentzBoost transformation{createBoost(Velocity3{0.6 * c, 0.0, 0.0})};
  // At t = 1 s, the origin of To is at x = 0.6 c * 1 s in From, and its proper time is 1 s / gamma = 0.8 s, with gamma = 1.25.
  const SpacetimeVectorA event{getSpeedOfLight<ValueMatrix, double>() * TimeQuantityD{1.0},
                               getSpeedOfLight<ValueMatrix, double>() * TimeQuantityD{0.6}, LengthQuantityD{0.0}, LengthQuantityD{0.0}};
  const SpacetimeVectorB transformedEvent{getSpeedOfLight<ValueMatrix, double>() * TimeQuantityD{0.8}, LengthQuantityD{0.0},
                                          LengthQuantityD{0.0}, LengthQuantityD{0.0}};
  EXPECT_TRUE(transformation.transform(event).isApprox(transformedEvent, 1e-12));
}
