#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

using AngleQuantity = QuantityD<Angle>;

// TODO(remod): Turn this into a proper function.
TEST(TestRigidBodyKinematics, CalculateVelocity) {  // NOLINT
  using VelocityVector = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using TranslationVector = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using RotationalVelocityVector = AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA>;  // Against inertial system.
  const VelocityVector velocity{1.0, 2.0, 3.0};
  const TranslationVector translation{0.0, 0.0, 1.0};
  const RotationalVelocityVector rotationalVelocity{4.0, 1.0, 2.0};
  const VelocityVector expectedVelocity{2.0, -2.0, 3.0};
  const VelocityVector actualVelocity{velocity + rotationalVelocity.crossProduct(translation)
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                     / AngleQuantity{1.0}
#endif
  };
  EXPECT_EQ(actualVelocity, expectedVelocity);
}

TEST(TestRigidBodyKinematics, CalculateAcceleration) {  // NOLINT
  using AccelerationVector = AccelerationVector3<ValueMatrix, double, CoordinateSystemA>;
  using TranslationVector = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using RotationalVelocityVector = AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA>;          // Against inertial system.
  using RotationalAccelerationVector = AngularAccelerationVector3<ValueMatrix, double, CoordinateSystemA>;  // Against inertial system.
  const AccelerationVector acceleration{1.0, 2.0, 3.0};
  const TranslationVector translation{0.0, 0.0, 1.0};
  const RotationalAccelerationVector rotationalAcceleration{2.0, -1.0, 3.0};
  const RotationalVelocityVector rotationalVelocity{4.0, 1.0, 2.0};
  const AccelerationVector expectedAcceleration{8.0, 2.0, -14.0};
  const AccelerationVector actualAcceleration{acceleration +
                                              rotationalAcceleration.crossProduct(translation)
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                  / AngleQuantity{1.0}
#endif
                                              + rotationalVelocity.crossProduct(rotationalVelocity.crossProduct(translation))
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                    / AngleQuantity{1.0} / AngleQuantity{1.0}
#endif
  };
  EXPECT_EQ(actualAcceleration, expectedAcceleration);
}
