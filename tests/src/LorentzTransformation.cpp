#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/LorentzTransformation.hpp"

#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Quantity.hpp"
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Quantity.hpp"
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;

TEST(TestLorentzTransformation, TransformSpacetimePoint) {  // NOLINT
  using Object = SpacetimeVector4<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = LorentzTransformation4<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = SpacetimeVector4<ValueMatrix, double, CoordinateSystemB>;
  const Object object(0.5 * getSpeedOfLight<double>() * Quantity<double, Time>(0.0), Quantity<double, Length>(1.0e1),
                      Quantity<double, Length>(1.0e2), Quantity<double, Length>(1.0e3));
  const TransformedObject transformedObject(-346.143, 27.9594, 135.919, 1053.88);
  using Velocity3 = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation = createBoostMatrix<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 4>(
      Velocity3(0.1 * getSpeedOfLight<double>(), 0.2 * getSpeedOfLight<double>(), 0.3 * getSpeedOfLight<double>()));
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-3));
  EXPECT_DOUBLE_EQ((getInvariant<4, ValueMatrix, double, CoordinateSystemA>(object).getValue()),
                   (getInvariant<4, ValueMatrix, double, CoordinateSystemB>(transformation.transform(object))).getValue());
}

#endif
