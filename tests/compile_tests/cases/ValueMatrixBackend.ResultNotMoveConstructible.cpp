// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The core moves the lazy results of the traits of a value matrix backend, e.g. into the values of their type. A backend whose
// lazy result cannot be moved does not satisfy the concepts of the backend interface, although its matrix can be constructed from
// it.

#include <type_traits>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

namespace {

// The lazy sum of two matrices of a linear algebra library, which cannot be moved, and its matrices, which can be constructed
// from it.
struct LazySum {
  LazySum() = default;
  LazySum(const LazySum&) = delete;
  LazySum(LazySum&&) = delete;
  LazySum& operator=(const LazySum&) = delete;
  LazySum& operator=(LazySum&&) = delete;
  ~LazySum() = default;
};
struct Matrix {
  explicit Matrix(const LazySum& /*sum*/) {}
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

static_assert(!std::is_move_constructible_v<LazySum>);
static_assert(std::is_constructible_v<Matrix, LazySum> && std::is_constructible_v<Matrix, const LazySum&>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedResult<LazySum>);
  return 0;
}
