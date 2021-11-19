#include <gtest/gtest.h>

#include <Eigen/Geometry>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ProperRigidTransformation.hpp"

using namespace wuzp;  // NOLINT

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp/value_matrix/eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

TEST(TestProperRigidTransformation2D, TransformPosition) {  // NOLINT
  //  struct FromCoordinateSystemName {};
  //  struct ToCoordinateSystemName {};
  //  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  //  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  //  const Position2D<double, FromCoordinateSystem> object(1.0, 2.0);
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2D<double, ToCoordinateSystem> translation(1.0, 2.0);
  //  const ProperRigidTransformation2D<FromCoordinateSystem, ToCoordinateSystem, double> transformation(rotation, translation);
  //  const auto result = transformation.transform(object);
}

TEST(TestProperRigidTransformation3D, TransformVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<ValueMatrix, double, FromCoordinateSystem>;
  using Transformation = ProperRigidTransformation3D<ValueMatrix, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem, double>;
  using TransformedObject = PositionVector3D<ValueMatrix, double, ToCoordinateSystem>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -2.0, -3.0);
  const Transformation::Rotation rotation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  const Transformation::Translation translation(4.0, 2.0, 1.0);
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestProperRigidTransformation3D, TransformPosition) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionPoint3D<ValueMatrix, double, FromCoordinateSystem>;
  using Transformation = ProperRigidTransformation3D<ValueMatrix, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem, double>;
  using TransformedObject = PositionPoint3D<ValueMatrix, double, ToCoordinateSystem>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(5.0, 0.0, -2.0);
  const Transformation::Rotation rotation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  const Transformation::Translation translation(4.0, 2.0, 1.0);
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
