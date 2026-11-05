#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestPassiveProperRigidTransformation2, TransformPosition) {  // NOLINT
  //  const Position2<double, FromCoordinateSystem> object{1.0, 2.0};
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2<double, CoordinateSystemB> translation{1.0, 2.0};
  //  const PassiveProperRigidTransformation2<FromCoordinateSystem, CoordinateSystemB, double> transformation{rotation, translation};
  //  const result = transformation.transform{object};
}

TEST(TestPassiveProperRigidTransformation3, TransformVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPosition) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -3.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 3.0, 6.0, -3.0, -6.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPassiveTranslation) {  // NOLINT
  using Object = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD, double>;
  using TransformedObject = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestPassiveProperRigidTransformation3, TransformPassiveRotation) {  // NOLINT
  using Object = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD, double>;
  using TransformedObject = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  const TransformedObject transformedObject{0.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
#endif

TEST(TestPassiveProperRigidTransformation, TranslationIsExpressedInTo) {  // NOLINT
  // The rotation transforms into an intermediate coordinate system with the axes of To, so the translation is expressed in To.
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  EXPECT_TRUE((std::is_same_v<GetCoordinateSystem<Transformation::Translation>::type, CoordinateSystemB>));
}
