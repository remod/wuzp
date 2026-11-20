// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The core compares floating point values relatively, with Comparison::approxEqual, and integers exactly, with
// Comparison::equal (see ValuesWrapper::isApprox()), so ValueMatrixBackend requires approxEqual only for floating point values.
// The value matrix archetype, which provides it only for them, without approxEqual of 3x3 double matrices, does not satisfy it:
// it misses only this requirement.

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

#include "archetype_values/ValueMatrix.hpp"

namespace wuzp {

// The comparison of the archetype, but without approxEqual() for 3x3 double matrices.
template <>
struct Comparison<archetype_values::internal::Access<double, 3, 3>, archetype_values::internal::Access<double, 3, 3>> {
  using MatrixT = archetype_values::internal::Matrix<double, 3, 3>;
  static bool equal(const MatrixT& lhs, const MatrixT& rhs) {
    return archetype_values::internal::sumOver<int, 3, 3>(
               [&](const int row, const int col) { return lhs.at(row, col) == rhs.at(row, col) ? 0 : 1; }) == 0;
  }
};

}  // namespace wuzp

namespace {

template <template <typename, int, int> typename ValueMatrixT_>
  requires wuzp::ValueMatrixBackend<ValueMatrixT_>
struct CheckedBackend {};

}  // namespace

static_assert(wuzp::internal::BackendMatrixOfSize<archetype_values::ValueMatrix, double, 1, 1>);
static_assert(wuzp::internal::BackendMatrixOfSize<archetype_values::ValueMatrix, double, 3, 1>);
static_assert(wuzp::internal::BackendMatrixOfSize<archetype_values::ValueMatrix, double, 1, 3>);
static_assert(wuzp::internal::BackendVectorProducts<archetype_values::ValueMatrix, double>);
static_assert(wuzp::internal::BackendValues<archetype_values::ValueMatrix, int>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedBackend<archetype_values::ValueMatrix>);
  return 0;
}
