// wuzp-expect: error
// wuzp-static-assert: 'Inverse' trait not specialized for given type\.

// Checking the interface of a value matrix backend that misses a trait fails with the message of the primary template of the
// trait, rather than making the concept unsatisfied (see "The backend interface" in concepts.hpp): no trait is specialized for
// the matrices of this backend, so checking their inverses fails with the message of Inverse.

#include "wuzp/core/concepts.hpp"

namespace {

// The matrices of a linear algebra library, for which no trait is specialized.
struct Matrix {};

template <typename ValueT_, int RowsV_, int ColsV_>
struct ValueMatrixWithoutTraits {
  using ImplementationT = Matrix;
};

}  // namespace

static_assert(wuzp::internal::BackendInverse<ValueMatrixWithoutTraits, double, 3>);

int main() {
  return 0;
}
