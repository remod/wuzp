// wuzp-expect: error
// wuzp-static-assert: Inverse is not implemented for blitz\.

// Checking the interface of a value matrix backend that misses a trait fails with the message of the trait (see "The backend
// interface" in concepts.hpp): Blitz++ provides no inverses (see "Backend support for inverses" in DESIGN.md), so it is a
// ValueMatrixBackend, but not an InvertibleValueMatrixBackend.

#include "wuzp/core/concepts.hpp"
#include "wuzp_blitz/ValueMatrix.hpp"

static_assert(wuzp::InvertibleValueMatrixBackend<wuzp_blitz::ValueMatrix>);

int main() {
  return 0;
}
