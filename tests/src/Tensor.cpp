#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

#include "coordinate_systems.hpp"
#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

#include "Unit.hpp"

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT

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
  const MyTensor tensor1;
  const MyTensor tensor2;
  const MyTensor tensor3 = tensor1 + tensor2;
  const MyTensor tensor = tensor3;
  EXPECT_EQ(tensor, tensor3);
}

TEST(TestTensor2, ConstructImplicitlyFromQuantities) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

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
using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;

int main() {
  MyQuantityMatrix quantities(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyTensor tensor = quantities;
  return 0;
}
)",
                                        "(no viable )?conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyQuantityMatrix quantities(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(quantities);
  EXPECT_EQ(tensor.getQuantities(), quantities);
}

TEST(TestTensor2, ConstructImplicitlyFromValues) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

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
using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;

int main() {
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyTensor tensor = values;
  return 0;
}
)",
                                        "(no viable )?conversion from");
}

TEST(TestTensor2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(values);
  EXPECT_EQ(tensor.getValues(), values);
}

TEST(TestTensor2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  const MyTensor tensor(1, 2, 3, 4, 5, 6);
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

TEST_F(TestTensorF, GetQuantityCompileTime) {  // NOLINT
  EXPECT_EQ((getTensor().getQuantity<0, 0>()), LengthQuantityI(1));
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI(4));
}

TEST_F(TestHomogeneousTensorF, GetQuantityRunTime) {  // NOLINT
  EXPECT_EQ(getTensor().getQuantity(0, 0), LengthQuantityI(1));
  EXPECT_EQ(getTensor().getQuantity(1, 0), LengthQuantityI(4));
}

TEST_F(TestHomogeneousTensorF, SetQuantityCompileTime) {  // NOLINT
  getTensor().setQuantity(1, 0, LengthQuantityI(1));
  EXPECT_EQ(getTensor().getQuantity(1, 0), LengthQuantityI(1));
}

TEST_F(TestTensorF, SetQuantityRunTime) {  // NOLINT
  getTensor().setQuantity<1, 0>(MassQuantityI(1));
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI(1));
}

TEST_F(TestTensorF, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = Quantities<MyValueMatrixB, MyUnitMatrixB>;
  using MyTensorB = Tensor<1, 0, MyQuantityMatrixB, CoordinateSystemA, Relative>;
  MyTensorB tensorB(2, 3, 5, 6);
  EXPECT_EQ((getTensor().block<0, 1, 2, 2>()), tensorB);
}

TEST_F(TestTensorF, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = Quantities<MyValueMatrixT, MyUnitMatrixT>;
  using MyTensorT = Tensor<1, 0, MyQuantityMatrixT, CoordinateSystemA, Relative>;
  const MyTensorT tensorT(1, 4, 2, 5, 3, 6);
  EXPECT_EQ(getTensor().transpose(), tensorT);
}

TEST_F(TestTensorF, Sum) {  // NOLINT
  EXPECT_EQ((getTensor() + getTensor()).getQuantities(), getTensor().getQuantities() + getTensor().getQuantities());
}

TEST_F(TestTensorF, SumRelativeAndAbsolute) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor(1, 4, 2, 5, 3, 6);
  EXPECT_EQ((getTensor() + tensor).getQuantities(), getTensor().getQuantities() + tensor.getQuantities());
}

TEST_F(TestTensorF, SumAbsoluteAndRelative) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor(1, 4, 2, 5, 3, 6);
  EXPECT_EQ((tensor + getTensor()).getQuantities(), getTensor().getQuantities() + tensor.getQuantities());
}

TEST(TestTensor, SumAbsoluteAndAbsolute) {  // NOLINT
  const std::string code = R"(
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

#include "Unit.hpp"

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;

struct CoordinateSystemAName {};
using CoordinateSystemA = wuzp::CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;

int main() {
  const MyAbsoluteTensor tensor(1, 4, 2, 5, 3, 6);
  tensor + tensor;
  return 0;
}
)";
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "Two absolute objects cannot be summed.");
}

TEST_F(TestTensorF, Difference) {  // NOLINT
  EXPECT_EQ((getTensor() - getTensor()).getQuantities(), getTensor().getQuantities() - getTensor().getQuantities());
}

TEST_F(TestTensorF, DifferenceAbsoluteAndRelative) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor(1, 4, 2, 5, 3, 6);
  EXPECT_EQ((tensor - getTensor()).getQuantities(), tensor.getQuantities() - getTensor().getQuantities());
}

TEST(TestTensor, DifferenceRelativeAndAbsolute) {  // NOLINT
  const std::string code = R"(
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

#include "Unit.hpp"

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;

struct CoordinateSystemAName {};
using CoordinateSystemA = wuzp::CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;

int main() {
  const MyTensor tensor(1, 2, 3, 4, 5, 6);
  const MyAbsoluteTensor absoluteTensor(1, 4, 2, 5, 3, 6);
  tensor - absoluteTensor;
  return 0;
}
)";
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "An absolute object cannot be subtracted from a relative object.");
}

TEST_F(TestTensorF, DifferenceAbsoluteAndAbsolute) {  // NOLINT
  using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const MyAbsoluteTensor tensor(1, 4, 2, 5, 3, 6);
  EXPECT_EQ((tensor - tensor).getQuantities(), tensor.getQuantities() - tensor.getQuantities());
}

TEST(TestTensor2, ProductQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyValue value{2};
  const LengthQuantityI quantity(value);
  const MyValueMatrix values(1, 2, 3, 4);
  const MyTensor tensor(values);
  EXPECT_EQ(tensor * quantity, MyTensorResult(values * value));
  EXPECT_EQ(quantity * tensor, MyTensorResult(values * value));
}

TEST(TestTensor2, ProductValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values(1, 2, 3, 4);
  const MyTensor tensor(values);
  EXPECT_EQ(tensor * value, MyTensor(values * value));
  EXPECT_EQ(value * tensor, MyTensor(values * value));
}

TEST(TestTensor2, DivisionQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensorResult = Tensor<1, 0, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyValue value{2};
  const LengthQuantityI quantity(value);
  const MyValueMatrix values(1, 2, 3, 4);
  const MyTensor tensor(values);
  EXPECT_EQ(tensor / quantity, MyTensorResult(values / value));
}

TEST(TestTensor2, DivisionValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values(1, 2, 3, 4);
  const MyTensor tensor(values);
  EXPECT_EQ(tensor / value, MyTensor(values / value));
}

TEST(TestTensor2, Dot) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;
  EXPECT_EQ(MyTensor(1, 2, 3, 4).dotProduct(MyTensor(0, 3, -1, 2)), AreaQuantityI(11));
}

TEST(TestTensor2, DotBadFrames) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

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
using MyTensorLhs = Vector<MyQuantityMatrix, CoordinateSystemA>;
using MyTensorRhs = Vector<MyQuantityMatrix, CoordinateSystemB>;

int main() {
  MyTensorLhs(1, 2, 3, 4).dotProduct(MyTensorRhs(0, 3, -1, 2));
  return 0;
}
)",
                                               "Coordinate systems do not match.");
}

TEST(TestTensor2, Norm) {  // NOLINT
  const PositionVector3<ValueMatrix, int, CoordinateSystemA> vector(-1, 2, 3);
  EXPECT_EQ(vector.norm(), LengthQuantityI(vector.getValues().getImplementation().norm()));
}

TEST(TestTensor2, SquaredNorm) {  // NOLINT
  const PositionVector3<ValueMatrix, int, CoordinateSystemA> vector(-1, 2, 3);
  EXPECT_EQ(vector.squaredNorm(), SquaredLengthQuantityI(vector.getValues().getImplementation().squaredNorm()));
}

TEST(TestTensor2, Determinant) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<1, 1, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyTensor tensor(-1, 2, 3, 1);
  EXPECT_EQ(tensor.determinant(), LengthQuantityI(tensor.getValues().getImplementation().determinant()));
}

#endif
