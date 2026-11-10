// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The core default constructs a matrix of a value matrix backend before it initializes it. The value matrix archetype, whose
// 1x1 matrices of doubles cannot be default constructed, does not satisfy the concepts of the backend interface: it misses only
// this requirement.

#include <concepts>

#include "wuzp/core/concepts.hpp"

#include "archetype_values/ValueMatrix.hpp"

namespace archetype_values::internal {

// The matrix of the archetype, but constructed from its element instead of by default.
template <>
class Matrix<double, 1, 1> {
 public:
  explicit Matrix(const double element) : element_(element) {}
  Matrix(const Matrix&) = delete;
  Matrix(Matrix&&) noexcept = default;
  Matrix& operator=(const Matrix&) = delete;
  Matrix& operator=(Matrix&&) = delete;
  ~Matrix() = default;

  double& at(const int /*row*/, const int /*col*/) { return element_; }
  double at(const int /*row*/, const int /*col*/) const { return element_; }

 private:
  double element_;
};

}  // namespace archetype_values::internal

namespace {

template <template <typename, int, int> typename ValueMatrixT_>
  requires wuzp::ValueMatrixBackend<ValueMatrixT_>
struct CheckedBackend {};

}  // namespace

static_assert(!std::default_initializable<archetype_values::internal::Matrix<double, 1, 1>>);
static_assert(std::move_constructible<archetype_values::internal::Matrix<double, 1, 1>>);

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedBackend<archetype_values::ValueMatrix>);
  return 0;
}
