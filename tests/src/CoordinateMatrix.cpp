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
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, Coordinate<Quantity<double, Length>, CoordinateSystemA>>::type, FromCoordinates>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, MyCoordinateMatrix&&>::type, FromCoordinateMatrix>);
  static_assert(std::is_same_v<GetTag<MyCoordinateMatrix, MyQuantityMatrix>::type, FromQuantityMatrixArgs>);
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

TEST(TestCoordinateMatrix2, ConstructExplicitlyFromCoordinates) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyCoordinate = Coordinate<Quantity<int, Length>, CoordinateSystemA>;
  const MyCoordinateMatrix coordinateMatrix(MyCoordinate(1), MyCoordinate(2), MyCoordinate(3), MyCoordinate(4), MyCoordinate(5),
                                            MyCoordinate(6));
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(coordinateMatrix.getValueMatrix(), valueMatrix);
}

TEST(TestCoordinateMatrix2, ConstructImplicitlyFromQuantityMatrix) {  // NOLINT
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/TypeMatrix.hpp"

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
template <typename ScalarT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ScalarT, Rows, Cols>;
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
                                        "(no viable )?conversion from");
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

TEST(TestCoordinateMatrix2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 3, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyQuantity = Quantity<int, Length>;
  const MyCoordinateMatrix coordinateMatrix(MyQuantity(1), MyQuantity(2), MyQuantity(3), MyQuantity(4), MyQuantity(5), MyQuantity(6));
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(coordinateMatrix.getValueMatrix(), valueMatrix);
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
template <typename ScalarT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ScalarT, Rows, Cols>;
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
  const MyValueMatrix valueMatrix(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyCoordinateMatrix coordinateMatrix = valueMatrix;
  return 0;
}
)",
                                        "(no viable )?conversion from");
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

TEST(TestCoordinateMatrix2, ProductCoordinate) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyCoordinate = Coordinate<MyQuantity, CoordinateSystemA>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  using MyCoordinateMatrixResult = CoordinateMatrix<MyQuantityMatrixResult, MyCoordinateSystemMatrix>;
  const MyValue value{2};
  const MyCoordinate coordinate(value);
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix * coordinate, MyCoordinateMatrixResult(valueMatrix * value));
  EXPECT_EQ(coordinate * coordinateMatrix, MyCoordinateMatrixResult(valueMatrix * value));
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

TEST(TestCoordinateMatrix2, ProductValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix * value, MyCoordinateMatrix(valueMatrix * value));
  EXPECT_EQ(value * coordinateMatrix, MyCoordinateMatrix(valueMatrix * value));
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

TEST(TestCoordinateMatrix2, DivisionValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyCoordinateSystemMatrix = TypeMatrix<2, 2, CoordinateSystemA>;
  using MyCoordinateMatrix = CoordinateMatrix<MyQuantityMatrix, MyCoordinateSystemMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix valueMatrix(1, 2, 3, 4);
  const MyCoordinateMatrix coordinateMatrix(valueMatrix);
  EXPECT_EQ(coordinateMatrix / value, MyCoordinateMatrix(valueMatrix / value));
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
template <typename ScalarT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ScalarT, Rows, Cols>;
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
template <typename ScalarT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ScalarT, Rows, Cols>;
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
