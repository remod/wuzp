#include <iostream>

#include "ctm/Frame.hpp"
#include "ctm/QuantityMatrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"
#include "ctm/boost_eigen.hpp"
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

  std::cout << "Hello from main." << std::endl;

  return 0;
}
