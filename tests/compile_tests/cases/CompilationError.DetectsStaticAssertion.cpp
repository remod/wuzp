// wuzp-expect: error
// wuzp-static-assert: This assertion always fails\.

int main() {
  static_assert(sizeof(int) == 0, "This assertion always fails.");
  return 0;
}
