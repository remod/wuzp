// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The core computes the determinants of integer matrices, e.g. of the quantities of an integer linear map, so
// InvertibleValueMatrixBackend requires them, besides those of floating point matrices. The value matrix archetype without the
// determinant of 3x3 integer matrices, like a backend that only computes determinants in floating point, does not satisfy it: it
// misses only this requirement.

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

#include "archetype_values/ValueMatrix.hpp"

namespace wuzp {

// The determinant of the archetype, but without value() for 3x3 integer matrices.
template <>
struct Determinant<archetype_values::internal::Access<int, 3, 3>> {};

}  // namespace wuzp

namespace {

template <template <typename, int, int> typename ValueMatrixT_>
  requires wuzp::InvertibleValueMatrixBackend<ValueMatrixT_>
struct CheckedBackend {};

}  // namespace

static_assert(wuzp::ValueMatrixBackend<archetype_values::ValueMatrix>);
static_assert(wuzp::internal::BackendInverse<archetype_values::ValueMatrix, double, 3>);
static_assert(wuzp::internal::BackendDeterminant<archetype_values::ValueMatrix, double, 3>);
static_assert(wuzp::internal::BackendDeterminant<archetype_values::ValueMatrix, int, 1>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedBackend<archetype_values::ValueMatrix>);
  return 0;
}
