// Functions that check that ../compare_codegen.py follows every call and jump out of a function: to a helper in an anonymous
// namespace, which the assembler calls without a relocation, to an inline function that is not inlined, which the compilers
// emit in a section of its own and call through a relocation against its symbol, like the template functions of wuzp, and to
// a cold part in another section, which GCC splits off.

#include <stdexcept>

namespace {

[[gnu::noinline]] double helper(const double* values) {
  return values[0] * values[1] + values[2];
}

}  // namespace

namespace codegen {

[[gnu::noinline]] inline double inlineHelper(const double* values) {
  return values[0] * values[1] - values[2];
}

double selfCheckLocalHelper(const double* values);
double selfCheckInlineHelper(const double* values);
double selfCheckColdPart(const double* values);

double selfCheckLocalHelper(const double* values) {
  return helper(values) + values[3];
}

double selfCheckInlineHelper(const double* values) {
  return inlineHelper(values) + values[3];
}

double selfCheckColdPart(const double* values) {
  if (__builtin_expect(values[0] < 0.0, 0)) {
    throw std::invalid_argument("The value is negative.");
  }
  return 2.0 * values[0];
}

}  // namespace codegen
