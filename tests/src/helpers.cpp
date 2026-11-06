#include <vector>

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

// Records the values that its comma operator is applied to.
class CommaRecorder {
 public:
  explicit CommaRecorder(std::vector<int>& values) : values_(&values) {}
  CommaRecorder operator,(int value) const {
    values_->push_back(value);
    return *this;
  }

 private:
  std::vector<int>* values_;
};

TEST(TestHelpers, ApplyCommas) {  // NOLINT
  std::vector<int> values;
  applyCommas(CommaRecorder{values}, 1, 2, 3);
  EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}
