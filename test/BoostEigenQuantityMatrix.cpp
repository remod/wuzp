#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/QuantityMatrix.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"
#include "ctm/boost_eigen.hpp"
#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using boost::units::si::area;
using boost::units::si::energy;
using boost::units::si::force;
using boost::units::si::length;
using boost::units::si::mass;
using boost::units::si::volume;
using ctm::Frame;
using ctm::Position2D;
using ctm::Position3D;
using ctm::ProperRigidTransformation2D;
using ctm::ProperRigidTransformation3D;
using ctm::QuantityMatrix;
using ctm::Quaternion;
using ctm::Sum;
using ctm::Translation2D;
using ctm::Translation3D;
using ctm::TypeMatrix;
using ctm::Vector3D;

class BoostEigenQuantityMatrix : public ::testing::Test {
 public:
  using MyValues = Eigen::Matrix<int, 2, 3>;
  using MyUnits = TypeMatrix<2, 3, length, area, volume, mass, force, energy>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;

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

TEST_F(BoostEigenQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix(), getMatrix());
}

TEST_F(BoostEigenQuantityMatrix, Transpose) {  // NOLINT
  using MyValuesT = Eigen::Matrix<int, 3, 2>;
  using MyUnitsT = TypeMatrix<3, 2, length, mass, area, force, volume, energy>;
  using MyMatrixT = QuantityMatrix<MyValuesT, MyUnitsT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix().transpose(), matrixT);
}
