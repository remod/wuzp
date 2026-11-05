// wuzp-expect: error
// wuzp-static-assert: 'Product' trait not specialized for given type\.

#include "wuzp/core/traits.hpp"

using namespace wuzp;  // NOLINT

using type = Product<double, int>::type;

int main() {
  return 0;
}
