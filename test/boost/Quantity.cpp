//#include <gtest/gtest.h>
//
//#include "ctm/boost/boost.hpp"
//#include "ctm/core/Quantity.hpp"
//#include "ctm/core/traits.hpp"
//
// namespace units = boost::units::si;
// using boost::units::absolute;
// using ctm::Quantity;
// using ctm::TypeMatrix;
// using ctm::UnitMatrix;
//
// using Length = Quantity<double, units::length>;
//
// TEST(BoostQuantity, Addition) {  // NOLINT
//  EXPECT_EQ(Length(1.0) + Length(2.0), Length(3.0));
//}
//
// TEST(BoostQuantity, Subtraction) {  // NOLINT
//  EXPECT_EQ(getMatrix1() - getMatrix2(), MyMatrix(getValues1() - getValues2()));
//}
//
// TEST(BoostQuantity, EqualityOperator) {  // NOLINT
//  EXPECT_EQ(getMatrix1(), getMatrix1());
//}
//
// TEST(BoostQuantity, InEqualityOperator) {  // NOLINT
//  EXPECT_NE(getMatrix1(), getMatrix1() + getMatrix2());
//}
//
// TEST(BoostQuantity, Block) {  // NOLINT
//  using MyValuesB = Eigen::Matrix<int, 2, 2>;
//  using MyUnitsB = UnitMatrix<2, 2, units::area, units::volume, units::force, units::energy>;
//  using MyMatrixB = QuantityMatrix<MyValuesB, MyUnitsB>;
//  MyValuesB valuesB;
//  valuesB << 2, 3, 5, 6;
//  MyMatrixB matrixB(valuesB);
//  EXPECT_EQ((getMatrix1().block<0, 1, 2, 2>()), matrixB);
//}
//
// TEST(BoostQuantity, Transpose) {  // NOLINT
//  using MyValuesT = Eigen::Matrix<int, 3, 2>;
//  using MyUnitsT = UnitMatrix<3, 2, units::length, units::mass, units::area, units::force, units::volume, units::energy>;
//  using MyMatrixT = QuantityMatrix<MyValuesT, MyUnitsT>;
//  MyValuesT valuesT;
//  valuesT << 1, 4, 2, 5, 3, 6;
//  MyMatrixT matrixT(valuesT);
//  EXPECT_EQ(getMatrix1().transpose(), matrixT);
//}
//
// TEST(BoostQuantity, Product) {  // NOLINT
//  using MyValues = Eigen::Matrix<int, 2, 2>;
//  using MyUnits = UnitMatrix<2, 2, units::length>;
//  using MyUnitsResult = UnitMatrix<2, 2, units::area>;
//  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
//  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
//  MyValues values;
//  values << 1, 2, 3, 4;
//  MyMatrix matrix;
//  matrix.setValues(values);
//  EXPECT_EQ(matrix * matrix, MyMatrixResult(values * values));
//}
//
// TEST(BoostQuantity, ScalarProduct) {  // NOLINT
//  using MyValues = Eigen::Matrix<int, 4, 1>;
//  using MyUnits = UnitMatrix<4, 1, units::length>;
//  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
//  using MyValuesResult = int;
//  using MyUnitsResult = UnitMatrix<1, 1, units::area>;
//  using MyMatrixResult = QuantityMatrix<MyValuesResult, MyUnitsResult>;
//  MyValues values1;
//  values1 << 1, 2, 3, 4;
//  MyValues values2;
//  values2 << 0, 3, -1, 2;
//  EXPECT_EQ(MyMatrix(values1).dot(MyMatrix(values2)), MyMatrixResult(11));
//}
//
// TEST(BoostQuantity, Chain) {  // NOLINT
//  using MyValues = Eigen::Matrix<int, 2, 2>;
//  using MyUnits1 = UnitMatrix<2, 2, units::length>;
//  using MyUnits2 = UnitMatrix<2, 2, units::area>;
//  using MyUnitsResult = UnitMatrix<2, 2, units::area>;
//  using MyMatrix1 = QuantityMatrix<MyValues, MyUnits1>;
//  using MyMatrix2 = QuantityMatrix<MyValues, MyUnits2>;
//  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
//  MyValues values;
//  values << 1, 2, 3, 4;
//  MyMatrix1 matrix1(values);
//  MyMatrix2 matrix2(values);
//  EXPECT_EQ(matrix2 + matrix1 * matrix1, MyMatrixResult(values + values * values));
//}
//
// TEST(BoostQuantity, Inverse) {  // NOLINT
//  using MyUnitsInv = UnitMatrix<2, 2, units::frequency>;
//  using MyMatrixInv = QuantityMatrix<MyValues, MyUnitsInv>;
//  MyValues valuesInv;
//  valuesInv << 0.6, -0.7, -0.2, 0.4;
//  MyMatrixInv matrixInv(valuesInv);
//  EXPECT_TRUE(getMatrix().inverse().isApprox(matrixInv));
//}
