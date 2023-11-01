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

TEST(TestLorentzTransformation, TransformSpacetimePoint) {  // NOLINT
  using Object = SpacetimeVector4<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = LorentzTransformation4<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = SpacetimeVector4<ValueMatrix, double, CoordinateSystemB>;
  const Object object(0.5 * getSpeedOfLight<double>() * TimeQuantityD(0.0), LengthQuantityD(1.0e1), LengthQuantityD(1.0e2),
                      LengthQuantityD(1.0e3));
  const TransformedObject transformedObject(-346.143, 27.9594, 135.919, 1053.88);
  using Velocity3 = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  //  const Transformation transformation = createBoostMatrix<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB,
  //  4>(
  //      Velocity3(0.1 * getSpeedOfLight<double>(), 0.2 * getSpeedOfLight<double>(), 0.3 * getSpeedOfLight<double>()));
  //  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-3));
  //  EXPECT_TRUE((getInvariant<4, ValueMatrix, double, CoordinateSystemA>(object).isApprox(
  //      getInvariant<4, ValueMatrix, double, CoordinateSystemB>(transformation.transform(object)))));
}
