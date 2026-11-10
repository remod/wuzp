// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The core moves the matrices of a value matrix backend, e.g. the results of the traits into the values of their type. The value
// matrix archetype, whose 1x1 matrices of doubles cannot be moved, does not satisfy the concepts of the backend interface: it
// misses only this requirement.

#include <concepts>

#include "wuzp/core/concepts.hpp"

#include "archetype_values/ValueMatrix.hpp"

namespace archetype_values::internal {

// The matrix of the archetype, but without a move constructor.
template <>
class Matrix<double, 1, 1> {
 public:
  Matrix() = default;
  Matrix(const Matrix&) = delete;
  Matrix(Matrix&&) = delete;
  Matrix& operator=(const Matrix&) = delete;
  Matrix& operator=(Matrix&&) = delete;
  ~Matrix() = default;

  double& at(const int /*row*/, const int /*col*/) { return element_; }
  double at(const int /*row*/, const int /*col*/) const { return element_; }

 private:
  double element_{};
};

}  // namespace archetype_values::internal

namespace {

template <template <typename, int, int> typename ValueMatrixT_>
  requires wuzp::ValueMatrixBackend<ValueMatrixT_>
struct CheckedBackend {};

}  // namespace

static_assert(std::default_initializable<archetype_values::internal::Matrix<double, 1, 1>>);
static_assert(!std::move_constructible<archetype_values::internal::Matrix<double, 1, 1>>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedBackend<archetype_values::ValueMatrix>);
  return 0;
}
