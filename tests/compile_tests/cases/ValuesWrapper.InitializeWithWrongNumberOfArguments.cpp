// wuzp-expect: error
// wuzp-static-assert: Number of arguments does not match size of matrix\.

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<int, 3, 2> matrix{1, 2, 3, 4, 5};
  return 0;
}
