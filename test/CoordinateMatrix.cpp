#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"

#include "test_compilation_error.hpp"

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

template <int Rows, int Cols>
using TestValueMatrix = ValueMatrix<double, Rows, Cols>;
template <int Rows, int Cols>
using TestUnitMatrix = TypeMatrix<Rows, Cols, Length>;
template <int Rows, int Cols>
using TestQuantityMatrix = QuantityMatrix<TestValueMatrix<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;
template <int Rows, int Cols>
using TestCoordinateSystemMatrix = TypeMatrix<Rows, Cols, CoordinateSystemA>;
template <int Rows, int Cols>
using TestCoordinateMatrix = CoordinateMatrix<TestQuantityMatrix<Rows, Cols>, TestCoordinateSystemMatrix<Rows, Cols>>;

TEST(TestCoordinateMatrix2, Dimension) {  // NOLINT
  static_assert(Dimension<TestCoordinateMatrix<1, 1>>::value == 0);
  static_assert(Dimension<TestCoordinateMatrix<3, 1>>::value == 1);
  static_assert(Dimension<TestCoordinateMatrix<1, 2>>::value == 1);
  static_assert(Dimension<TestCoordinateMatrix<3, 2>>::value == 2);
}

TEST(TestCoordinateMatrix2, Rows) {  // NOLINT
  static_assert(Rows<TestQuantityMatrix<3, 2>>::value == 3);
}

TEST(TestCoordinateMatrix2, Cols) {  // NOLINT
  static_assert(Cols<TestQuantityMatrix<3, 2>>::value == 2);
}

TEST(TestCoordinateMatrix2, GetTag) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix =
      TypeMatrix<2, 3, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC, CoordinateSystemD, CoordinateSystemE, CoordinateSystemF>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, MyCoordinateMatrix>::type, FromCoordinateMatrix>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, const MyCoordinateMatrix&>::type, FromCoordinateMatrix>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, MyCoordinateMatrix&&>::type, FromCoordinateMatrix>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, MyQuantityMatrix>::type, FromQuantityMatrix>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, Quantity<double, Length>>::type, FromQuantityMatrixArgs>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, MyValueMatrix>::type, FromQuantityMatrixArgs>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, double>::type, FromQuantityMatrixArgs>);
}

TEST(TestCoordinateMatrix2, EmptyConstructor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  [[maybe_unused]] const MyCoordinateMatrix coordinateMatrix;
}

TEST(TestCoordinateMatrix2, ConstructImplicitlyFromCoordinateMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  const MyCoordinateMatrix coordinateMatrix1;
  const MyCoordinateMatrix coordinateMatrix2;
  const auto coordinateMatrix3 = coordinateMatrix1 + coordinateMatrix2;
  const MyCoordinateMatrix coordinateMatrix = coordinateMatrix3;
  EXPECT_EQ(coordinateMatrix, coordinateMatrix3);
}

TEST(TestCoordinateMatrix2, ConstructImplicitlyFromQuantityMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;

int main() {
  const MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyCoordinateMatrix coordinateMatrix = quantityMatrix;
  return 0;
}
)",
                                        "conversion from");
}

TEST(TestCoordinateMatrix2, ConstructExplicitlyFromQuantityMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  const MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  const MyCoordinateMatrix coordinateMatrix(quantityMatrix);
  EXPECT_EQ(coordinateMatrix.getQuantityMatrix(), quantityMatrix);
}

TEST(TestCoordinateMatrix2, ConstructImplicitlyFromValueMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;

int main() {
  MyValueMatrix valueMatrix;
  valueMatrix << 1, 2, 3, 4, 5, 6;
  [[maybe_unused]] const MyCoordinateMatrix coordinateMatrix = valueMatrix;
  return 0;
}
)",
                                        "conversion from");
}

TEST(TestCoordinateMatrix2, ConstructExplicitlyFromValueMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix.getValueMatrix(), valueMatrix);
}

TEST(TestCoordinateMatrix2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  const MyCoordinateMatrix coordinateMatrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(coordinateMatrix.getValueMatrix(), valueMatrix);
}

class TestCoordinateMatrixF : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix =
      TypeMatrix<2, 3, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC, CoordinateSystemD, CoordinateSystemE, CoordinateSystemF>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;

  const MyCoordinateMatrix& getCoordinateMatrix() const { return coordinateMatrix_; }

 protected:
  void SetUp() override { coordinateMatrix_.setValueMatrix(valueMatrix_); }

 private:
  MyValueMatrix valueMatrix_{1, 2, 3, 4, 5, 6};
  MyCoordinateMatrix coordinateMatrix_;
};

TEST_F(TestCoordinateMatrixF, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getCoordinateMatrix(), getCoordinateMatrix());
}

TEST_F(TestCoordinateMatrixF, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = QuantityMatrix<MyValueMatrixB, MyUnitMatrixB>;
  using MyCoordinateSystemMatrixB = TypeMatrix<2, 2, CoordinateSystemB, CoordinateSystemC, CoordinateSystemE, CoordinateSystemF>;
  using MyCoordinateMatrixB = CoordinateMatrix<MyQuantityMatrixB, MyCoordinateSystemMatrixB>;
  const MyValueMatrixB valueMatrixB(2, 3, 5, 6);
  const MyCoordinateMatrixB coordinateMatrixB(valueMatrixB);
  EXPECT_EQ((getCoordinateMatrix().block<0, 1, 2, 2>()), coordinateMatrixB);
}

TEST_F(TestCoordinateMatrixF, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = QuantityMatrix<MyValueMatrixT, MyUnitMatrixT>;
  using MyCoordinateSystemMatrixT =
      TypeMatrix<3, 2, CoordinateSystemA, CoordinateSystemD, CoordinateSystemB, CoordinateSystemE, CoordinateSystemC, CoordinateSystemF>;
  using MyCoordinateMatrixT = CoordinateMatrix<MyQuantityMatrixT, MyCoordinateSystemMatrixT>;
  const MyValueMatrixT valueMatrixT(1, 4, 2, 5, 3, 6);
  const MyCoordinateMatrixT coordinateMatrixT(valueMatrixT);
  EXPECT_EQ(getCoordinateMatrix().transpose(), coordinateMatrixT);
}

TEST(TestCoordinateMatrix2, ProductCoordinateMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix * coordinateMatrix, MyCoordinateMatrixResult(valueMatrix * valueMatrix));
  // OLD:
  //  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  //  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  //  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  //  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  //  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  //  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  //  MyValueMatrix valueMatrix;
  //  valueMatrix << 1, 2, 3, 4;
  //  MyCoordinateMatrix coordinateMatrix;
  //  coordinateMatrix.setValueMatrix(valueMatrix);
  //  EXPECT_EQ(coordinateMatrix * coordinateMatrix, MyQuantityMatrixResult(valueMatrix * valueMatrix));
}

TEST(TestCoordinateMatrix2, ProductQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
  const MyValue value{2};
  const MyQuantity quantity(value);
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix * quantity, MyCoordinateMatrixResult(valueMatrix * value));
  EXPECT_EQ(quantity * coordinateMatrix, MyCoordinateMatrixResult(valueMatrix * value));
}

TEST(TestCoordinateMatrix2, DivisionQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
  const MyValue value{2};
  const MyQuantity quantity(value);
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix / quantity, MyCoordinateMatrixResult(valueMatrix / value));
}

TEST(TestCoordinateMatrix2, Dot) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<4, 1, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyQuantityResult = Quantity<int, Area>;
  EXPECT_EQ(MyCoordinateMatrix(1, 2, 3, 4).dot(MyCoordinateMatrix(0, 3, -1, 2)), MyQuantityResult(11));
}

TEST(TestCoordinateMatrix2, DotBadFrames) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
struct CoordinateSystemBName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
using MyValueMatrix = ValueMatrix<int, 4, 1>;
using MyUnitMatrix = TypeMatrix<4, 1, Length>;
using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
using MyCoordinateSystemMatrixLhs = TypeMatrix<4, 1, CoordinateSystemA>;
using MyCoordinateSystemMatrixRhs = TypeMatrix<4, 1, CoordinateSystemB>;
using MyCoordinateMatrixLhs = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrixLhs>;
using MyCoordinateMatrixRhs = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrixRhs>;

int main() {
  MyCoordinateMatrixLhs(1, 2, 3, 4).dot(MyCoordinateMatrixRhs(0, 3, -1, 2));
  return 0;
}
)",
                                               "Coordinate systems do not match.");
}

TEST(TestCoordinateMatrix2, DotBadFrames2) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
struct CoordinateSystemBName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
using MyValueMatrix = ValueMatrix<int, 4, 1>;
using MyUnitMatrix = TypeMatrix<4, 1, Length>;
using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
using MyCoordinateSystemMatrix = TypeMatrix<4, 1, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, CoordinateSystemB>;
using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;

int main() {
  MyCoordinateMatrix(1, 2, 3, 4).dot(MyCoordinateMatrix(0, 3, -1, 2));
  return 0;
}
)",
                                               "Coordinate systems do not match.");
}

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
using PointVector = CoordinateMatrix<PointQuantityMatrix, PointCoordinateSystemMatrix>;
using DirectionVector = CoordinateMatrix<DirectionQuantityMatrix, PointCoordinateSystemMatrix>;
using VelocityVector = CoordinateMatrix<VelocityQuantityMatrix, PointCoordinateSystemMatrix>;

using RadiusQuantity = Quantity<double, Length>;
using MassQuantity = Quantity<double, Mass>;
using EnergyQuantity = Quantity<double, Energy>;

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

template <typename To, typename T>
auto castCoordinateSystem2(T t) {
  return typename CastCoordinateSystem<T, To>::type(t.getQuantityMatrix());
}

auto minimalToCartesian(const MinimalCoordinates& minimalCoordinates, const RadiusQuantity& radius) {
  // TODO(remod): Introduce CoordinateMatrix::getQuantity()?
  // TODO(remod): Temporarily cast to relative? Or define sin/cos for absolute quantities
  const auto relativeAngle = makeRelative(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto positionVector = radius * DirectionVector(cos(relativeAngle), sin(relativeAngle));
  // TODO(remod): use auto.
  const PointVector position1 = castCoordinateSystem2<CartesianCoordinateSystem>(minimalCoordinates.block<0, 0, 2, 1>()) + positionVector;
  const PointVector position2 = castCoordinateSystem2<CartesianCoordinateSystem>(minimalCoordinates.block<0, 0, 2, 1>()) - positionVector;
  return std::make_pair(position1, position2);
}

auto minimalToCartesian(const MinimalCoordinates& minimalCoordinates, const MinimalCoordinatesDt& minimalCoordinatesDt,
                        const RadiusQuantity& radius) {
  const auto relativeAngle = makeRelative(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
  const auto velocityVector = radius * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>() *
                              DirectionVector(-sin(relativeAngle), cos(relativeAngle))
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                              / Quantity<double, Angle>(1.0)
#endif
      ;
  const VelocityVector velocity1 =
      castCoordinateSystem2<CartesianCoordinateSystem>(minimalCoordinatesDt.block<0, 0, 2, 1>()) + velocityVector;
  const VelocityVector velocity2 =
      castCoordinateSystem2<CartesianCoordinateSystem>(minimalCoordinatesDt.block<0, 0, 2, 1>()) - velocityVector;
  return std::make_pair(velocity1, velocity2);
}

TEST(TestCoordinateMatrix2, RotatingDumbbell) {  // NOLINT
  const auto minimalCoordinates = MinimalCoordinates::random();
  const auto minimalCoordinatesDt = MinimalCoordinatesDt::random();
  const RadiusQuantity radius(2.0);
  const MassQuantity mass1(1.0);
  const MassQuantity mass2(1.0);

  const auto cartesianCoordinates = minimalToCartesian(minimalCoordinates, radius);
  const auto cartesianCoordinatesDt = minimalToCartesian(minimalCoordinates, minimalCoordinatesDt, radius);

  const auto relativeAngle = makeRelative(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());

  const MassQuantity mass = mass1 + mass2;
  const MassQuantity massDelta = mass1 - mass2;
  const EnergyQuantity energy1 = 0.5 * (mass * (pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()) +
                                                pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>()) +
                                                pow<2>(radius) * pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                                        / Quantity<double, Angle>(1.0)
#endif
                                                                            )) +
                                        2.0 * radius * massDelta * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                            / Quantity<double, Angle>(1.0)
#endif
                                            * (cos(relativeAngle) * minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>() -
                                               sin(relativeAngle) * minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()));

  auto massMatrix00 = mass;
  auto massMatrix01 = MassQuantity(0.0);
  auto massMatrix02 = -radius * massDelta * sin(relativeAngle)
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                      / Quantity<double, Angle>(1.0)
#endif
      ;
  auto massMatrix12 = radius * massDelta * cos(relativeAngle)
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                      / Quantity<double, Angle>(1.0)
#endif
      ;
  auto massMatrix22 = pow<2>(radius) * mass
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                      / Quantity<double, Angle>(1.0) / Quantity<double, Angle>(1.0)
#endif
      ;
  const MassMatrix massMatrix(massMatrix00, massMatrix01, massMatrix02, massMatrix01, massMatrix00, massMatrix12, massMatrix02,
                              massMatrix12, massMatrix22);

  // TODO(remod): Enable implicit conversion.
  const EnergyQuantity energy2 =
      0.5 * (minimalCoordinatesDt.transpose() * massMatrix * minimalCoordinatesDt).getQuantityMatrix().getQuantity<0, 0>();

  EXPECT_EQ(energy1, energy2);
}
