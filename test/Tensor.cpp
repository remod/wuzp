#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Quantity.hpp"
#include "wuzp/units/wuzp/Unit.hpp"
template <typename Scalar, int Rows, int Cols>
using Quantity = wuzp::Quantity<Scalar, Rows, Cols>;
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
template <int Rows, int Cols>
using TestTensor = Tensor<1, 0, TestCoordinateMatrix<Rows, Cols>>;

TEST(TestTensor2, Dimension) {  // NOLINT
  static_assert(Dimension<TestTensor<1, 1>>::value == 0);
  static_assert(Dimension<TestTensor<3, 1>>::value == 1);
  static_assert(Dimension<TestTensor<1, 2>>::value == 1);
  static_assert(Dimension<TestTensor<3, 2>>::value == 2);
}

TEST(TestTensor2, Rows) {  // NOLINT
  static_assert(Rows<TestTensor<3, 2>>::value == 3);
}

TEST(TestTensor2, Cols) {  // NOLINT
  static_assert(Cols<TestTensor<3, 2>>::value == 2);
}

TEST(TestTensor2, GetTag) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC, CoordinateSystemD,
                                                          CoordinateSystemE, CoordinateSystemF>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;
  static_assert(std::is_same_v<GetTag<MyTensor, MyTensor>::type, FromTensor>);
  static_assert(std::is_same_v<GetTag<MyTensor, const MyTensor&>::type, FromTensor>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyTensor&&>::type, FromTensor>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyCoordinateMatrix>::type, FromCoordinateMatrix>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyQuantityMatrix>::type, FromCoordinateMatrixArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, Quantity<double, Length>>::type, FromCoordinateMatrixArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyValueMatrix>::type, FromCoordinateMatrixArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, double>::type, FromCoordinateMatrixArgs>);
}

TEST(TestTensor2, EmptyConstructor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  [[maybe_unused]] const MyCoordinateMatrix coordinateMatrix;
}

TEST(TestTensor2, ConstructImplicitlyFromTensor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;
  const MyTensor tensor1;
  const MyTensor tensor2;
  const auto tensor3 = tensor1 + tensor2;
  const MyTensor tensor = tensor3;
  EXPECT_EQ(tensor, tensor3);
}

TEST(TestTensor2, ConstructImplicitlyFromCoordinateMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/Tensor.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Unit.hpp"
using namespace wuzp_unit;  // NOLINT
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
using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;

int main() {
  const MyCoordinateMatrix coordinateMatrix(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyTensor tensor = coordinateMatrix;
  return 0;
}
)",
                                        "conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromCoordinateMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;
  const MyCoordinateMatrix coordinateMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(coordinateMatrix);
  EXPECT_EQ(tensor.getCoordinateMatrix(), coordinateMatrix);
}

TEST(TestTensor2, ConstructImplicitlyFromQuantityMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Unit.hpp"
using namespace wuzp_unit;  // NOLINT
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
using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;

int main() {
  MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyTensor tensor = quantityMatrix;
  return 0;
}
)",
                                        "conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromQuantityMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;
  const MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(quantityMatrix);
  EXPECT_EQ(tensor.getQuantityMatrix(), quantityMatrix);
}

TEST(TestTensor2, ConstructImplicitlyFromValueMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Unit.hpp"
using namespace wuzp_unit;  // NOLINT
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
using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;

int main() {
  MyValueMatrix valueMatrix;
  valueMatrix << 1, 2, 3, 4, 5, 6;
  [[maybe_unused]] const MyTensor tensor = valueMatrix;
  return 0;
}
)",
                                        "conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromValueMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(valueMatrix);
  EXPECT_EQ(tensor.getValueMatrix(), valueMatrix);
}

TEST(TestTensor2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(tensor.getValueMatrix(), valueMatrix);
}

class TestTensorF : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC, CoordinateSystemD,
                                                          CoordinateSystemE, CoordinateSystemF>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyTensor = Tensor<1, 0, MyCoordinateMatrix>;

  const MyTensor& getTensor() const { return tensor_; }

 private:
  MyTensor tensor_{1, 2, 3, 4, 5, 6};
};

TEST_F(TestTensorF, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getTensor(), getTensor());
}

TEST_F(TestTensorF, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = QuantityMatrix<MyValueMatrixB, MyUnitMatrixB>;
  using MyCoordinateSystemMatrixB =
      TypeMatrix<2, 2, CoordinateSystemB, CoordinateSystemC, CoordinateSystemE, CoordinateSystemF>;
  using MyCoordinateMatrixB = CoordinateMatrix<MyQuantityMatrixB, MyCoordinateSystemMatrixB>;
  using MyTensorB = Tensor<1, 0, MyCoordinateMatrixB>;
  MyTensorB tensorB(2, 3, 5, 6);
  EXPECT_EQ((getTensor().block<0, 1, 2, 2>()), tensorB);
}

TEST_F(TestTensorF, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = QuantityMatrix<MyValueMatrixT, MyUnitMatrixT>;
  using MyCoordinateSystemMatrixT = TypeMatrix<3, 2, CoordinateSystemA, CoordinateSystemD, CoordinateSystemB, CoordinateSystemE,
                                                           CoordinateSystemC, CoordinateSystemF>;
  using MyCoordinateMatrixT = CoordinateMatrix<MyQuantityMatrixT, MyCoordinateSystemMatrixT>;
  using MyTensorT = Tensor<0, 1, MyCoordinateMatrixT>;
  const MyTensorT tensorT(1, 4, 2, 5, 3, 6);
  EXPECT_EQ(getTensor().transpose(), tensorT);
}

// TEST(TestTensor2, ProductCoordinateMatrix) {  // NOLINT
//  using MyValueMatrix = ValueMatrix<int, 2, 2>;
//  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
//  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
//  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
//  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
//  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
//  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
//  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
//  MyValueMatrix valueMatrix;
//  valueMatrix << 1, 2, 3, 4;
//  MyCoordinateMatrix coordinateMatrix;
//  coordinateMatrix.setValueMatrix(valueMatrix);
//  EXPECT_EQ(coordinateMatrix * coordinateMatrix, MyCoordinateMatrixResult(valueMatrix * valueMatrix));
//  // OLD:
//  //  using MyValueMatrix = ValueMatrix<int, 2, 2>;
//  //  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
//  //  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
//  //  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
//  //  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
//  //  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
//  //  MyValueMatrix valueMatrix;
//  //  valueMatrix << 1, 2, 3, 4;
//  //  MyCoordinateMatrix coordinateMatrix;
//  //  coordinateMatrix.setValueMatrix(valueMatrix);
//  //  EXPECT_EQ(coordinateMatrix * coordinateMatrix, MyQuantityMatrixResult(valueMatrix * valueMatrix));
//}
//
// TEST(TestTensor2, ProductQuantity) {  // NOLINT
//  using MyValueMatrix = ValueMatrix<int, 2, 2>;
//  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
//  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
//  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
//  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
//  using MyValue = int;
//  using MyQuantity = Quantity<MyValue, Length>;
//  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
//  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
//  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
//  const MyValue value{2};
//  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
//  MyValueMatrix valueMatrix;
//  valueMatrix << 1, 2, 3, 4;
//  MyCoordinateMatrix coordinateMatrix;
//  coordinateMatrix.setValueMatrix(valueMatrix);
//  EXPECT_EQ(coordinateMatrix * quantity, MyCoordinateMatrixResult(valueMatrix * value));
//  EXPECT_EQ(quantity * coordinateMatrix, MyCoordinateMatrixResult(valueMatrix * value));
//}
//
// TEST(TestTensor2, DivisionQuantity) {  // NOLINT
//  using MyValueMatrix = ValueMatrix<int, 2, 2>;
//  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
//  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
//  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
//  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
//  using MyValue = int;
//  using MyQuantity = Quantity<MyValue, Length>;
//  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
//  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
//  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
//  const MyValue value{2};
//  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
//  MyValueMatrix valueMatrix;
//  valueMatrix << 1, 2, 3, 4;
//  MyCoordinateMatrix coordinateMatrix;
//  coordinateMatrix.setValueMatrix(valueMatrix);
//  EXPECT_EQ(coordinateMatrix / quantity, MyCoordinateMatrixResult(valueMatrix / value));
//}
//
// TEST(TestTensor2, Dot) {  // NOLINT
//  using MyValueMatrix = ValueMatrix<int, 4, 1>;
//  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
//  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
//  using MyCoordinateSystemMatrix = TypeMatrix<4, 1, CoordinateSystemA, CoordinateSystemB, CoordinateSystemC,
//  CoordinateSystemD>; using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>; using MyQuantityResult =
//  Quantity<int, Area>; MyValueMatrix valueMatrix1; valueMatrix1 << 1, 2, 3, 4; MyValueMatrix valueMatrix2; valueMatrix2 << 0, 3, -1, 2;
//  EXPECT_EQ(MyCoordinateMatrix(valueMatrix1).dot(MyCoordinateMatrix(valueMatrix2)), QuantityTraits::Create<MyQuantityResult>::create(11));
//}
//
// TEST(TestTensor2, RotatingDumbbell) {  // NOLINT
//  // From https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell
//  struct CartesianCoordinateSystemName {};
//  using CartesianCoordinateSystem = CoordinateSystem<CartesianCoordinateSystemName>;
//
//  using MinimalCoordinatesValueMatrix = ValueMatrix<double, 3, 1>;
//  using MinimalCoordinatesUnitMatrix = TypeMatrix<3, 1, Absolute<Length>, Absolute<Length>, Angle>;
//  using MinimalCoordinatesQuantityMatrix = QuantityMatrix<MinimalCoordinatesValueMatrix, MinimalCoordinatesUnitMatrix>;
//  using MinimalCoordinatesDtUnitMatrix = TypeMatrix<3, 1, Velocity, Velocity, AngularVelocity>;
//  using MinimalCoordinatesDtQuantityMatrix = QuantityMatrix<MinimalCoordinatesValueMatrix, MinimalCoordinatesDtUnitMatrix>;
//  using MinimalCoordinatesCoordinateSystemMatrix =
//      TypeMatrix<3, 1, CartesianCoordinateSystem, CartesianCoordinateSystem, NoCoordinateSystem>;
//  using MinimalCoordinates = CoordinateMatrix<MinimalCoordinatesQuantityMatrix, MinimalCoordinatesCoordinateSystemMatrix>;
//  using MinimalCoordinatesDt = CoordinateMatrix<MinimalCoordinatesDtQuantityMatrix, MinimalCoordinatesCoordinateSystemMatrix>;
//
//  using PointValueMatrix = ValueMatrix<double, 2, 1>;
//  using PointUnitMatrix = TypeMatrix<2, 1, Absolute<Length>>;
//  using DirectionUnitMatrix = TypeMatrix<2, 1, Dimensionless>;
//  using VelocityUnitMatrix = TypeMatrix<2, 1, Velocity>;
//  using PointQuantityMatrix = QuantityMatrix<PointValueMatrix, PointUnitMatrix>;
//  using DirectionQuantityMatrix = QuantityMatrix<PointValueMatrix, DirectionUnitMatrix>;
//  using VelocityQuantityMatrix = QuantityMatrix<PointValueMatrix, VelocityUnitMatrix>;
//  using PointCoordinateSystemMatrix = TypeMatrix<2, 1, CartesianCoordinateSystem>;
//  using Point = CoordinateMatrix<PointQuantityMatrix, PointCoordinateSystemMatrix>;
//  using Direction = CoordinateMatrix<DirectionQuantityMatrix, PointCoordinateSystemMatrix>;
//  using Velocity_ = CoordinateMatrix<VelocityQuantityMatrix, PointCoordinateSystemMatrix>;
//
//  using Radius = Quantity<double, Length>;
//  using Mass_ = Quantity<double, Mass>;
//  using Energy_ = Quantity<double, Energy>;
//
//  using MassMatrixValues = ValueMatrix<double, 3, 3>;
//  using MassLength = UnitTraits::Product<Mass, Length>::type;
//  using MassLength2 = UnitTraits::Product<MassLength, Length>::type;
//  using MassMatrixUnits = TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
//  using MassMatrix = QuantityMatrix<MassMatrixValues, MassMatrixUnits>;
//
//  const MinimalCoordinates minimalCoordinates(MinimalCoordinatesValueMatrix::Random());
//  const MinimalCoordinatesDt minimalCoordinatesDt(MinimalCoordinatesValueMatrix::Random());
//  const Radius radius = QuantityTraits::Create<Radius>::create(2.0);
//  const Mass_ mass1 = QuantityTraits::Create<Mass_>::create(1.0);
//  const Mass_ mass2 = QuantityTraits::Create<Mass_>::create(1.0);
//
//  // TODO(remod): Introduce CoordinateMatrix::getQuantity()?
//  const auto positionVector = radius * Direction(cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()),
//                                                 sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()));
//  const Point position1 = minimalCoordinates.block<0, 0, 2, 1>() + positionVector;
//  const Point position2 = minimalCoordinates.block<0, 0, 2, 1>() - positionVector;
//  const auto velocityVector = radius * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>() *
//                              Direction(-sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()),
//                                        cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()))
//#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
//                              / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
//#endif
//      ;
//  const Velocity_ velocity1 = minimalCoordinatesDt.block<0, 0, 2, 1>() + velocityVector;
//  const Velocity_ velocity2 = minimalCoordinatesDt.block<0, 0, 2, 1>() - velocityVector;
//
//  const Mass_ mass = mass1 + mass2;
//  const Mass_ massDelta = mass1 - mass2;
//  const Energy_ energy1 = 0.5 * (mass * (pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()) +
//                                         pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>()) +
//                                         pow<2>(radius) * pow<2>(minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
//#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
//                                                                 / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
//#endif
//                                                                     )) +
//                                 2.0 * radius * massDelta * minimalCoordinatesDt.getQuantityMatrix().getQuantity<2, 0>()
//#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
//                                     / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
//#endif
//                                     * (cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()) *
//                                            minimalCoordinatesDt.getQuantityMatrix().getQuantity<1, 0>() -
//                                        sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>()) *
//                                            minimalCoordinatesDt.getQuantityMatrix().getQuantity<0, 0>()));
//
//  // TODO(remod): Can numbers be implicitly converted to dimensionless quantities?
//  const auto massMatrix00 = mass;
//  const auto massMatrix01 = QuantityTraits::Create<Mass_>::create(0.0);
//  const auto massMatrix02 = -radius * massDelta * sin(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
//  const auto massMatrix12 = radius * massDelta * cos(minimalCoordinates.getQuantityMatrix().getQuantity<2, 0>());
//  const auto massMatrix22 = pow<2>(radius) * mass;
//  MassMatrix massMatrix;
//  massMatrix.setQuantity<0, 0>(massMatrix00);
//  massMatrix.setQuantity<0, 1>(massMatrix01);
//  massMatrix.setQuantity<0, 2>(massMatrix02);
//  massMatrix.setQuantity<1, 0>(massMatrix01);
//  massMatrix.setQuantity<1, 1>(massMatrix00);
//  massMatrix.setQuantity<1, 2>(massMatrix12);
//  massMatrix.setQuantity<2, 0>(massMatrix02);
//  massMatrix.setQuantity<2, 1>(massMatrix12);
//  massMatrix.setQuantity<2, 2>(massMatrix22);
//
//  //  const Energy_ energy2 = 0.5 * minimalCoordinatesDt.transpose() * massMatrix * minimalCoordinatesDt;
//
//  //  EXPECT_EQ(energy1, energy2);
//}
