#include <iostream>

#include "ctm/Frame.hpp"
#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"
#include "ctm/eigen.hpp"

using namespace ctm;
using namespace boost::units;
using namespace boost::units::si;

// https://stackoverflow.com/a/23549126

// template<class T>
// struct sink {
//  typedef void type;
//};
//
// template<class T>
// using sink_t = typename sink<T>::type;
//
// template<typename T, typename = void>
// struct my_test : std::false_type {};
//
// template<typename T>
// struct my_test<T,
//    sink_t<decltype(std::declval<Sum<length, area>::type>())>
//>:std::true_type {};

template <typename, typename = void>
constexpr bool is_type_complete_v = false;

template <typename T>
constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

int main() {
  //  static_assert(std::false_type::value);
  //  static_assert(!is_type_complete_v<Sum<length, area, length>::type>);

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

  struct FrameAImpl {};
  using FrameA = Frame<FrameAImpl>;
  using MyValues = Eigen::Matrix<double, 3, 2>;
  using MyFrames = TypeMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  using MyMatrix = Matrix<MyValues, MyFrames, MyUnits>;
  static_assert(std::is_same<Sum<MyValues, MyValues>::type, MyValues>::value);
  static_assert(std::is_same<Sum<MyFrames, MyFrames>::type, MyFrames>::value);
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
  static_assert(std::is_same<Sum<MyMatrix, MyMatrix>::type, MyMatrix>::value);

  //  using lengthInv = Divide<dimensionless, length>;
  //  using massInv = Divide<dimensionless, mass>;
  //  Matrix<2, 2, length, length, length, length> matrix1;
  //  Matrix<2, 2, length, length, length, mass> matrix2;
  //  Matrix<2, 2, lengthInv, lengthInv, lengthInv, lengthInv> matrix3;
  //  static_assert(matrix1.isHomogeneous);
  //  static_assert(!matrix2.isHomogeneous);
  //  static_assert(matrix3.isHomogeneous);
  //  matrix1 + matrix1;
  //  matrix2 + matrix2;
  //  //matrix1 + matrix2;
  //  matrix1.inverse();
  //  //matrix2.inverse();
  //  matrix1.inverse() + matrix3;
  //  quantity<length> q1 = 1.0*meters;
  //  Matrix<2, 2, dimensionless, dimensionless, dimensionless, Divide<mass, length>> temp = matrix2 / q1;

  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  const Eigen::Vector3d translation(1.0, 2.0, 3.0);
  const ProperRigidTransformationEigen<FrameA, FrameA, double> transformation(rotation, translation);
  const Eigen::Vector3d object(1.0, 2.0, 3.0);
  auto result = transformation.transform(object);

  std::cout << result << std::endl;
  std::cout << "Hello from main." << std::endl;

  return 0;
}
