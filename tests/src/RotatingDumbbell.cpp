#include <utility>

#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

// From https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell
struct CartesianCoordinateSystemName {};
using CartesianCoordinateSystem = wuzp::CoordinateSystem<CartesianCoordinateSystemName>;
struct MinimalCoordinateSystemName {};
using MinimalCoordinateSystem = wuzp::CoordinateSystem<MinimalCoordinateSystemName>;
// The Cartesian coordinates (x1, y1, x2, y2) of both masses, in which the mass matrix is diagonal.
struct MassesCoordinateSystemName {};
using MassesCoordinateSystem = wuzp::CoordinateSystem<MassesCoordinateSystemName>;

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

using RadiusQuantity = QuantityD<Length>;
using MassQuantity = QuantityD<Mass>;
using MassLengthQuantity = QuantityD<MassLength>;
using MassLength2Quantity = QuantityD<MassLength2>;
using EnergyQuantity = QuantityD<Energy>;
using AngleQuantity = QuantityD<Angle>;

using MassMatrixValues = ValueMatrix<double, 3, 3>;
using MassMatrixUnits = wuzp::TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
using MassMatrixQuantities = wuzp::Quantities<MassMatrixValues, MassMatrixUnits>;
using MassMatrix = wuzp::Tensor<wuzp::Indices<wuzp::Down, wuzp::Down>, MassMatrixQuantities, MinimalCoordinateSystem, wuzp::Relative>;

using MassesVelocity = wuzp::VelocityVector<4, ValueMatrix, double, MassesCoordinateSystem>;
using MassesMassMatrix =
    wuzp::Tensor<wuzp::Indices<wuzp::Down, wuzp::Down>, wuzp::Quantities<ValueMatrix<double, 4, 4>, wuzp::TypeMatrix<4, 4, Mass>>,
                 MassesCoordinateSystem, wuzp::Relative>;
using CartesianMassMatrix =
    wuzp::Tensor<wuzp::Indices<wuzp::Down, wuzp::Down>, wuzp::Quantities<ValueMatrix<double, 2, 2>, wuzp::TypeMatrix<2, 2, Mass>>,
                 CartesianCoordinateSystem, wuzp::Relative>;

// The Jacobians of the Cartesian coordinates of both masses, and of the first mass, with respect to the minimal coordinates. The
// units of their elements are [x_i] / [q_j]: 1 for the derivatives by x and y, and m / rad for those by phi.
using LengthOverAngle = wuzp::Division<Length, Angle>::type;
using MassesJacobian = wuzp::Jacobian<ValueMatrix, double, MinimalCoordinateSystem, MassesCoordinateSystem, 3, 4, Dimensionless,
                                      Dimensionless, LengthOverAngle, Dimensionless, Dimensionless, LengthOverAngle, Dimensionless,
                                      Dimensionless, LengthOverAngle, Dimensionless, Dimensionless, LengthOverAngle>;
using FirstMassJacobian = wuzp::Jacobian<ValueMatrix, double, MinimalCoordinateSystem, CartesianCoordinateSystem, 3, 2, Dimensionless,
                                         Dimensionless, LengthOverAngle, Dimensionless, Dimensionless, LengthOverAngle>;

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

namespace {

// The mass matrix of the minimal coordinates (see TestCoordinateMatrix2.RotatingDumbbell).
MassMatrix computeMassMatrix(const MinimalCoordinates& minimalCoordinates, const RadiusQuantity& radius, const MassQuantity& mass1,
                             const MassQuantity& mass2) {
  const AngleQuantity relativeAngle{minimalCoordinates.getQuantity<2, 0>()};
  const MassQuantity massSum{mass1 + mass2};
  const MassQuantity massDifference{mass1 - mass2};
  const MassQuantity massMatrix00{massSum};
  const MassQuantity massMatrix01{0.0};
  const MassLengthQuantity massMatrix02{-radius * massDifference * sin(relativeAngle)};
  const MassLengthQuantity massMatrix12{radius * massDifference * cos(relativeAngle)};
  const MassLength2Quantity massMatrix22{pow<2>(radius) * massSum};
  return MassMatrix{massMatrix00, massMatrix01, massMatrix02, massMatrix01, massMatrix00,
                    massMatrix12, massMatrix02, massMatrix12, massMatrix22};
}

// The Jacobian of the Cartesian coordinates (x1, y1, x2, y2) of both masses with respect to the minimal coordinates (x, y, phi),
// where (x1, y1) = (x, y) + r (cos(phi), sin(phi)) and (x2, y2) = (x, y) - r (cos(phi), sin(phi)) (see minimalToCartesian()).
MassesJacobian computeMassesJacobian(const MinimalCoordinates& minimalCoordinates, const RadiusQuantity& radius) {
  using DimensionlessQuantity = QuantityD<Dimensionless>;
  using LengthOverAngleQuantity = QuantityD<LengthOverAngle>;
  const AngleQuantity relativeAngle{minimalCoordinates.getQuantity<2, 0>()};
  const DimensionlessQuantity one{1.0};
  const DimensionlessQuantity zero{0.0};
  const LengthOverAngleQuantity dx1dphi{-radius * sin(relativeAngle)};
  const LengthOverAngleQuantity dy1dphi{radius * cos(relativeAngle)};
  return MassesJacobian{one, zero, dx1dphi, zero, one, dy1dphi, one, zero, -dx1dphi, zero, one, -dy1dphi};
}

const MinimalCoordinates minimalCoordinates{0.2, 0.4, 0.5};
const MinimalCoordinatesDt minimalCoordinatesDt{0.7, -0.3, 0.1};
const RadiusQuantity radius{2.0};
const MassQuantity mass1{1.0};
const MassQuantity mass2{3.0};

}  // namespace

TEST(TestCoordinateMatrix2, RotatingDumbbell) {  // NOLINT
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

  const MassMatrix massMatrix{computeMassMatrix(minimalCoordinates, radius, mass1, mass2)};

  const EnergyQuantity energyMinimalCoordinates2{
      0.5 * innerProduct(innerProduct(massMatrix, minimalCoordinatesDt), minimalCoordinatesDt).getQuantity()};

  EXPECT_TRUE(energyCartesianCoordinates.isApprox(energyMinimalCoordinates1))
      << energyCartesianCoordinates << " is not approximately " << energyMinimalCoordinates1;
  EXPECT_TRUE(energyMinimalCoordinates1.isApprox(energyMinimalCoordinates2))
      << energyMinimalCoordinates1 << " is not approximately " << energyMinimalCoordinates2;
}

// The mass matrix of the minimal coordinates is the pullback J^T M_x J of the diagonal mass matrix M_x = diag(m1, m1, m2, m2) of
// the Cartesian coordinates of both masses by the Jacobian J of the Cartesian coordinates with respect to the minimal coordinates,
// since the kinetic energy is (J q')^T M_x (J q') / 2. The Jacobian pushes the velocities q' of the minimal coordinates forward to
// those of the masses.
TEST(TestRotatingDumbbell, MassMatrixIsThePullbackOfTheCartesianMassMatrix) {  // NOLINT
  const MassesJacobian jacobian{computeMassesJacobian(minimalCoordinates, radius)};
  const MassQuantity zero{0.0};
  const MassesMassMatrix cartesianMassMatrix{mass1, zero, zero,  zero, zero, mass1, zero, zero,
                                             zero,  zero, mass2, zero, zero, zero,  zero, mass2};
  const MassMatrix massMatrix{jacobian.pullback(cartesianMassMatrix)};
  EXPECT_TRUE(massMatrix.isApprox(computeMassMatrix(minimalCoordinates, radius, mass1, mass2), 1e-12));

  const MassesVelocity velocities{jacobian.transform(minimalCoordinatesDt)};
  const std::pair<VelocityVector, VelocityVector> cartesianCoordinatesDt{
      minimalToCartesian(minimalCoordinates, minimalCoordinatesDt, radius)};
  EXPECT_TRUE(
      wuzp::castCoordinateSystem<CartesianCoordinateSystem>(velocities.block<0, 0, 2, 1>()).isApprox(cartesianCoordinatesDt.first, 1e-12));
  EXPECT_TRUE(
      wuzp::castCoordinateSystem<CartesianCoordinateSystem>(velocities.block<2, 0, 2, 1>()).isApprox(cartesianCoordinatesDt.second, 1e-12));
}

// The operational space inertia of Khatib, Lambda = (J M^-1 J^T)^-1 for the Jacobian J of the first mass ("A unified approach for
// motion and force control of robot manipulators: The operational space formulation", IEEE Journal on Robotics and Automation
// 3(1), 1987, https://doi.org/10.1109/JRA.1987.1087068): the inertia that a force at the first mass feels. The inverse of the mass
// matrix has two upper indices, so the Jacobian pushes it forward, and the inverse of that has two lower indices again. The rod
// between the masses has no mass, so it only transmits forces along itself: a force along the rod accelerates both masses, one
// across it only the first one. So Lambda = (m1 + m2) u u^T + m1 n n^T, with the direction u = (cos(phi), sin(phi)) of the rod
// and its normal n = (-sin(phi), cos(phi)).
TEST(TestRotatingDumbbell, OperationalSpaceInertia) {  // NOLINT
  const MassMatrix massMatrix{computeMassMatrix(minimalCoordinates, radius, mass1, mass2)};
  const FirstMassJacobian jacobian{computeMassesJacobian(minimalCoordinates, radius).getQuantities().block<0, 0, 2, 3>()};
  const CartesianMassMatrix operationalSpaceInertia{jacobian.transform(massMatrix.inverse()).inverse()};

  const AngleQuantity relativeAngle{minimalCoordinates.getQuantity<2, 0>()};
  const DirectionVector direction{cos(relativeAngle), sin(relativeAngle)};
  const DirectionVector normal{-sin(relativeAngle), cos(relativeAngle)};
  const CartesianMassMatrix expected{(mass1 + mass2) * outerProduct(direction.transpose(), direction.transpose()) +
                                     mass1 * outerProduct(normal.transpose(), normal.transpose())};
  EXPECT_TRUE(operationalSpaceInertia.isApprox(expected, 1e-12));
}
