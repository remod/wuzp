#include <iostream>

#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/core.hpp"
#include "ctm/Frame.hpp"
#include "ctm/boost_units.hpp"

using namespace ctm;
using namespace boost::units;
using namespace boost::units::si;

// https://stackoverflow.com/a/23549126

//template<class T>
//struct sink {
//  typedef void type;
//};
//
//template<class T>
//using sink_t = typename sink<T>::type;
//
//template<typename T, typename = void>
//struct my_test : std::false_type {};
//
//template<typename T>
//struct my_test<T,
//    sink_t<decltype(std::declval<Sum<length, area>::type>())>
//>:std::true_type {};

template<typename, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T>
constexpr bool is_type_complete_v
    <T, std::void_t<decltype(sizeof(T))>> = true;

int main() {
//  static_assert(std::false_type::value);
//  static_assert(!is_type_complete_v<Sum<length, area, length>::type>);

  static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type, std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
  static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type, std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);

  static_assert(std::is_same<Sum<length>::type, length>::value);
  static_assert(std::is_same<Sum<length, length>::type, length>::value);
  static_assert(std::is_same<Sum<length, length, length>::type, length>::value);
//  static_assert(std::is_same<Sum<Matrix<2, 2, length, length, length, length>, Matrix<2, 2, length, length, length, length>>::type, Matrix<2, 2, length, length, length, length>>::value);

  static_assert(std::is_same<Product<length, length>::type, area>::value);
  static_assert(std::is_same<Product<CtMatrix<2, 2, length, length, length, length>, CtMatrix<2, 2, length, length, length, length>>::type, CtMatrix<2, 2, area, area, area, area>>::value);

  using ctm::Frame;

  struct FrameAImpl {};
  using FrameA = Frame<FrameAImpl>;
  using MyFrames = CtMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  using MyUnits = CtMatrix<3, 2, length, area, volume, mass, force, energy>;
  using MyMatrix = Matrix<3, 2, double, MyFrames, MyUnits>;
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

  return 0;
}
