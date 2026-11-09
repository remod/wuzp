#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

// From https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell
struct CartesianCoordinateSystemName {};
using CartesianCoordinateSystem = wuzp::CoordinateSystem<CartesianCoordinateSystemName>;
struct MinimalCoordinateSystemName {};
using MinimalCoordinateSystem = wuzp::CoordinateSystem<MinimalCoordinateSystemName>;

using MinimalCoordinatesValueVector = ValueMatrix<double, 3, 1>;
using MinimalCoordinatesUnitVector = wuzp::TypeMatrix<3, 1, Length, Length, Angle>;
using MinimalCoordinatesQuantityVector = wuzp::Quantities<MinimalCoordinatesValueVector, MinimalCoordinatesUnitVector>;
using MinimalCoordinatesDtUnitVector = wuzp::TypeMatrix<3, 1, Velocity, Velocity, AngularVelocity>;
using MinimalCoordinatesDtQuantityVector = wuzp::Quantities<MinimalCoordinatesValueVector, MinimalCoordinatesDtUnitVector>;
using MinimalCoordinates = wuzp::Point<MinimalCoordinatesQuantityVector, MinimalCoordinateSystem>;
using MinimalCoordinatesDt = wuzp::Vector<MinimalCoordinatesDtQuantityVector, MinimalCoordinateSystem>;

using PositionPoint = wuzp::PositionPoint2<ValueMatrix, double, CartesianCoordinateSystem>;
using PositionVector = wuzp::PositionVector2<ValueMatrix, double, CartesianCoordinateSystem>;
using DirectionVector = wuzp::DirectionVector2<ValueMatrix, double, CartesianCoordinateSystem>;
using VelocityVector = wuzp::VelocityVector2<ValueMatrix, double, CartesianCoordinateSystem>;

using MassLength = wuzp::Product<Mass, wuzp::Division<Length, Angle>::type>::type;
using MassLength2 = wuzp::Product<MassLength, wuzp::Division<Length, Angle>::type>::type;

using RadiusQuantity = wuzp::QuantityD<Length>;
using MassQuantity = wuzp::QuantityD<Mass>;
using MassLengthQuantity = wuzp::QuantityD<MassLength>;
using MassLength2Quantity = wuzp::QuantityD<MassLength2>;
using EnergyQuantity = wuzp::QuantityD<Energy>;
using AngleQuantity = wuzp::QuantityD<Angle>;

using MassMatrixValues = ValueMatrix<double, 3, 3>;
using MassMatrixUnits = wuzp::TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
using MassMatrixQuantities = wuzp::Quantities<MassMatrixValues, MassMatrixUnits>;
using MassMatrix = wuzp::Tensor<0, 2, MassMatrixQuantities, MinimalCoordinateSystem, wuzp::Relative>;

// class MinimalToCartesianTransformation
//    : public wuzp::PassiveTransformation<MinimalToCartesianTransformation, MinimalCoordinateSystem, CartesianCoordinateSystem> {
// public:
//  static constexpr int DimensionTo = 4;
//  static constexpr int DimensionFrom = 3;
//  using ValuesT = ValueMatrix<double, DimensionTo, DimensionFrom>;
//  using UnitMatrixT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
//  using QuantitiesT = Quantities<ValuesT, UnitMatrixT>;
//  using CoordinateSystemMatrixT = TypeMatrix<DimensionTo, DimensionFrom, CoordinateSystem>;
//  using CoordinateMatrixT = CoordinateMatrix<QuantitiesT, CoordinateSystemMatrixT>;
//  using TensorT = Tensor<1, 1, CoordinateMatrixT>;
//
//  template <typename... Args>
//  explicit PassiveLinearTransformation(Args... args) : tensor_(args...) {}
//
//  const TensorT& getTensor() const { return tensor_; }
//
// private:
//  TensorT tensor_;
//};

// TODO(remod): Create a custom transformation class for this?
std::pair<PositionPoint, PositionPoint> minimalToCartesian(const MinimalCoordinates& minimalCoordinates, const RadiusQuantity& radius) {
  // TODO(remod): Temporarily cast to relative? Or define sin/cos for absolute quantities
  // TODO(remod): Replace all autos by explicit types and then try to remove all evaluate calls.
  const AngleQuantity relativeAngle{minimalCoordinates.getQuantity<2, 0>()};
  const PositionVector positionVector{radius * DirectionVector{cos(relativeAngle), sin(relativeAngle)}};
  const PositionPoint position1{castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinates.block<0, 0, 2, 1>()) + positionVector};
  const PositionPoint position2{castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinates.block<0, 0, 2, 1>()) - positionVector};
  return std::make_pair(position1, position2);
}

std::pair<VelocityVector, VelocityVector> minimalToCartesian(const MinimalCoordinates& minimalCoordinates,
                                                             const MinimalCoordinatesDt& minimalCoordinatesDt,
                                                             const RadiusQuantity& radius) {
  const AngleQuantity relativeAngle{minimalCoordinates.getQuantity<2, 0>()};
  const VelocityVector velocityVector{radius * minimalCoordinatesDt.getQuantity<2, 0>() *
                                      DirectionVector(-sin(relativeAngle), cos(relativeAngle))};
  const VelocityVector velocity1{castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinatesDt.block<0, 0, 2, 1>()) +
                                 velocityVector};
  const VelocityVector velocity2{castCoordinateSystem<CartesianCoordinateSystem>(minimalCoordinatesDt.block<0, 0, 2, 1>()) -
                                 velocityVector};
  return std::make_pair(velocity1, velocity2);
}

TEST(TestCoordinateMatrix2, RotatingDumbbell) {  // NOLINT
  const MinimalCoordinates minimalCoordinates{0.2, 0.4, 0.5};
  const MinimalCoordinatesDt minimalCoordinatesDt{0.7, -0.3, 0.1};
  const RadiusQuantity radius{2.0};
  const MassQuantity mass1{1.0};
  const MassQuantity mass2{3.0};

  const std::pair<PositionPoint, PositionPoint> cartesianCoordinates{minimalToCartesian(minimalCoordinates, radius)};
  // The dumbbell is rigid: the masses are always two radii apart.
  const RadiusQuantity distance{(cartesianCoordinates.first - cartesianCoordinates.second).norm()};
  EXPECT_TRUE(distance.isApprox(RadiusQuantity{4.0}, 1e-12));
  const std::pair<VelocityVector, VelocityVector> cartesianCoordinatesDt{
      minimalToCartesian(minimalCoordinates, minimalCoordinatesDt, radius)};

  const EnergyQuantity energyCartesianCoordinates{
      0.5 * (mass1 * cartesianCoordinatesDt.first.squaredNorm() + mass2 * cartesianCoordinatesDt.second.squaredNorm())};

  const AngleQuantity relativeAngle{minimalCoordinates.getQuantity<2, 0>()};

  const MassQuantity massSum{mass1 + mass2};
  const MassQuantity massDifference{mass1 - mass2};
  // TODO(remod): For vectors, make it possible to write <1> instead of <1, 0>.
  const EnergyQuantity energyMinimalCoordinates1{
      0.5 * (massSum * (pow<2>(minimalCoordinatesDt.getQuantity<0, 0>()) + pow<2>(minimalCoordinatesDt.getQuantity<1, 0>()) +
                        pow<2>(radius) * pow<2>(minimalCoordinatesDt.getQuantity<2, 0>())) +
             2.0 * radius * massDifference * minimalCoordinatesDt.getQuantity<2, 0>() *
                 (cos(relativeAngle) * minimalCoordinatesDt.getQuantity<1, 0>() -
                  sin(relativeAngle) * minimalCoordinatesDt.getQuantity<0, 0>()))};

  const MassQuantity massMatrix00{massSum};
  const MassQuantity massMatrix01{0.0};
  const MassLengthQuantity massMatrix02{-radius * massDifference * sin(relativeAngle)};
  const MassLengthQuantity massMatrix12{radius * massDifference * cos(relativeAngle)};
  const MassLength2Quantity massMatrix22{pow<2>(radius) * massSum};
  const MassMatrix massMatrix{massMatrix00, massMatrix01, massMatrix02, massMatrix01, massMatrix00,
                              massMatrix12, massMatrix02, massMatrix12, massMatrix22};

  const EnergyQuantity energyMinimalCoordinates2{
      0.5 * innerProduct(minimalCoordinatesDt.transpose(), innerProduct(massMatrix, minimalCoordinatesDt)).getQuantity()};

  EXPECT_TRUE(energyCartesianCoordinates.isApprox(energyMinimalCoordinates1))
      << energyCartesianCoordinates << " is not approximately " << energyMinimalCoordinates1;
  EXPECT_TRUE(energyMinimalCoordinates1.isApprox(energyMinimalCoordinates2))
      << energyMinimalCoordinates1 << " is not approximately " << energyMinimalCoordinates2;
}
