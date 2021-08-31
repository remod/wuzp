#include <gtest/gtest.h>

#include "ctm/core/Frame.hpp"
#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/trafo/Translation.hpp"
#include "ctm/units/QuantityTraits.hpp"
#include "ctm/value_matrix/ValueMatrix.hpp"

using ctm::Frame;
using ctm::FramedQuantityMatrix;
using ctm::FrameMatrix;
using ctm::Position2D;
using ctm::Position3D;
using ctm::QuantityMatrix;
using ctm::Translation2D;
using ctm::Translation3D;
using ctm::TypeMatrix;
using ctm::Vector3D;
using namespace ctm::units;         // NOLINT
using namespace ctm::value_matrix;  // NOLINT

struct FrameAName {};
struct FrameBName {};
struct FrameCName {};
struct FrameDName {};
struct FrameEName {};
struct FrameFName {};
using FrameA = Frame<FrameAName>;
using FrameB = Frame<FrameBName>;
using FrameC = Frame<FrameCName>;
using FrameD = Frame<FrameDName>;
using FrameE = Frame<FrameEName>;
using FrameF = Frame<FrameFName>;

class TestFramedQuantityMatrix : public ::testing::Test {
 public:
  using MyValues = ValueMatrix<int, 2, 3>;
  using MyUnits = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyFrames = FrameMatrix<2, 3, FrameA, FrameB, FrameC, FrameD, FrameE, FrameF>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;

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

TEST_F(TestFramedQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix(), getMatrix());
}

TEST_F(TestFramedQuantityMatrix, Block) {  // NOLINT
  using MyValuesB = ValueMatrix<int, 2, 2>;
  using MyUnitsB = UnitMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyFramesB = FrameMatrix<2, 2, FrameB, FrameC, FrameE, FrameF>;
  using MyMatrixB = FramedQuantityMatrix<MyValuesB, MyUnitsB, MyFramesB>;
  MyValuesB valuesB;
  valuesB << 2, 3, 5, 6;
  MyMatrixB matrixB(valuesB);
  EXPECT_EQ((getMatrix().block<0, 1, 2, 2>()), matrixB);
}

TEST_F(TestFramedQuantityMatrix, Transpose) {  // NOLINT
  using MyValuesT = ValueMatrix<int, 3, 2>;
  using MyUnitsT = UnitMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyFramesT = FrameMatrix<3, 2, FrameA, FrameD, FrameB, FrameE, FrameC, FrameF>;
  using MyMatrixT = FramedQuantityMatrix<MyValuesT, MyUnitsT, MyFramesT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix().transpose(), matrixT);
}

TEST(TestFramedQuantityMatrix2, ProductQuantityMatrix) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyFrames = FrameMatrix<2, 2, FrameA>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * matrix, MyMatrixResult(values * values));
}

TEST(TestFramedQuantityMatrix2, ProductQuantity) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyFrames = FrameMatrix<2, 2, FrameA>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrixResult = FramedQuantityMatrix<MyValues, MyUnitsResult, MyFrames>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * quantity, MyMatrixResult(values * value));
  EXPECT_EQ(quantity * matrix, MyMatrixResult(values * value));
}

TEST(TestFramedQuantityMatrix2, DivisionQuantity) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyFrames = FrameMatrix<2, 2, FrameA>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Dimensionless>;
  using MyMatrixResult = FramedQuantityMatrix<MyValues, MyUnitsResult, MyFrames>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix / quantity, MyMatrixResult(values / value));
}

TEST(TestFramedQuantityMatrix2, ScalarProduct) {  // NOLINT
  using MyValues = ValueMatrix<int, 4, 1>;
  using MyUnits = UnitMatrix<4, 1, Length>;
  using MyFrames = FrameMatrix<4, 1, FrameA, FrameB, FrameC, FrameD>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyMatrixResult = Quantity<int, Area>;
  MyValues values1;
  values1 << 1, 2, 3, 4;
  MyValues values2;
  values2 << 0, 3, -1, 2;
  EXPECT_EQ(MyMatrix(values1).dot(MyMatrix(values2)), QuantityTraits::Create<MyMatrixResult>::create(11));
}

TEST(TestFramedQuantityMatrix2, RotatingDumbbell) {  // NOLINT
  // From https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell
  struct FrameName;
  using Frame = Frame<FrameName>;

  using MinimalCoordinatesValues = ctm::value_matrix::ValueMatrix<double, 3, 1>;
  using MinimalCoordinatesUnits = ctm::units::UnitMatrix<3, 1, Absolute<Length>, Absolute<Length>, Angle>;
  using MinimalCoordinatesDtUnits = ctm::units::UnitMatrix<3, 1, Velocity, Velocity, AngularVelocity>;
  using MinimalCoordinatesFrames = ctm::FrameMatrix<3, 1, Frame, Frame, ctm::NoFrame>;
  using MinimalCoordinates = ctm::FramedQuantityMatrix<MinimalCoordinatesValues, MinimalCoordinatesUnits, MinimalCoordinatesFrames>;
  using MinimalCoordinatesDt = ctm::FramedQuantityMatrix<MinimalCoordinatesValues, MinimalCoordinatesDtUnits, MinimalCoordinatesFrames>;

  using PointValues = ctm::value_matrix::ValueMatrix<double, 2, 1>;
  using PointUnits = ctm::units::UnitMatrix<2, 1, Absolute<Length>>;
  using DirectionUnits = ctm::units::UnitMatrix<2, 1, Dimensionless>;
  using VelocityUnits = ctm::units::UnitMatrix<2, 1, Velocity>;
  using PointFrames = ctm::FrameMatrix<2, 1, Frame>;
  using Point = ctm::FramedQuantityMatrix<PointValues, PointUnits, PointFrames>;
  using Direction = ctm::FramedQuantityMatrix<PointValues, DirectionUnits, PointFrames>;
  using Velocity_ = ctm::FramedQuantityMatrix<PointValues, VelocityUnits, PointFrames>;

  using Radius = Quantity<double, Length>;
  using Mass_ = Quantity<double, Mass>;
  using Energy_ = Quantity<double, Energy>;

  using MassMatrixValues = ValueMatrix<double, 3, 3>;
  using MassLength = ctm::units::UnitTraits::Product<Mass, Length>::type;
  using MassLength2 = ctm::units::UnitTraits::Product<MassLength, Length>::type;
  using MassMatrixUnits = UnitMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
  using MassMatrix = QuantityMatrix<MassMatrixValues, MassMatrixUnits>;

  const MinimalCoordinates minimalCoordinates(Eigen::Matrix<double, 3, 1>::Random());
  const MinimalCoordinatesDt minimalCoordinatesDt(Eigen::Matrix<double, 3, 1>::Random());
  const Radius radius = QuantityTraits::Create<Radius>::create(2.0);
  const Mass_ mass1 = QuantityTraits::Create<Mass_>::create(1.0);
  const Mass_ mass2 = QuantityTraits::Create<Mass_>::create(1.0);

  // TODO(remod): Introduce FramedQuantityMatrix::getQuantity()?
  // TODO(remod): Avoid Eigen::Matrix() and getValue() calls.
  const auto positionVector =
      radius * Direction(Eigen::Matrix<double, 2, 1>(QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(
                                                         ctm::cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>())),
                                                     QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(
                                                         ctm::sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()))));
  const Point position1 = minimalCoordinates.block<0, 0, 2, 1>() + positionVector;
  const Point position2 = minimalCoordinates.block<0, 0, 2, 1>() - positionVector;
  const auto velocityVector =
      radius * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>() *
      Direction(Eigen::Matrix<double, 2, 1>(QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(
                                                -ctm::sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>())),
                                            QuantityTraits::GetValue<Quantity<double, Dimensionless>>::get(
                                                ctm::cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()))))
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
      / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
      ;
  const Velocity_ velocity1 = minimalCoordinatesDt.block<0, 0, 2, 1>() + velocityVector;
  const Velocity_ velocity2 = minimalCoordinatesDt.block<0, 0, 2, 1>() - velocityVector;

  const Mass_ mass = mass1 + mass2;
  const Mass_ massDelta = mass1 - mass2;
  const Energy_ energy1 = 0.5 * (mass * (ctm::pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()) +
                                         ctm::pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>()) +
                                         ctm::pow<2>(radius) * ctm::pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                                           / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
                                                                               )) +
                                 2.0 * radius * massDelta * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                     / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
                                     * (ctm::cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()) *
                                            minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>() -
                                        ctm::sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()) *
                                            minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()));

  // TODO(remod): Can numbers be implicitly converted to dimensionless quantities?
  const auto massMatrix00 = mass;
  const auto massMatrix01 = QuantityTraits::Create<Mass_>::create(0.0);
  const auto massMatrix02 = -radius * massDelta * ctm::sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto massMatrix12 = radius * massDelta * ctm::cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto massMatrix22 = ctm::pow<2>(radius) * mass;
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
  //  massMatrix.inverse();

  //  const Energy_ energy2 = 0.5 * minimalCoordinatesDt.transpose() * massMatrix * minimalCoordinatesDt;

  //  EXPECT_EQ(energy1, energy2);
}
