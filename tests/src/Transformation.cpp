#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/Transformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// The base class of the transformations has a protected non-virtual destructor: transformations are never deleted through a
// pointer to it, so they need no virtual table, which made every transformation larger.
TEST(TestTransformation, HasNoVirtualTable) {  // NOLINT
  static_assert(!std::is_polymorphic_v<Transformation>);
  static_assert(!std::has_virtual_destructor_v<Transformation>);
  static_assert(!std::is_destructible_v<Transformation>);
  using ActiveRotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveRotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using ActiveTranslation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveTranslation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  // A transformation is as large as its matrix or vector.
  EXPECT_EQ(sizeof(ActiveRotation), sizeof(ActiveRotation::QuantitiesT));
  EXPECT_EQ(sizeof(PassiveRotation), sizeof(PassiveRotation::QuantitiesT));
  EXPECT_EQ(sizeof(ActiveTranslation), sizeof(ActiveTranslation::TensorT));
  EXPECT_EQ(sizeof(PassiveTranslation), sizeof(PassiveTranslation::TensorT));
}
