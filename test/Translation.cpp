#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Translation.hpp"

using namespace wuzp;  // NOLINT

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp/value_matrix/eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

TEST(TestTranslation, TransformPositionVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<ValueMatrix, double, FromCoordinateSystem>;
  using Transformation = SpatialTranslation3D<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionVector3D<ValueMatrix, double, ToCoordinateSystem>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestTranslation, TransformPositionPoint) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionPoint3D<ValueMatrix, double, FromCoordinateSystem>;
  using Transformation = SpatialTranslation3D<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionPoint3D<ValueMatrix, double, ToCoordinateSystem>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(5.0, 4.0, 4.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
