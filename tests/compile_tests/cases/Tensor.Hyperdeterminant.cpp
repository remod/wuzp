// wuzp-expect: error
// wuzp-static-assert: Not implemented

// The hyperdeterminant is not implemented.

#include "compile_test_types.hpp"

int main() {
  TestTensor<1, 0>::zero().hyperdeterminant();
  return 0;
}
