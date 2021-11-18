#include <iostream>

#include <Eigen/Core>
#include <Eigen/Geometry>
//#include <../../eigen/unsupported/Eigen/CXX11/Tensor>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/CoordinateSystemMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/Rotation.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

int main() {
  //  // Create 2 matrices using tensors of rank 2
  //  Eigen::Tensor<int, 2> a(2, 3);
  //  a.setValues({{1, 2, 3}, {6, 5, 4}});
  //  Eigen::Tensor<int, 2> b(3, 2);
  //  b.setValues({{1, 2}, {4, 5}, {5, 6}});
  //
  //  // Compute the traditional matrix product
  //  Eigen::array<Eigen::IndexPair<int>, 1> product_dims = { Eigen::IndexPair<int>(1, 0) };
  //  Eigen::Tensor<int, 2> AB = a.contract(b, product_dims);
  //
  //  // Compute the product of the transpose of the matrices
  //  Eigen::array<Eigen::IndexPair<int>, 1> transposed_product_dims = { Eigen::IndexPair<int>(0, 1) };
  //  Eigen::Tensor<int, 2> AtBt = a.contract(b, transposed_product_dims);
  //
  //  // Contraction to scalar value using a double contraction.
  //  // First coordinate of both tensors are contracted as well as both second coordinates, i.e., this computes the sum of the squares of
  //  the elements. Eigen::array<Eigen::IndexPair<int>, 2> double_contraction_product_dims = { Eigen::IndexPair<int>(0, 0),
  //  Eigen::IndexPair<int>(1, 1) }; Eigen::Tensor<int, 0> AdoubleContractedA = a.contract(a, double_contraction_product_dims);
  //
  //  // Extracting the scalar value of the tensor contraction for further usage
  //  int value = AdoubleContractedA(0);
  return 0;
}
