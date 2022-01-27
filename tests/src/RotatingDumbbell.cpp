#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"

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

// From https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell
struct CartesianCoordinateSystemName {};
using CartesianCoordinateSystem = CoordinateSystem<CartesianCoordinateSystemName>;
struct MinimalCoordinateSystemName {};
using MinimalCoordinateSystem = CoordinateSystem<MinimalCoordinateSystemName>;

using MinimalCoordinatesValueMatrix = ValueMatrix<double, 3, 1>;
using MinimalCoordinatesUnitMatrix = TypeMatrix<3, 1, AbsoluteLength, AbsoluteLength, AbsoluteAngle>;
using MinimalCoordinatesQuantityMatrix = QuantityMatrix<MinimalCoordinatesValueMatrix, MinimalCoordinatesUnitMatrix>;
using MinimalCoordinatesDtUnitMatrix = TypeMatrix<3, 1, Velocity, Velocity, AngularVelocity>;
using MinimalCoordinatesDtQuantityMatrix = QuantityMatrix<MinimalCoordinatesValueMatrix, MinimalCoordinatesDtUnitMatrix>;
using MinimalCoordinatesCoordinateSystemMatrix = TypeMatrix<3, 1, MinimalCoordinateSystem>;
using MinimalCoordinates = CoordinateMatrix<MinimalCoordinatesQuantityMatrix, MinimalCoordinatesCoordinateSystemMatrix>;
using MinimalCoordinatesDt = CoordinateMatrix<MinimalCoordinatesDtQuantityMatrix, MinimalCoordinatesCoordinateSystemMatrix>;

using PointValueMatrix = ValueMatrix<double, 2, 1>;
using PointUnitMatrix = TypeMatrix<2, 1, AbsoluteLength>;
using DirectionUnitMatrix = TypeMatrix<2, 1, Dimensionless>;
using VelocityUnitMatrix = TypeMatrix<2, 1, Velocity>;
using PointQuantityMatrix = QuantityMatrix<PointValueMatrix, PointUnitMatrix>;
using DirectionQuantityMatrix = QuantityMatrix<PointValueMatrix, DirectionUnitMatrix>;
using VelocityQuantityMatrix = QuantityMatrix<PointValueMatrix, VelocityUnitMatrix>;
using PointCoordinateSystemMatrix = TypeMatrix<2, 1, CartesianCoordinateSystem>;
using PositionPoint = CoordinateMatrix<PointQuantityMatrix, PointCoordinateSystemMatrix>;
using DirectionVector = CoordinateMatrix<DirectionQuantityMatrix, PointCoordinateSystemMatrix>;
using VelocityVector = CoordinateMatrix<VelocityQuantityMatrix, PointCoordinateSystemMatrix>;

using RadiusQuantity = Quantity<double, Length>;
using MassQuantity = Quantity<double, Mass>;
using EnergyQuantity = Quantity<double, Energy>;
using AngleQuantity = Quantity<double, Angle>;

using MassMatrixValues = ValueMatrix<double, 3, 3>;
using MassLength = Product<Mass, Division<Length, Angle>::type>::type;
using MassLength2 = Product<MassLength, Division<Length, Angle>::type>::type;
using MassMatrixUnits = TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
using MassMatrixQuantities = QuantityMatrix<MassMatrixValues, MassMatrixUnits>;
using MassMatrixSystemMatrix = TypeMatrix<3, 3, MinimalCoordinateSystem>;
using MassMatrix = CoordinateMatrix<MassMatrixQuantities, MassMatrixSystemMatrix>;

// class MinimalToCartesianTransformation
//    : public wuzp::PassiveTransformation<MinimalToCartesianTransformation, MinimalCoordinateSystem, CartesianCoordinateSystem> {
// public:
//  static constexpr int DimensionTo = 4;
//  static constexpr int DimensionFrom = 3;
//  using ValueMatrixT = ValueMatrix<double, DimensionTo, DimensionFrom>;
//  using UnitMatrixT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
//  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
//  using CoordinateSystemMatrixT = TypeMatrix<DimensionTo, DimensionFrom, CoordinateSystem>;
//  using CoordinateMatrixT = CoordinateMatrix<QuantityMatrixT, CoordinateSystemMatrixT>;
//  using TensorT = Tensor<1, 1, CoordinateMatrixT>;
//
//  template <typename... Args>
//  explicit LinearTransformation(Args... args) : tensor_(args...) {}
//
//  const TensorT& getTensor() const { return tensor_; }
//
// private:
//  TensorT tensor_;
//};

std::pair<PositionPoint, PositionPoint> minimalToCartesian(const MinimalCoordinates& minimalCoordinates, const RadiusQuantity& radius) {
  // TODO(remod): Introduce CoordinateMatrix::getQuantity()?
  // TODO(remod): Temporarily cast to relative? Or define sin/cos for absolute quantities
  const auto relativeAngle = makeRelative(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto positionVector = radius * DirectionVector(cos(relativeAngle), sin(relativeAngle));
  const PositionPoint position1 = castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinates.block<0, 0, 2, 1>()) + positionVector;
  const PositionPoint position2 = castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinates.block<0, 0, 2, 1>()) - positionVector;
  return std::make_pair(position1, position2);
}

std::pair<VelocityVector, VelocityVector> minimalToCartesian(const MinimalCoordinates& minimalCoordinates,
                                                             const MinimalCoordinatesDt& minimalCoordinatesDt,
                                                             const RadiusQuantity& radius) {
  const auto relativeAngle = makeRelative(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto velocityVector = radius * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>() *
                              DirectionVector(-sin(relativeAngle), cos(relativeAngle))
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                              / Quantity<double, Angle>(1.0)
#endif
      ;
  const VelocityVector velocity1 =
      castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinatesDt.block<0, 0, 2, 1>()) + velocityVector;
  const VelocityVector velocity2 =
      castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinatesDt.block<0, 0, 2, 1>()) - velocityVector;
  return std::make_pair(velocity1, velocity2);
}

TEST(TestCoordinateMatrix2, RotatingDumbbell) {  // NOLINT
  const MinimalCoordinates minimalCoordinates(0.2, 0.4, 0.5);
  const MinimalCoordinatesDt minimalCoordinatesDt(0.7, -0.3, 0.1);
  const RadiusQuantity radius(2.0);
  const MassQuantity mass1(1.0);
  const MassQuantity mass2(3.0);

  const auto cartesianCoordinates = minimalToCartesian(minimalCoordinates, radius);
  const auto cartesianCoordinatesDt = minimalToCartesian(minimalCoordinates, minimalCoordinatesDt, radius);

  // TODO(remod): Introduce norm() and squaredNorm() for vectors (and matrices?).
  const EnergyQuantity energyCartesianCoordinates = 0.5 * (mass1 * cartesianCoordinatesDt.first.dot(cartesianCoordinatesDt.first) +
                                                           mass2 * cartesianCoordinatesDt.second.dot(cartesianCoordinatesDt.second));

  const AngleQuantity relativeAngle = makeRelative(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());

  const MassQuantity massSum = mass1 + mass2;
  const MassQuantity massDifference = mass1 - mass2;
  const EnergyQuantity energyMinimalCoordinates1 =
      0.5 * (massSum * (pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()) +
                        pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>()) +
                        pow<2>(radius) * pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                / Quantity<double, Angle>(1.0)
#endif
                                                    )) +
             2.0 * radius * massDifference * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                 / Quantity<double, Angle>(1.0)
#endif
                 * (cos(relativeAngle) * minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>() -
                    sin(relativeAngle) * minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()));

  auto massMatrix00 = massSum;
  auto massMatrix01 = MassQuantity(0.0);
  auto massMatrix02 = -radius * massDifference * sin(relativeAngle)
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                      / Quantity<double, Angle>(1.0)
#endif
      ;
  auto massMatrix12 = radius * massDifference * cos(relativeAngle)
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                      / Quantity<double, Angle>(1.0)
#endif
      ;
  auto massMatrix22 = pow<2>(radius) * massSum
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                      / Quantity<double, Angle>(1.0) / Quantity<double, Angle>(1.0)
#endif
      ;
  const MassMatrix massMatrix(massMatrix00, massMatrix01, massMatrix02, massMatrix01, massMatrix00, massMatrix12, massMatrix02,
                              massMatrix12, massMatrix22);

  // TODO(remod): Enable implicit conversion from 1x1 quantity matrix to quantity. Only required if we don't just use 1x1 matrices as impl.
  const EnergyQuantity energyMinimalCoordinates2 =
      0.5 * (minimalCoordinatesDt.transpose() * massMatrix * minimalCoordinatesDt).getQuantityMatrix().getQuantity<0, 0>();

  // TODO(remod): Use isApprox() after switching to 1x1 matrices.
  EXPECT_NEAR(energyCartesianCoordinates.getValue(), energyMinimalCoordinates1.getValue(), 1e-5);
  EXPECT_NEAR(energyMinimalCoordinates1.getValue(), energyMinimalCoordinates2.getValue(), 1e-5);
}
