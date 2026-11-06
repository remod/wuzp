#include <cmath>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

template <int Rows, int Cols>
using TestValues = ValueMatrix<double, Rows, Cols>;
template <int Rows, int Cols>
using TestUnitMatrix = TypeMatrix<Rows, Cols, Length>;
template <int Rows, int Cols>
using TestQuantities = Quantities<TestValues<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;
template <int Rows, int Cols>
using TestTensor = Tensor<1, 0, TestQuantities<Rows, Cols>, CoordinateSystemA, Relative>;

using LengthQuantityI = QuantityI<Length>;
using SquaredLengthQuantityI = QuantityI<Power<Length, Integer<2>>::type>;
using MassQuantityI = QuantityI<Mass>;
using AreaQuantityI = QuantityI<Area>;
// The unit of a length times a force. Boost.Units defines torque as N m / rad, so there it is a torque times an angle.
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
using LengthTimesForce = Product<Torque, Angle>::type;
#else
using LengthTimesForce = Torque;
#endif

template <typename TensorT>
using UnitOf = typename GetType<typename TensorT::UnitsT>::type;

TEST(TestTensor, Zero) {  // NOLINT
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> expected{0.0, 0.0, 0.0};
  EXPECT_EQ((PositionVector3<ValueMatrix, double, CoordinateSystemA>::zero()), expected);
}

TEST(TestTensor, Random) {  // NOLINT
  const auto vector = PositionVector3<ValueMatrix, double, CoordinateSystemA>::random();
  EXPECT_LE(vector.norm().getValue(), std::sqrt(3.0));
}

// Every named tensor alias has the unit its name promises.
TEST(TestTensor, UnitsOfAliases) {  // NOLINT
  using CS = CoordinateSystemA;
  EXPECT_TRUE((std::is_same_v<UnitOf<DirectionVector3<ValueMatrix, double, CS>>, Dimensionless>));
  EXPECT_TRUE((std::is_same_v<UnitOf<PositionPoint3<ValueMatrix, double, CS>>, Length>));
  EXPECT_TRUE((std::is_same_v<UnitOf<PositionVector3<ValueMatrix, double, CS>>, Length>));
  EXPECT_TRUE((std::is_same_v<UnitOf<VelocityVector3<ValueMatrix, double, CS>>, Velocity>));
  EXPECT_TRUE((std::is_same_v<UnitOf<AccelerationVector3<ValueMatrix, double, CS>>, Acceleration>));
  EXPECT_TRUE((std::is_same_v<UnitOf<AngularVelocityVector3<ValueMatrix, double, CS>>, AngularVelocity>));
  EXPECT_TRUE((std::is_same_v<UnitOf<AngularAccelerationVector3<ValueMatrix, double, CS>>, AngularAcceleration>));
  EXPECT_TRUE((std::is_same_v<UnitOf<ForceVector3<ValueMatrix, double, CS>>, Force>));
  EXPECT_TRUE((std::is_same_v<UnitOf<TorqueVector3<ValueMatrix, double, CS>>, Torque>));
  EXPECT_TRUE((std::is_same_v<UnitOf<TimePoint<ValueMatrix, double, CS>>, Time>));
  EXPECT_TRUE((std::is_same_v<UnitOf<Duration<ValueMatrix, double, CS>>, Time>));
  EXPECT_TRUE((std::is_same_v<UnitOf<SpacetimePoint4<ValueMatrix, double, CS>>, Length>));
  EXPECT_TRUE((std::is_same_v<UnitOf<SpacetimeVector4<ValueMatrix, double, CS>>, Length>));
}

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
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  static_assert(std::is_same_v<GetTag<MyTensor, MyTensor>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyTensor, const MyTensor&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyTensor&&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyQuantityMatrix>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, QuantityD<Length>>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, QuantityD<Length>, QuantityD<Length>>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyValueMatrix>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, double>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, double, int>::type, FromImplementationArgs>);
}

TEST(TestTensor2, EmptyConstructor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 1, MyQuantityMatrix, CoordinateSystemA, Relative>;
  [[maybe_unused]] const MyTensor tensor;
}

TEST(TestTensor2, ConstructImplicitlyFromTensor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyTensor tensor1{1, 2, 3, 4, 5, 6};
  const MyTensor tensor2{6, 5, 4, 3, 2, 1};
  // Use = instead of {} to trigger implicit conversion.
  const MyTensor tensor3 = tensor1 + tensor2;
  const MyTensor tensor = tensor3;
  EXPECT_EQ(tensor, tensor3);
  EXPECT_EQ(tensor, (MyTensor{7, 7, 7, 7, 7, 7}));
}

TEST(TestTensor2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyQuantityMatrix quantities{1, 2, 3, 4, 5, 6};
  const MyTensor tensor{quantities};
  EXPECT_EQ(tensor.getQuantities(), quantities);
}

TEST(TestTensor2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  const MyTensor tensor{values};
  EXPECT_EQ(tensor.getValues(), values);
}

TEST(TestTensor2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  const MyTensor tensor{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(tensor.getValues(), values);
}

class TestTensorF : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;

  MyTensor& getTensor() { return tensor_; }

 private:
  MyTensor tensor_{1, 2, 3, 4, 5, 6};
};

class TestHomogeneousTensorF : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;

  MyTensor& getTensor() { return tensor_; }

 private:
  MyTensor tensor_{1, 2, 3, 4, 5, 6};
};

TEST_F(TestTensorF, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getTensor(), getTensor());
}

TEST_F(TestTensorF, StreamOperator) {  // NOLINT
  EXPECT_EQ(toString(getTensor()), toString(getTensor().getQuantities()));
}

TEST_F(TestTensorF, GetQuantityCompileTime) {  // NOLINT
  EXPECT_EQ((getTensor().getQuantity<0, 0>()), LengthQuantityI{1});
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI{4});
}

TEST_F(TestHomogeneousTensorF, GetQuantityRunTime) {  // NOLINT
  EXPECT_EQ(getTensor().getQuantity(0, 0), LengthQuantityI{1});
  EXPECT_EQ(getTensor().getQuantity(1, 0), LengthQuantityI{4});
}

TEST_F(TestHomogeneousTensorF, SetQuantityCompileTime) {  // NOLINT
  getTensor().setQuantity(1, 0, LengthQuantityI{1});
  EXPECT_EQ(getTensor().getQuantity(1, 0), LengthQuantityI{1});
}

TEST_F(TestTensorF, SetQuantityRunTime) {  // NOLINT
  getTensor().setQuantity<1, 0>(MassQuantityI{1});
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI{1});
}

TEST_F(TestTensorF, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = Quantities<MyValueMatrixB, MyUnitMatrixB>;
  using MyTensorB = Tensor<1, 0, MyQuantityMatrixB, CoordinateSystemA, Relative>;
  const MyTensorB tensorB{2, 3, 5, 6};
  EXPECT_EQ((getTensor().block<0, 1, 2, 2>()), tensorB);
}

TEST_F(TestTensorF, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = Quantities<MyValueMatrixT, MyUnitMatrixT>;
  using MyTensorT = Tensor<1, 0, MyQuantityMatrixT, CoordinateSystemA, Relative>;
  const MyTensorT tensorT{1, 4, 2, 5, 3, 6};
  EXPECT_EQ(getTensor().transpose(), tensorT);
}

TEST_F(TestTensorF, Sum) {  // NOLINT
  const auto result{getTensor().getQuantities() + getTensor().getQuantities()};
  EXPECT_EQ(sum(getTensor(), getTensor()).getQuantities(), result);
  EXPECT_EQ((getTensor() + getTensor()).getQuantities(), result);
}

TEST_F(TestTensorF, SumRelativeAndAbsolute) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor{1, 4, 2, 5, 3, 6};
  const auto result{getTensor().getQuantities() + tensor.getQuantities()};
  EXPECT_EQ(sum(getTensor(), tensor).getQuantities(), result);
  EXPECT_EQ((getTensor() + tensor).getQuantities(), result);
}

TEST_F(TestTensorF, SumAbsoluteAndRelative) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor{1, 4, 2, 5, 3, 6};
  const auto result{getTensor().getQuantities() + tensor.getQuantities()};
  EXPECT_EQ(sum(tensor, getTensor()).getQuantities(), result);
  EXPECT_EQ((tensor + getTensor()).getQuantities(), result);
}

TEST_F(TestTensorF, Difference) {  // NOLINT
  const auto result{getTensor().getQuantities() - getTensor().getQuantities()};
  EXPECT_EQ(difference(getTensor(), getTensor()).getQuantities(), result);
  EXPECT_EQ((getTensor() - getTensor()).getQuantities(), result);
}

TEST_F(TestTensorF, DifferenceAbsoluteAndRelative) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor{1, 4, 2, 5, 3, 6};
  const auto result{tensor.getQuantities() - getTensor().getQuantities()};
  EXPECT_EQ(difference(tensor, getTensor()).getQuantities(), result);
  EXPECT_EQ((tensor - getTensor()).getQuantities(), result);
}

TEST_F(TestTensorF, DifferenceAbsoluteAndAbsolute) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor{1, 4, 2, 5, 3, 6};
  const auto result{tensor.getQuantities() - tensor.getQuantities()};
  EXPECT_EQ(difference(tensor, tensor).getQuantities(), result);
  EXPECT_EQ((tensor - tensor).getQuantities(), result);
}

TEST(TestTensor2, ProductWithBuiltinValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values * value};
  EXPECT_EQ(product(tensor, value), result);
  EXPECT_EQ(product(value, tensor), result);
  EXPECT_EQ(tensor * value, result);
  EXPECT_EQ(value * tensor, result);
}

TEST(TestTensor2, ProductWithValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const ValueI value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values * value};
  EXPECT_EQ(product(tensor, value), result);
  EXPECT_EQ(product(value, tensor), result);
  EXPECT_EQ(tensor * value, result);
  EXPECT_EQ(value * tensor, result);
}

TEST(TestTensor2, ProductWithQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyValue value{2};
  const LengthQuantityI quantity{value};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values * value};
  EXPECT_EQ(product(tensor, quantity), result);
  EXPECT_EQ(product(quantity, tensor), result);
  EXPECT_EQ(tensor * quantity, result);
  EXPECT_EQ(quantity * tensor, result);
}

TEST(TestTensor2, ProductWithQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyValue value{2};
  const MyQuantityMatrix quantities{value, value, value, value};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values * value};
  EXPECT_EQ(product(tensor, quantities), result);
  EXPECT_EQ(product(quantities, tensor), result);
  EXPECT_EQ(tensor * quantities, result);
  EXPECT_EQ(quantities * tensor, result);
}

TEST(TestTensor2, ProductWith1x1Tensor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValueMatrix1x1 = ValueMatrix<int, 1, 1>;
  using MyQuantity = Quantity<MyValueMatrix1x1, Length>;
  using MyTensor1x1 = Tensor<1, 0, MyQuantity, CoordinateSystemA, Relative>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  using MyValue = int;
  const MyValue value{2};
  const LengthQuantityI quantity{value};
  const MyTensor1x1 tensor1x1{quantity};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values * value};
  EXPECT_EQ(product(tensor, tensor1x1), result);
  EXPECT_EQ(product(tensor1x1, tensor), result);
  EXPECT_EQ(tensor * tensor1x1, result);
  EXPECT_EQ(tensor1x1 * tensor, result);
}

TEST(TestTensor2, ProductWithTensor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 1, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 1, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values * values};
  EXPECT_EQ(product(tensor, tensor), result);
  EXPECT_EQ(tensor * tensor, result);
}

TEST(TestTensor2, InnerProduct) {  // NOLINT
  using ValueMatrixLhs = ValueMatrix<int, 3, 2>;
  using UnitMatrixLhs = TypeMatrix<3, 2, Length>;
  using QuantityMatrixLhs = Quantities<ValueMatrixLhs, UnitMatrixLhs>;
  using TensorLhs = Tensor<1, 1, QuantityMatrixLhs, CoordinateSystemA, Relative>;
  using ValueMatrixRhs = ValueMatrix<int, 2, 1>;
  using UnitMatrixRhs = TypeMatrix<2, 1, Force>;
  using QuantityMatrixRhs = Quantities<ValueMatrixRhs, UnitMatrixRhs>;
  using TensorRhs = Tensor<1, 0, QuantityMatrixRhs, CoordinateSystemA, Relative>;
  using ValueMatrixResult = ValueMatrix<int, 3, 1>;
  using UnitMatrixResult = TypeMatrix<3, 1, LengthTimesForce>;
  using QuantityMatrixResult = Quantities<ValueMatrixResult, UnitMatrixResult>;
  using TensorResult = Tensor<1, 0, QuantityMatrixResult, CoordinateSystemA, Relative>;
  const TensorLhs tensorLhs{1, 2, 3, 4, 5, 6};
  const TensorRhs tensorRhs{-2, -1};
  const TensorResult tensorResult{-4, -10, -16};
  EXPECT_EQ(innerProduct(tensorLhs, tensorRhs).evaluate(), tensorResult);
}

TEST(TestTensor2, OuterProductOfScalars) {  // NOLINT
  using ValueMatrixLhs = ValueMatrix<int, 1, 1>;
  using UnitMatrixLhs = TypeMatrix<1, 1, Force>;
  using QuantityMatrixLhs = Quantities<ValueMatrixLhs, UnitMatrixLhs>;
  using TensorLhs = Tensor<0, 0, QuantityMatrixLhs, CoordinateSystemA, Relative>;
  using ValueMatrixRhs = ValueMatrix<int, 1, 1>;
  using UnitMatrixRhs = TypeMatrix<1, 1, Length>;
  using QuantityMatrixRhs = Quantities<ValueMatrixRhs, UnitMatrixRhs>;
  using TensorRhs = Tensor<0, 0, QuantityMatrixRhs, CoordinateSystemA, Relative>;
  using ValueMatrixResult = ValueMatrix<int, 1, 1>;
  using UnitMatrixResult = TypeMatrix<1, 1, LengthTimesForce>;
  using QuantityMatrixResult = Quantities<ValueMatrixResult, UnitMatrixResult>;
  using TensorResult = Tensor<0, 0, QuantityMatrixResult, CoordinateSystemA, Relative>;
  const TensorLhs tensorLhs{2};
  const TensorRhs tensorRhs{3};
  const TensorResult tensorResult{6};
  EXPECT_EQ(outerProduct(tensorLhs, tensorRhs), tensorResult);
}

TEST(TestTensor2, OuterProductOfCovectorAndVector) {  // NOLINT
  using ValueMatrixLhs = ValueMatrix<int, 1, 2>;
  using UnitMatrixLhs = TypeMatrix<1, 2, Force>;
  using QuantityMatrixLhs = Quantities<ValueMatrixLhs, UnitMatrixLhs>;
  using TensorLhs = Tensor<1, 0, QuantityMatrixLhs, CoordinateSystemA, Relative>;
  using ValueMatrixRhs = ValueMatrix<int, 3, 1>;
  using UnitMatrixRhs = TypeMatrix<3, 1, Length>;
  using QuantityMatrixRhs = Quantities<ValueMatrixRhs, UnitMatrixRhs>;
  using TensorRhs = Tensor<0, 1, QuantityMatrixRhs, CoordinateSystemA, Relative>;
  using ValueMatrixResult = ValueMatrix<int, 3, 2>;
  using UnitMatrixResult = TypeMatrix<3, 2, LengthTimesForce>;
  using QuantityMatrixResult = Quantities<ValueMatrixResult, UnitMatrixResult>;
  using TensorResult = Tensor<1, 1, QuantityMatrixResult, CoordinateSystemA, Relative>;
  const TensorLhs tensorLhs{1, 2};
  const TensorRhs tensorRhs{-1, 3, 4};
  const TensorResult tensorResult{-1, -2, 3, 6, 4, 8};
  EXPECT_EQ(outerProduct(tensorLhs, tensorRhs), tensorResult);
}

TEST(TestTensor2, OuterProductOfVectorAndCovector) {  // NOLINT
  using ValueMatrixLhs = ValueMatrix<int, 3, 1>;
  using UnitMatrixLhs = TypeMatrix<3, 1, Force>;
  using QuantityMatrixLhs = Quantities<ValueMatrixLhs, UnitMatrixLhs>;
  using TensorLhs = Tensor<1, 0, QuantityMatrixLhs, CoordinateSystemA, Relative>;
  using ValueMatrixRhs = ValueMatrix<int, 1, 2>;
  using UnitMatrixRhs = TypeMatrix<1, 2, Length>;
  using QuantityMatrixRhs = Quantities<ValueMatrixRhs, UnitMatrixRhs>;
  using TensorRhs = Tensor<0, 1, QuantityMatrixRhs, CoordinateSystemA, Relative>;
  using ValueMatrixResult = ValueMatrix<int, 3, 2>;
  using UnitMatrixResult = TypeMatrix<3, 2, LengthTimesForce>;
  using QuantityMatrixResult = Quantities<ValueMatrixResult, UnitMatrixResult>;
  using TensorResult = Tensor<1, 1, QuantityMatrixResult, CoordinateSystemA, Relative>;
  const TensorLhs tensorLhs{-1, 3, 4};
  const TensorRhs tensorRhs{1, 2};
  const TensorResult tensorResult{-1, -2, 3, 6, 4, 8};
  EXPECT_EQ(outerProduct(tensorLhs, tensorRhs), tensorResult);
}

TEST(TestTensor2, OuterProductOfScalarAndMatrix) {  // NOLINT
  using ValueMatrixLhs = ValueMatrix<int, 1, 1>;
  using UnitMatrixLhs = TypeMatrix<1, 1, Force>;
  using QuantityMatrixLhs = Quantities<ValueMatrixLhs, UnitMatrixLhs>;
  using TensorLhs = Tensor<0, 0, QuantityMatrixLhs, CoordinateSystemA, Relative>;
  using ValueMatrixRhs = ValueMatrix<int, 2, 2>;
  using UnitMatrixRhs = TypeMatrix<2, 2, Length>;
  using QuantityMatrixRhs = Quantities<ValueMatrixRhs, UnitMatrixRhs>;
  using TensorRhs = Tensor<1, 1, QuantityMatrixRhs, CoordinateSystemA, Relative>;
  using ValueMatrixResult = ValueMatrix<int, 2, 2>;
  using UnitMatrixResult = TypeMatrix<2, 2, LengthTimesForce>;
  using QuantityMatrixResult = Quantities<ValueMatrixResult, UnitMatrixResult>;
  using TensorResult = Tensor<1, 1, QuantityMatrixResult, CoordinateSystemA, Relative>;
  const TensorLhs tensorLhs{2};
  const TensorRhs tensorRhs{-1, 3, 4, 2};
  const TensorResult tensorResult{-2, 6, 8, 4};
  EXPECT_EQ(outerProduct(tensorLhs, tensorRhs), tensorResult);
}

TEST(TestTensor2, DivisionWithBuiltinValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values / value};
  EXPECT_EQ(division(tensor, value), result);
  EXPECT_EQ(tensor / value, result);
}

TEST(TestTensor2, DivisionWithValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const ValueI value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values / value};
  EXPECT_EQ(division(tensor, value), result);
  EXPECT_EQ(tensor / value, result);
}

TEST(TestTensor2, DivisionWithQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyValue value{2};
  const LengthQuantityI quantity{value};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values / value};
  EXPECT_EQ(division(tensor, quantity), result);
  EXPECT_EQ(tensor / quantity, result);
}

TEST(TestTensor2, DivisionWith1x1Tensor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValueMatrix1x1 = ValueMatrix<int, 1, 1>;
  using MyQuantity = Quantity<MyValueMatrix1x1, Length>;
  using MyTensor1x1 = Tensor<1, 0, MyQuantity, CoordinateSystemA, Relative>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const int value{2};
  const MyTensor1x1 tensor1x1{value};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values / value};
  EXPECT_EQ(division(tensor, tensor1x1), result);
  EXPECT_EQ(tensor / tensor1x1, result);
}

TEST(TestTensor2, DotProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyTensor lhs{1, 2, 3, 4};
  const MyTensor rhs{0, 3, -1, 2};
  const AreaQuantityI result{11};
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestTensor2, CrossProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  using MyUnitMatrix = TypeMatrix<3, 1, Length>;
  using MyUnitMatrixResult = TypeMatrix<3, 1, Area>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyTensor lhs{1, 2, 3};
  const MyTensor rhs{-1, 3, 4};
  const MyTensorResult result{-1, -7, 5};
  EXPECT_EQ(lhs.crossProduct(rhs), result);
}

TEST(TestTensor2, Norm) {  // NOLINT
  const PositionVector3<ValueMatrix, int, CoordinateSystemA> vector{3, 0, 4};
  EXPECT_EQ(vector.norm(), LengthQuantityI{5});
}

TEST(TestTensor2, SquaredNorm) {  // NOLINT
  const PositionVector3<ValueMatrix, int, CoordinateSystemA> vector{3, 0, 4};
  EXPECT_EQ(vector.squaredNorm(), SquaredLengthQuantityI{25});
}

// The Blitz++ backend implements neither inverses nor determinants.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestTensor2, Determinant) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 1, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyTensor tensor{-1, 2, 3, 1};
  EXPECT_EQ(tensor.determinant(), AreaQuantityI{-7});
}
#endif
