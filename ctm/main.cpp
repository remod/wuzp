#include <iostream>

#include "ctm/Matrix.hpp"

int main() {
    using lengthInv = divide<dimensionless, length>;
    using massInv = divide<dimensionless, mass>;
    Matrix<2, 2, length, length, length, length> matrix1;
    Matrix<2, 2, length, length, length, mass> matrix2;
    Matrix<2, 2, lengthInv, lengthInv, lengthInv, lengthInv> matrix3;
    static_assert(matrix1.isHomogeneous);
    static_assert(!matrix2.isHomogeneous);
    static_assert(matrix3.isHomogeneous);
    matrix1 + matrix1;
    matrix2 + matrix2;
    //matrix1 + matrix2;
    matrix1.inverse();
    //matrix2.inverse();
    matrix1.inverse() + matrix3;
    quantity<length> q1 = 1.0*meters;
    Matrix<2, 2, dimensionless, dimensionless, dimensionless, divide<mass, length>> temp = matrix2 / q1;

    std::cout << type_name<ToRows<3, int, double, int, float, char, double>::type>() << std::endl;
    std::cout << type_name<ToCols<3, int, double, int, float, char, double>::type>() << std::endl;
    static_assert(std::is_same<ToRows<3, int, double, int, float, char, double>::type, std::tuple<std::tuple<int, double, int>, std::tuple<float, char, double>>>::value);
    static_assert(std::is_same<ToCols<3, int, double, int, float, char, double>::type, std::tuple<std::tuple<int, float>, std::tuple<double, char>, std::tuple<int, double>>>::value);

    Matrix<2, 2, multiply<length, length>, multiply<length, length>, multiply<length, length>, multiply<length, length>> temp2 = matrix1 * matrix1;

    return 0;
}
