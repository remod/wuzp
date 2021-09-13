#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Translation.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
struct CoordinateSystemBName {};
struct CoordinateSystemCName {};
struct CoordinateSystemDName {};
struct CoordinateSystemEName {};
struct CoordinateSystemFName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
using CoordinateSystemC = CoordinateSystem<CoordinateSystemCName>;
using CoordinateSystemD = CoordinateSystem<CoordinateSystemDName>;
using CoordinateSystemE = CoordinateSystem<CoordinateSystemEName>;
using CoordinateSystemF = CoordinateSystem<CoordinateSystemFName>;

class TestCoordinateMatrix : public ::testing::Test {
 public:
  using MyValues = ValueMatrix<int, 2, 3>;
  using MyUnits = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyCoordinateSystems = CoordinateSystemMatrix<2, 3, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC, CoordinateSystemD,
                                                     CoordinateSystemE, CoordinateSystemF>;
  using MyMatrix = CoordinateMatrix<MyValues, MyUnits, MyCoordinateSystems>;

  const MyMatrix& getMatrix() const { return matrix_; }

 protected:
  void SetUp() override {
    values_ << 1, 2, 3, 4, 5, 6;
    matrix_.setValues(values_);
  }

 private:
  MyValues values_;
  MyMatrix matrix_;
};

TEST_F(TestCoordinateMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix(), getMatrix());
}

TEST_F(TestCoordinateMatrix, Block) {  // NOLINT
  using MyValuesB = ValueMatrix<int, 2, 2>;
  using MyUnitsB = UnitMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyCoordinateSystemsB = CoordinateSystemMatrix<2, 2, CoordinateSystemB, CoordinateSystemC, CoordinateSystemE, CoordinateSystemF>;
  using MyMatrixB = CoordinateMatrix<MyValuesB, MyUnitsB, MyCoordinateSystemsB>;
  MyValuesB valuesB;
  valuesB << 2, 3, 5, 6;
  MyMatrixB matrixB(valuesB);
  EXPECT_EQ((getMatrix().block<0, 1, 2, 2>()), matrixB);
}

TEST_F(TestCoordinateMatrix, Transpose) {  // NOLINT
  using MyValuesT = ValueMatrix<int, 3, 2>;
  using MyUnitsT = UnitMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyCoordinateSystemsT = CoordinateSystemMatrix<3, 2, CoordinateSystemA, CoordinateSystemD, CoordinateSystemB, CoordinateSystemE,
                                                      CoordinateSystemC, CoordinateSystemF>;
  using MyMatrixT = CoordinateMatrix<MyValuesT, MyUnitsT, MyCoordinateSystemsT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix().transpose(), matrixT);
}

TEST(TestCoordinateMatrix2, ProductQuantityMatrix) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyCoordinateSystems = CoordinateSystemMatrix<2, 2, CoordinateSystemA>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrix = CoordinateMatrix<MyValues, MyUnits, MyCoordinateSystems>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * matrix, MyMatrixResult(values * values));
}

TEST(TestCoordinateMatrix2, ProductQuantity) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyCoordinateSystems = CoordinateSystemMatrix<2, 2, CoordinateSystemA>;
  using MyMatrix = CoordinateMatrix<MyValues, MyUnits, MyCoordinateSystems>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrixResult = CoordinateMatrix<MyValues, MyUnitsResult, MyCoordinateSystems>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * quantity, MyMatrixResult(values * value));
  EXPECT_EQ(quantity * matrix, MyMatrixResult(values * value));
}

TEST(TestCoordinateMatrix2, DivisionQuantity) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyCoordinateSystems = CoordinateSystemMatrix<2, 2, CoordinateSystemA>;
  using MyMatrix = CoordinateMatrix<MyValues, MyUnits, MyCoordinateSystems>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Dimensionless>;
  using MyMatrixResult = CoordinateMatrix<MyValues, MyUnitsResult, MyCoordinateSystems>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix / quantity, MyMatrixResult(values / value));
}

TEST(TestCoordinateMatrix2, ScalarProduct) {  // NOLINT
  using MyValues = ValueMatrix<int, 4, 1>;
  using MyUnits = UnitMatrix<4, 1, Length>;
  using MyCoordinateSystems = CoordinateSystemMatrix<4, 1, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC, CoordinateSystemD>;
  using MyMatrix = CoordinateMatrix<MyValues, MyUnits, MyCoordinateSystems>;
  using MyMatrixResult = Quantity<int, Area>;
  MyValues values1;
  values1 << 1, 2, 3, 4;
  MyValues values2;
  values2 << 0, 3, -1, 2;
  EXPECT_EQ(MyMatrix(values1).dot(MyMatrix(values2)), QuantityTraits::Create<MyMatrixResult>::create(11));
}

TEST(TestCoordinateMatrix2, RotatingDumbbell) {  // NOLINT
  // From https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell
  struct CartesianCoordinateSystemName {};
  using CartesianCoordinateSystem = CoordinateSystem<CartesianCoordinateSystemName>;

  using MinimalCoordinatesValues = ValueMatrix<double, 3, 1>;
  using MinimalCoordinatesUnits = UnitMatrix<3, 1, Absolute<Length>, Absolute<Length>, Angle>;
  using MinimalCoordinatesDtUnits = UnitMatrix<3, 1, Velocity, Velocity, AngularVelocity>;
  using MinimalCoordinatesCoordinateSystems =
      CoordinateSystemMatrix<3, 1, CartesianCoordinateSystem, CartesianCoordinateSystem, NoCoordinateSystem>;
  using MinimalCoordinates = CoordinateMatrix<MinimalCoordinatesValues, MinimalCoordinatesUnits, MinimalCoordinatesCoordinateSystems>;
  using MinimalCoordinatesDt = CoordinateMatrix<MinimalCoordinatesValues, MinimalCoordinatesDtUnits, MinimalCoordinatesCoordinateSystems>;

  using PointValues = ValueMatrix<double, 2, 1>;
  using PointUnits = UnitMatrix<2, 1, Absolute<Length>>;
  using DirectionUnits = UnitMatrix<2, 1, Dimensionless>;
  using VelocityUnits = UnitMatrix<2, 1, Velocity>;
  using PointCoordinateSystems = CoordinateSystemMatrix<2, 1, CartesianCoordinateSystem>;
  using Point = CoordinateMatrix<PointValues, PointUnits, PointCoordinateSystems>;
  using Direction = CoordinateMatrix<PointValues, DirectionUnits, PointCoordinateSystems>;
  using Velocity_ = CoordinateMatrix<PointValues, VelocityUnits, PointCoordinateSystems>;

  using Radius = Quantity<double, Length>;
  using Mass_ = Quantity<double, Mass>;
  using Energy_ = Quantity<double, Energy>;

  using MassMatrixValues = ValueMatrix<double, 3, 3>;
  using MassLength = UnitTraits::Product<Mass, Length>::type;
  using MassLength2 = UnitTraits::Product<MassLength, Length>::type;
  using MassMatrixUnits = UnitMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
  using MassMatrix = QuantityMatrix<MassMatrixValues, MassMatrixUnits>;

  const MinimalCoordinates minimalCoordinates(Eigen::Matrix<double, 3, 1>::Random());
  const MinimalCoordinatesDt minimalCoordinatesDt(Eigen::Matrix<double, 3, 1>::Random());
  const Radius radius = QuantityTraits::Create<Radius>::create(2.0);
  const Mass_ mass1 = QuantityTraits::Create<Mass_>::create(1.0);
  const Mass_ mass2 = QuantityTraits::Create<Mass_>::create(1.0);

  // TODO(remod): Introduce CoordinateMatrix::getQuantity()?
  // TODO(remod): Avoid Eigen::Matrix() and getValue() calls.
  const auto positionVector =
      radius *
      Direction(Eigen::Matrix<double, 2, 1>(
          QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>())),
          QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()))));
  const Point position1 = minimalCoordinates.block<0, 0, 2, 1>() + positionVector;
  const Point position2 = minimalCoordinates.block<0, 0, 2, 1>() - positionVector;
  const auto velocityVector =
      radius * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>() *
      Direction(Eigen::Matrix<double, 2, 1>(
          QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(-sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>())),
          QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()))))
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
      / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
      ;
  const Velocity_ velocity1 = minimalCoordinatesDt.block<0, 0, 2, 1>() + velocityVector;
  const Velocity_ velocity2 = minimalCoordinatesDt.block<0, 0, 2, 1>() - velocityVector;

  const Mass_ mass = mass1 + mass2;
  const Mass_ massDelta = mass1 - mass2;
  const Energy_ energy1 = 0.5 * (mass * (pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()) +
                                         pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>()) +
                                         pow<2>(radius) * pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                                 / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
                                                                     )) +
                                 2.0 * radius * massDelta * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                     / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
                                     * (cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()) *
                                            minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>() -
                                        sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()) *
                                            minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()));

  // TODO(remod): Can numbers be implicitly converted to dimensionless quantities?
  const auto massMatrix00 = mass;
  const auto massMatrix01 = QuantityTraits::Create<Mass_>::create(0.0);
  const auto massMatrix02 = -radius * massDelta * sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto massMatrix12 = radius * massDelta * cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto massMatrix22 = pow<2>(radius) * mass;
  MassMatrix massMatrix;
  massMatrix.setQuantity<0, 0>(massMatrix00);
  massMatrix.setQuantity<0, 1>(massMatrix01);
  massMatrix.setQuantity<0, 2>(massMatrix02);
  massMatrix.setQuantity<1, 0>(massMatrix01);
  massMatrix.setQuantity<1, 1>(massMatrix00);
  massMatrix.setQuantity<1, 2>(massMatrix12);
  massMatrix.setQuantity<2, 0>(massMatrix02);
  massMatrix.setQuantity<2, 1>(massMatrix12);
  massMatrix.setQuantity<2, 2>(massMatrix22);

  //  const Energy_ energy2 = 0.5 * minimalCoordinatesDt.transpose() * massMatrix * minimalCoordinatesDt;

  //  EXPECT_EQ(energy1, energy2);
}
