#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/LorentzTransformation.hpp"

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

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

struct CoordinateSystemAName {};
template <int DimensionV, typename ScalarT, typename CoordinateSystemT>
auto getInvariant2(const SpacetimeVector<DimensionV, ValueMatrix, ScalarT, CoordinateSystemT>& vector) {
  const auto quantityMatrix = vector.getQuantityMatrix();
  return pow<2>(quantityMatrix.template getQuantity<0, 0>()) - quantityMatrix.template block<1, 0, DimensionV - 1, 1>().squaredNorm();
};

TEST(TestLorentzTransformation, TransformSpacetimePoint) {  // NOLINT
  struct CoordinateSystemBName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
  using Object = SpacetimeVector4<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = LorentzTransformation4<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = SpacetimeVector4<ValueMatrix, double, CoordinateSystemB>;
  const Object object(0.5 * getSpeedOfLight<double>() * Quantity<double, Time>(0.0), Quantity<double, Length>(0.0),
                      Quantity<double, Length>(0.0), Quantity<double, Length>(0.0));
  const TransformedObject transformedObject(0.0, 1.0, 2.0, 3.0);
  using Velocity3 = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation =
      createBoostMatrix<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 4>(Velocity3(0.1 * getSpeedOfLight<double>(), 0.2 * getSpeedOfLight<double>(), 0.3* getSpeedOfLight<double>()));
  std::cerr << transformation << std::endl;
  EXPECT_EQ(transformation.transform(object), transformedObject);
  const auto x = getInvariant<4, ValueMatrix, double, CoordinateSystemA>(object);
  const auto y = getInvariant2(object);
//  EXPECT_EQ(getInvariant(object), getInvariant(transformation.transform(object)));
}
