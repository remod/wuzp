// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The converting constructor of ValuesWrapper constructs the matrix of a value matrix backend from a const reference to a lazy
// result of its traits, e.g. when an unevaluated sum is converted into a vector. A backend whose matrix can only be constructed
// from a temporary result does not satisfy the concepts of the backend interface.

#include <type_traits>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

namespace {

// The matrices of a linear algebra library and its lazy sum of two matrices, which only a temporary can be converted from.
struct LazySum {};
struct Matrix {
  explicit Matrix(LazySum&& /*sum*/) {}
};

// A trait whose result is the lazy sum.
struct SumTrait {
  using type = LazySum;
};

template <typename ResultT_>
  requires wuzp::internal::TraitResult<ResultT_, SumTrait, Matrix>
struct CheckedResult {};

}  // namespace

namespace wuzp {

// The lazy sum evaluates to a matrix.
template <>
struct Evaluate<AccessImplementation<LazySum>> {
  using type = Matrix;
};

}  // namespace wuzp

static_assert(std::is_constructible_v<Matrix, LazySum> && !std::is_constructible_v<Matrix, const LazySum&>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedResult<LazySum>);
  return 0;
}
