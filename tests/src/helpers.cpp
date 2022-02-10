#include <gtest/gtest.h>

#include "wuzp/core/helpers.hpp"

using namespace wuzp;  // NOLINT

template <typename T>
struct MyTemplate {};
template <typename T>
using MyTemplateAlias = MyTemplate<T>;
template <typename T0, typename T1>
struct MyOtherTemplate {};
template <typename T0, typename T1>
using MyOtherTemplateAlias = MyOtherTemplate<T1, T0>;

TEST(TestHelpers, IsSpecialization) {  // NOLINT
  static_assert(IsSpecialization<MyTemplate<int>, MyTemplate>::value);
  static_assert(!IsSpecialization<double, MyTemplate>::value);

  static_assert(IsSpecialization<MyTemplateAlias<int>, MyTemplateAlias>::value);
  static_assert(IsSpecialization<MyTemplate<int>, MyTemplateAlias>::value);
  static_assert(IsSpecialization<MyTemplateAlias<int>, MyTemplate>::value);

  static_assert(!IsSpecialization<MyTemplate<int>, MyOtherTemplate>::value);

  // TODO(remod): This trait does not work if there is an alias which switches the template arguments (like for boost).
  // For this reason we introduced the IsQuantity trait.
  //  static_assert(is_specialization<MyOtherTemplateAlias<int, double>, MyOtherTemplateAlias>::value);
}

TEST(TestHelpers, AreSame) {  // NOLINT
  static_assert(areSame<double>);
  static_assert(areSame<double, double>);
  static_assert(areSame<double, double, double>);
  static_assert(!areSame<double, double, int>);
}
