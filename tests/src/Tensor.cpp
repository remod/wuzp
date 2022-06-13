#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

#include "coordinate_systems.hpp"
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

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;

template <int Rows, int Cols>
using TestValueMatrix = ValueMatrix<double, Rows, Cols>;
template <int Rows, int Cols>
using TestUnitMatrix = TypeMatrix<Rows, Cols, Length>;
template <int Rows, int Cols>
using TestQuantityMatrix = Quantities<TestValueMatrix<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;
template <int Rows, int Cols>
using TestTensor = Tensor<1, 0, TestQuantityMatrix<Rows, Cols>, CoordinateSystemA>;

using LengthQuantityI = QuantityI<Length>;
using MassQuantityI = QuantityI<Mass>;
using AreaQuantityI = QuantityI<Area>;

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
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
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
  using MyTensor = Tensor<1, 1, MyQuantityMatrix, CoordinateSystemA>;
  [[maybe_unused]] const MyTensor tensor;
}

TEST(TestTensor2, ConstructImplicitlyFromTensor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  const MyTensor tensor1;
  const MyTensor tensor2;
  const auto tensor3 = tensor1 + tensor2;
  const MyTensor tensor = tensor3;
  EXPECT_EQ(tensor, tensor3);
}

TEST(TestTensor2, ConstructImplicitlyFromQuantityMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;

int main() {
  MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyTensor tensor = quantityMatrix;
  return 0;
}
)",
                                        "(no viable )?conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromQuantityMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  const MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(quantityMatrix);
  EXPECT_EQ(tensor.getQuantities(), quantityMatrix);
}

TEST(TestTensor2, ConstructImplicitlyFromValues) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;

int main() {
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyTensor tensor = valueMatrix;
  return 0;
}
)",
                                        "(no viable )?conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromValueMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(valueMatrix);
  EXPECT_EQ(tensor.getValues(), valueMatrix);
}

TEST(TestTensor2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(tensor.getValues(), valueMatrix);
}

class TestTensorF : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyHomogeneousUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyHomogeneousQuantityMatrix = Quantities<MyValueMatrix, MyHomogeneousUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using MyHomogeneousTensor = Tensor<1, 0, MyHomogeneousQuantityMatrix, CoordinateSystemA>;

  MyTensor& getTensor() { return tensor_; }
  MyHomogeneousTensor& getHomogeneousTensor() { return homogeneousTensor_; }

 private:
  MyTensor tensor_{1, 2, 3, 4, 5, 6};
  MyHomogeneousTensor homogeneousTensor_{1, 2, 3, 4, 5, 6};
};

TEST_F(TestTensorF, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getTensor(), getTensor());
}

TEST_F(TestTensorF, GetQuantityCompileTime) {  // NOLINT
  EXPECT_EQ((getTensor().getQuantity<0, 0>()), LengthQuantityI(1));
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI(4));
}

TEST_F(TestTensorF, GetQuantityRunTime) {  // NOLINT
  EXPECT_EQ(getHomogeneousTensor().getQuantity(0, 0), LengthQuantityI(1));
  EXPECT_EQ(getHomogeneousTensor().getQuantity(1, 0), LengthQuantityI(4));
}

TEST_F(TestTensorF, SetQuantityCompileTime) {  // NOLINT
  getHomogeneousTensor().setQuantity(1, 0, LengthQuantityI(1));
  EXPECT_EQ(getHomogeneousTensor().getQuantity(1, 0), LengthQuantityI(1));
}

TEST_F(TestTensorF, SetQuantityRunTime) {  // NOLINT
  getTensor().setQuantity<1, 0>(MassQuantityI(1));
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI(1));
}

TEST_F(TestTensorF, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = Quantities<MyValueMatrixB, MyUnitMatrixB>;
  using MyTensorB = Tensor<1, 0, MyQuantityMatrixB, CoordinateSystemA>;
  MyTensorB tensorB(2, 3, 5, 6);
  EXPECT_EQ((getTensor().block<0, 1, 2, 2>()), tensorB);
}

TEST_F(TestTensorF, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = Quantities<MyValueMatrixT, MyUnitMatrixT>;
  using MyTensorT = Tensor<1, 0, MyQuantityMatrixT, CoordinateSystemA>;
  const MyTensorT tensorT(1, 4, 2, 5, 3, 6);
  EXPECT_EQ(getTensor().transpose(), tensorT);
}

TEST(TestTensor2, ProductQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA>;
  const MyValue value{2};
  const LengthQuantityI quantity(value);
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyTensor tensor(valueMatrix);
  EXPECT_EQ(tensor * quantity, MyTensorResult(valueMatrix * value));
  EXPECT_EQ(quantity * tensor, MyTensorResult(valueMatrix * value));
}

TEST(TestTensor2, ProductValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyTensor tensor(valueMatrix);
  EXPECT_EQ(tensor * value, MyTensor(valueMatrix * value));
  EXPECT_EQ(value * tensor, MyTensor(valueMatrix * value));
}

TEST(TestTensor2, DivisionQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA>;
  const MyValue value{2};
  const LengthQuantityI quantity(value);
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyTensor tensor(valueMatrix);
  EXPECT_EQ(tensor / quantity, MyTensorResult(valueMatrix / value));
}

TEST(TestTensor2, DivisionValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyTensor tensor(valueMatrix);
  EXPECT_EQ(tensor / value, MyTensor(valueMatrix / value));
}

TEST(TestTensor2, Dot) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  EXPECT_EQ(MyTensor(1, 2, 3, 4).dotProduct(MyTensor(0, 3, -1, 2)), AreaQuantityI(11));
}

TEST(TestTensor2, DotBadFrames) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
struct CoordinateSystemBName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
using MyValueMatrix = ValueMatrix<int, 4, 1>;
using MyUnitMatrix = TypeMatrix<4, 1, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensorLhs = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
using MyTensorRhs = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemB>;

int main() {
  MyTensorLhs(1, 2, 3, 4).dotProduct(MyTensorRhs(0, 3, -1, 2));
  return 0;
}
)",
                                               "Coordinate systems do not match.");
}

#endif
