// wuzp-expect: error
// wuzp-static-assert: Type mismatch\.

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

using namespace wuzp;

template <typename T>
struct MyType;

template <typename Lhs, typename Rhs>
struct Sum<MyType<Lhs>, MyType<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Type mismatch.");
  using type = Lhs;
};

int main() {
  using type = Sum<TypeMatrix<1, 1, MyType<int>>, TypeMatrix<1, 1, MyType<double>>>::type;
  return 0;
}
