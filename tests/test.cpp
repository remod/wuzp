#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"
#include "ctm/eigen.hpp"

using namespace ctm;
using namespace boost::units;
using namespace boost::units::si;

TEST(Test, Test) { // NOLINT
  static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type,
      std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type,
      std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);

  static_assert(std::is_same<Sum<length>::type, length>::value);
  static_assert(std::is_same<Sum<length, length>::type, length>::value);
  static_assert(std::is_same<Sum<length, length, length>::type, length>::value);
  static_assert(std::is_same<Product<length, length>::type, area>::value);

  static_assert(std::is_same<ToTuple<3, int>, std::tuple<int, int, int>>::value);
  static_assert(std::is_same<TypeMatrix<3, 2, int>, TypeMatrixImpl<3, 2, std::tuple<int, int, int, int, int, int>>>::value);
  static_assert(
      std::is_same<TypeMatrix<3, 2, int, int, int, int, int, int>, TypeMatrixImpl<3, 2, std::tuple<int, int, int, int, int, int>>>::value);

  static_assert(std::is_same<Sum<TypeMatrix<2, 2, length, length, length, length>, TypeMatrix<2, 2, length>>::type,
                TypeMatrix<2, 2, length, length, length, length>>::value);
  static_assert(std::is_same<Product<TypeMatrix<2, 2, length, length, length, length>, TypeMatrix<2, 2, length>>::type,
                TypeMatrix<2, 2, area, area, area, area>>::value);

  struct FrameAName {};
  using FrameA = Frame<FrameAName>;
  using MyValues = Eigen::Matrix<double, 3, 2>;
  using MyFrames = TypeMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  using MyMatrix = Matrix<MyValues, MyFrames, MyUnits>;
  static_assert(std::is_same<Sum<MyValues, MyValues>::type, MyValues>::value);
  static_assert(std::is_same<Sum<MyFrames, MyFrames>::type, MyFrames>::value);
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
  static_assert(std::is_same<Sum<MyMatrix, MyMatrix>::type, MyMatrix>::value);
}