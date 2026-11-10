// wuzp-expect: error
// wuzp-requires: values=eigen|blitz
// wuzp-static-assert: Unevaluated values cannot be assigned to\. Evaluate them first\.

// An unevaluated value refers to its operands, so assigning to it would write into them (Blitz++) or does not compile (Eigen).
// The value matrix archetype has no unevaluated values: it evaluates every operation (tests/src/archetype_values).

#include <utility>

#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/traits.hpp"

#include "ValueMatrix.hpp"

int main() {
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  using Implementation = MyValueMatrix::ImplementationT;
  using Unevaluated = wuzp::Sum<MyValueMatrix, MyValueMatrix>::type;
  const MyValueMatrix lhs{1, 2, 3};
  const MyValueMatrix rhs{3, 2, 1};
  Unevaluated target{wuzp::Sum<wuzp::AccessImplementation<Implementation>, wuzp::AccessImplementation<Implementation>>::value(
      lhs.getImplementation(), rhs.getImplementation())};
  Unevaluated source{target};
  target = std::move(source);
  return 0;
}
