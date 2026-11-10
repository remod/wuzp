// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The matrix of a value matrix backend is constructible from the lazy results of its traits: the converting constructor of
// ValuesWrapper constructs it from them, e.g. when an unevaluated sum is converted into a vector. A backend whose matrix cannot be
// constructed from a result, although the result evaluates to it, does not satisfy the concepts of the backend interface.

#include <type_traits>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

namespace {

// The matrices of a linear algebra library and its lazy sum of two matrices, which cannot be converted into a matrix.
struct Matrix {};
struct LazySum {};

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

static_assert(!std::is_constructible_v<Matrix, LazySum>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedResult<LazySum>);
  return 0;
}
