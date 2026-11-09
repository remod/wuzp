// wuzp-expect: error
// wuzp-error(compiler=gnu|clang|appleclang): (no viable )?conversion from
// wuzp-error(compiler=msvc): cannot convert from

// Invalid conversions are not implicit, so that type traits such as std::is_convertible report them as invalid, and overloads
// for different coordinate systems are not ambiguous (#70). Direct initialization explains why the conversion fails
// (Tensor.ConvertPointToVector).

#include "compile_test_types.hpp"

int main() {
  // Use = instead of {} to trigger implicit conversion.
  [[maybe_unused]] const TestTensor<Indices<Up>> tensor = TestTensor<Indices<Up>, wuzp::Absolute>::zero();
  return 0;
}
