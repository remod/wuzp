#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
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
  using ActiveRotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  static_assert(!std::is_polymorphic_v<Transformation<ActiveRotation>>);
  static_assert(!std::has_virtual_destructor_v<Transformation<ActiveRotation>>);
  static_assert(!std::is_destructible_v<Transformation<ActiveRotation>>);
  using PassiveRotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using ActiveTranslation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveTranslation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  // A transformation is as large as its matrix or vector.
  EXPECT_EQ(sizeof(ActiveRotation), sizeof(ActiveRotation::QuantitiesT));
  EXPECT_EQ(sizeof(PassiveRotation), sizeof(PassiveRotation::QuantitiesT));
  EXPECT_EQ(sizeof(ActiveTranslation), sizeof(ActiveTranslation::TensorT));
  EXPECT_EQ(sizeof(PassiveTranslation), sizeof(PassiveTranslation::TensorT));
}

// A proper rigid transformation is as large as its rotation and its translation. Its empty base classes differ from those of its
// rotation, its first member, since the base class Transformation is a template of the transformation that derives from it: with a
// common empty base, the rotation could not start at the address of the transformation, since two objects of the same type cannot
// have the same address (https://en.cppreference.com/w/cpp/language/ebo), and the transformation was 8 bytes larger with Eigen.
TEST(TestTransformation, ProperRigidTransformationIsAsLargeAsItsRotationAndTranslation) {  // NOLINT
  using ActiveRigid = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using PassiveRigid = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  EXPECT_EQ(sizeof(ActiveRigid), sizeof(ActiveRigid::Rotation) + sizeof(ActiveRigid::Translation));
  EXPECT_EQ(sizeof(PassiveRigid), sizeof(PassiveRigid::Rotation) + sizeof(PassiveRigid::Translation));
#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
  // The sizes themselves depend on the value matrix backend: Eigen stores the values of fixed-size matrices in the object, while
  // e.g. Blitz++ allocates them.
  EXPECT_EQ(sizeof(ActiveRigid::Rotation), 9 * sizeof(double));
  EXPECT_EQ(sizeof(ActiveRigid::Translation), 3 * sizeof(double));
  EXPECT_EQ(sizeof(ActiveRigid), 12 * sizeof(double));
  EXPECT_EQ(sizeof(PassiveRigid), 12 * sizeof(double));
#endif
}

// AnyTransformation tells transformations from the objects they transform, also for a class that derives from a transformation.
TEST(TestTransformation, AnyTransformation) {  // NOLINT
  using Rotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  struct DerivedRotation : Rotation {};
  static_assert(AnyTransformation<Rotation>);
  static_assert(AnyTransformation<PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>>);
  static_assert(AnyTransformation<ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>>);
  static_assert(AnyTransformation<DerivedRotation>);
  static_assert(!AnyTransformation<PositionVector3<ValueMatrix, double, CoordinateSystemA>>);
  static_assert(!AnyTransformation<double>);
}
