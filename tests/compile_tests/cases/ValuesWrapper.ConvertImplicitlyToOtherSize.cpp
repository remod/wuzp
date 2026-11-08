// wuzp-expect: error
// wuzp-error(compiler=gnu|clang|appleclang): (no viable )?conversion from
// wuzp-error(compiler=msvc): cannot convert from

// Values of another size do not convert implicitly, so that type traits such as std::is_convertible report the conversion as
// invalid (#70). Direct initialization explains why it fails (ValuesWrapper.ConvertRowsMismatch).

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<double, 2, 1> values{1.0, 2.0};
  // Use = instead of {} to trigger implicit conversion.
  [[maybe_unused]] const ValueMatrix<double, 3, 1> converted = values;
  return 0;
}
