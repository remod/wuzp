#include <cmath>
#include <concepts>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

template <int Rows, int Cols>
using TestValues = ValueMatrix<double, Rows, Cols>;
template <int Rows, int Cols>
using TestUnitMatrix = TypeMatrix<Rows, Cols, Length>;
template <int Rows, int Cols>
using TestQuantities = Quantities<TestValues<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;
template <typename IndicesT, int Rows, int Cols>
using TestTensor = Tensor<IndicesT, TestQuantities<Rows, Cols>, CoordinateSystemA, Relative>;

// Tensors of integers with a single unit, in the canonical shapes of their indices.
template <typename UnitT>
using ScalarI = Tensor<Indices<>, Quantities<ValueMatrix<int, 1, 1>, TypeMatrix<1, 1, UnitT>>, CoordinateSystemA, Relative>;
template <int DimensionV, typename UnitT>
using VectorI =
    Tensor<Indices<Up>, Quantities<ValueMatrix<int, DimensionV, 1>, TypeMatrix<DimensionV, 1, UnitT>>, CoordinateSystemA, Relative>;
template <int DimensionV, typename UnitT>
using CovectorI =
    Tensor<Indices<Down>, Quantities<ValueMatrix<int, 1, DimensionV>, TypeMatrix<1, DimensionV, UnitT>>, CoordinateSystemA, Relative>;
template <typename IndicesT, int DimensionV, typename UnitT>
using MatrixI = Tensor<IndicesT, Quantities<ValueMatrix<int, DimensionV, DimensionV>, TypeMatrix<DimensionV, DimensionV, UnitT>>,
                       CoordinateSystemA, Relative>;

using LengthQuantityI = QuantityI<Length>;
using SquaredLengthQuantityI = QuantityI<Power<Length, Integer<2>>::type>;
using MassQuantityI = QuantityI<Mass>;
using AreaQuantityI = QuantityI<Area>;

template <typename TensorT>
using UnitOf = typename GetType<typename TensorT::UnitsT>::type;

TEST(TestTensor, Zero) {  // NOLINT
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> expected{0.0, 0.0, 0.0};
  EXPECT_EQ((PositionVector3<ValueMatrix, double, CoordinateSystemA>::zero()), expected);
}

TEST(TestTensor, Random) {  // NOLINT
  const auto vector = PositionVector3<ValueMatrix, double, CoordinateSystemA>::random();
  EXPECT_LE(vector.norm().getValue(), std::sqrt(3.0));
}

// Every named tensor alias has the unit its name promises.
TEST(TestTensor, UnitsOfAliases) {  // NOLINT
  using CS = CoordinateSystemA;
  EXPECT_TRUE((std::is_same_v<UnitOf<DirectionVector3<ValueMatrix, double, CS>>, Dimensionless>));
  EXPECT_TRUE((std::is_same_v<UnitOf<PositionPoint3<ValueMatrix, double, CS>>, Length>));
  EXPECT_TRUE((std::is_same_v<UnitOf<PositionVector3<ValueMatrix, double, CS>>, Length>));
  EXPECT_TRUE((std::is_same_v<UnitOf<VelocityVector3<ValueMatrix, double, CS>>, Velocity>));
  EXPECT_TRUE((std::is_same_v<UnitOf<AccelerationVector3<ValueMatrix, double, CS>>, Acceleration>));
  EXPECT_TRUE((std::is_same_v<UnitOf<AngularVelocityVector3<ValueMatrix, double, CS>>, AngularVelocity>));
  EXPECT_TRUE((std::is_same_v<UnitOf<AngularAccelerationVector3<ValueMatrix, double, CS>>, AngularAcceleration>));
  EXPECT_TRUE((std::is_same_v<UnitOf<ForceVector3<ValueMatrix, double, CS>>, Force>));
  EXPECT_TRUE((std::is_same_v<UnitOf<TorqueVector3<ValueMatrix, double, CS>>, Torque>));
  EXPECT_TRUE((std::is_same_v<UnitOf<TimePoint<ValueMatrix, double, CS>>, Time>));
  EXPECT_TRUE((std::is_same_v<UnitOf<Duration<ValueMatrix, double, CS>>, Time>));
  EXPECT_TRUE((std::is_same_v<UnitOf<ForceCovector3<ValueMatrix, double, CS>>, Force>));
  // Spacetime vectors are expressed in coordinate systems with the Minkowski metric.
  EXPECT_TRUE((std::is_same_v<UnitOf<SpacetimePoint4<ValueMatrix, double, SpacetimeA>>, Length>));
  EXPECT_TRUE((std::is_same_v<UnitOf<SpacetimeVector4<ValueMatrix, double, SpacetimeA>>, Length>));
}

// The aliases have the indices of their names: vectors and points an upper index, covectors a lower one.
TEST(TestTensor, IndicesOfAliases) {  // NOLINT
  using CS = CoordinateSystemA;
  static_assert(std::is_same_v<PositionVector3<ValueMatrix, double, CS>::IndicesT, Indices<Up>>);
  static_assert(std::is_same_v<PositionPoint3<ValueMatrix, double, CS>::IndicesT, Indices<Up>>);
  static_assert(std::is_same_v<HomogeneousCovector<3, ValueMatrix, double, Force, CS>::IndicesT, Indices<Down>>);
  static_assert(std::is_same_v<ForceCovector3<ValueMatrix, double, CS>::IndicesT, Indices<Down>>);
  static_assert(Indices<>::RankV == 0);
  static_assert(Indices<Up, Down>::RankV == 2);
}

// The indices of the results of operations (see Indices.hpp). A contraction removes the last index of the left-hand side and the
// first index of the right-hand side, which must be an upper and a lower one; the transpose swaps the indices, and the inverse
// of a tensor of rank 0 or 2 swaps and flips them (Carroll, eq. 1.56).
TEST(TestTensor, IndicesOfOperations) {  // NOLINT
  static_assert(std::is_same_v<InnerProduct<Indices<Down>, Indices<Up>>::type, Indices<>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Up>, Indices<Down>>::type, Indices<>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Up>, Indices<Down, Down>>::type, Indices<Down>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Up, Down>, Indices<Up>>::type, Indices<Up>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Down, Down>, Indices<Up>>::type, Indices<Down>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Down>, Indices<Up, Up>>::type, Indices<Up>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Up, Down>, Indices<Up, Down>>::type, Indices<Up, Down>>);
  static_assert(std::is_same_v<InnerProduct<Indices<Up, Up>, Indices<Down, Down>>::type, Indices<Up, Down>>);
  static_assert(std::is_same_v<OuterProduct<Indices<Up>, Indices<Down>>::type, Indices<Up, Down>>);
  static_assert(std::is_same_v<OuterProduct<Indices<Down>, Indices<Up>>::type, Indices<Down, Up>>);
  static_assert(std::is_same_v<Product<Indices<>, Indices<Up>>::type, Indices<Up>>);
  static_assert(std::is_same_v<Product<Indices<Down>, Indices<>>::type, Indices<Down>>);
  static_assert(std::is_same_v<Division<Indices<Up, Down>, Indices<>>::type, Indices<Up, Down>>);
  static_assert(std::is_same_v<Transpose<Indices<>>::type, Indices<>>);
  static_assert(std::is_same_v<Transpose<Indices<Up>>::type, Indices<Down>>);
  static_assert(std::is_same_v<Transpose<Indices<Down>>::type, Indices<Up>>);
  static_assert(std::is_same_v<Transpose<Indices<Up, Down>>::type, Indices<Down, Up>>);
  static_assert(std::is_same_v<Inverse<Indices<>>::type, Indices<>>);
  static_assert(std::is_same_v<Inverse<Indices<Up, Down>>::type, Indices<Up, Down>>);
  static_assert(std::is_same_v<Inverse<Indices<Down, Up>>::type, Indices<Down, Up>>);
  static_assert(std::is_same_v<Inverse<Indices<Down, Down>>::type, Indices<Up, Up>>);
  static_assert(std::is_same_v<Inverse<Indices<Up, Up>>::type, Indices<Down, Down>>);
  static_assert(std::is_same_v<Determinant<Indices<>>::type, Indices<>>);
  static_assert(std::is_same_v<Determinant<Indices<Up, Down>>::type, Indices<>>);
  static_assert(std::is_same_v<Determinant<Indices<Down, Up>>::type, Indices<>>);
}

// The absoluteness of the results of operations (see Absolute.hpp): p - q is a vector, p + v, v + p and p - v are points, and the
// other operations are only defined for vectors, whose results are vectors. The operations that depend on the origin reject
// points (compile tests Tensor.*OfPoint).
TEST(TestTensor, AbsolutenessOfOperations) {  // NOLINT
  static_assert(std::is_same_v<Sum<Relative, Relative>::type, Relative>);
  static_assert(std::is_same_v<Sum<Absolute, Relative>::type, Absolute>);
  static_assert(std::is_same_v<Sum<Relative, Absolute>::type, Absolute>);
  static_assert(std::is_same_v<Difference<Relative, Relative>::type, Relative>);
  static_assert(std::is_same_v<Difference<Absolute, Relative>::type, Absolute>);
  static_assert(std::is_same_v<Difference<Absolute, Absolute>::type, Relative>);
  static_assert(std::is_same_v<Negation<Relative>::type, Relative>);
  static_assert(std::is_same_v<Product<Relative, Relative>::type, Relative>);
  static_assert(std::is_same_v<Division<Relative, Relative>::type, Relative>);
  static_assert(std::is_same_v<Inverse<Relative>::type, Relative>);
  static_assert(std::is_same_v<Norm<Relative>::type, Relative>);
  static_assert(std::is_same_v<MakeRelative<Absolute>::type, Relative>);
  static_assert(std::is_same_v<MakeAbsolute<Relative>::type, Absolute>);
  static_assert(IsAbsolute<Absolute>::value);
  static_assert(!IsAbsolute<Relative>::value);
}

// The metric of a coordinate system is its second template parameter, Euclidean by default. It is part of the type, so
// coordinate systems with the same identifier and different metrics do not match (compile test Tensor.SumMetricMismatch).
TEST(TestTensor, MetricOfCoordinateSystems) {  // NOLINT
  static_assert(std::is_same_v<CoordinateSystemA, CoordinateSystem<CoordinateSystemAId, Euclidean>>);
  static_assert(std::is_same_v<Metric<CoordinateSystemA>::type, Euclidean>);
  static_assert(std::is_same_v<Metric<SpacetimeA>::type, Minkowski>);
  static_assert(!std::is_same_v<CoordinateSystem<SpacetimeAId, Euclidean>, SpacetimeA>);
  static_assert(EuclideanCoordinateSystem<CoordinateSystemA>);
  static_assert(!EuclideanCoordinateSystem<SpacetimeA>);
  static_assert(MinkowskiCoordinateSystem<SpacetimeA>);
  static_assert(!MinkowskiCoordinateSystem<CoordinateSystemA>);
  static_assert(AnyCoordinateSystem<SpacetimeA>);
  // An identifier is not a coordinate system, so it has no metric.
  static_assert(!EuclideanCoordinateSystem<CoordinateSystemAId>);
  static_assert(!MinkowskiCoordinateSystem<SpacetimeAId>);
}

// The operations that use the Euclidean metric on vectors and covectors are rejected in a Minkowski coordinate system (compile
// tests Tensor.*InMinkowskiCoordinateSystem), but not on tensors of rank 0 and 2, for which they need no metric: the norm of a
// scalar is its absolute value, its dot product the product, and the transpose of a tensor of rank 2 swaps its indices.
TEST(TestTensor, OperationsWithoutMetricInMinkowskiCoordinateSystem) {  // NOLINT
  using ScalarInSpacetime = Tensor<Indices<>, Quantities<ValueMatrix<int, 1, 1>, TypeMatrix<1, 1, Length>>, SpacetimeA, Relative>;
  EXPECT_EQ(ScalarInSpacetime{-2}.norm(), LengthQuantityI{2});
  EXPECT_EQ(ScalarInSpacetime{-2}.squaredNorm(), SquaredLengthQuantityI{4});
  EXPECT_EQ(ScalarInSpacetime{-2}.dotProduct(ScalarInSpacetime{3}), AreaQuantityI{-6});
  EXPECT_EQ(ScalarInSpacetime{-2}.transpose(), ScalarInSpacetime{-2});
  using LinearMapInSpacetime =
      Tensor<Indices<Up, Down>, Quantities<ValueMatrix<int, 2, 2>, TypeMatrix<2, 2, Dimensionless>>, SpacetimeA, Relative>;
  using TransposedLinearMapInSpacetime =
      Tensor<Indices<Down, Up>, Quantities<ValueMatrix<int, 2, 2>, TypeMatrix<2, 2, Dimensionless>>, SpacetimeA, Relative>;
  EXPECT_EQ((LinearMapInSpacetime{1, 2, 3, 4}.transpose()), (TransposedLinearMapInSpacetime{1, 3, 2, 4}));
  // The pairing of a covector and a vector needs no metric.
  const ForceCovector2<ValueMatrix, int, SpacetimeA> covector{1, 2};
  const SpacetimeVector2<ValueMatrix, int, SpacetimeA> vector{3, 4};
  EXPECT_EQ(innerProduct(covector, vector).getQuantity(), QuantityI<Energy>{11});
}

TEST(TestTensor2, Dimension) {  // NOLINT
  static_assert(Dimension<TestTensor<Indices<>, 1, 1>>::value == 0);
  static_assert(Dimension<TestTensor<Indices<Up>, 3, 1>>::value == 1);
  static_assert(Dimension<TestTensor<Indices<Down>, 1, 2>>::value == 1);
  static_assert(Dimension<TestTensor<Indices<Up, Down>, 3, 3>>::value == 2);
}

TEST(TestTensor2, Rows) {  // NOLINT
  static_assert(Rows<TestTensor<Indices<Up>, 3, 1>>::value == 3);
  static_assert(Rows<TestTensor<Indices<Down>, 1, 3>>::value == 1);
}

TEST(TestTensor2, Cols) {  // NOLINT
  static_assert(Cols<TestTensor<Indices<Up>, 3, 1>>::value == 1);
  static_assert(Cols<TestTensor<Indices<Down>, 1, 2>>::value == 2);
}

TEST(TestTensor2, GetTag) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  using MyUnitMatrix = TypeMatrix<3, 1, Length, Area, Volume>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  static_assert(std::is_same_v<GetTag<MyTensor, MyTensor>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyTensor, const MyTensor&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyTensor&&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyQuantityMatrix>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, QuantityD<Length>>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, QuantityD<Length>, QuantityD<Length>>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, MyValueMatrix>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, double>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyTensor, double, int>::type, FromImplementationArgs>);
}

TEST(TestTensor2, EmptyConstructor) {  // NOLINT
  [[maybe_unused]] const MatrixI<Indices<Up, Down>, 2, Length> tensor;
}

TEST(TestTensor2, ConstructImplicitlyFromTensor) {  // NOLINT
  using MyTensor = VectorI<3, Length>;
  const MyTensor tensor1{1, 2, 3};
  const MyTensor tensor2{6, 5, 4};
  // Use = instead of {} to trigger implicit conversion.
  const MyTensor tensor3 = tensor1 + tensor2;
  const MyTensor tensor = tensor3;
  EXPECT_EQ(tensor, tensor3);
  EXPECT_EQ(tensor, (MyTensor{7, 7, 7}));
}

TEST(TestTensor2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyTensor = VectorI<3, Length>;
  const MyTensor::QuantitiesT quantities{1, 2, 3};
  const MyTensor tensor{quantities};
  EXPECT_EQ(tensor.getQuantities(), quantities);
}

TEST(TestTensor2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyTensor = VectorI<3, Length>;
  const MyTensor::ValuesT values{1, 2, 3};
  const MyTensor tensor{values};
  EXPECT_EQ(tensor.getValues(), values);
}

TEST(TestTensor2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyTensor = VectorI<3, Length>;
  const MyTensor::ValuesT values{1, 2, 3};
  const MyTensor tensor{1, 2, 3};
  EXPECT_EQ(tensor.getValues(), values);
}

// A tensor with an upper index may store a batch of vectors or points, one per column, e.g. a point cloud. A batch of points
// follows the rules of points column by column: p - q is a batch of vectors, p + v, v + p and p - v are batches of points.
TEST(TestTensor2, BatchOfPoints) {  // NOLINT
  using PointCloud = Tensor<Indices<Up>, Quantities<ValueMatrix<int, 3, 2>, TypeMatrix<3, 2, Length>>, CoordinateSystemA, Absolute>;
  using Displacements = Tensor<Indices<Up>, Quantities<ValueMatrix<int, 3, 2>, TypeMatrix<3, 2, Length>>, CoordinateSystemA, Relative>;
  const PointCloud points{1, 4, 2, 5, 3, 6};
  const Displacements displacements{1, 1, 1, 1, 1, 1};
  const PointCloud moved{2, 5, 3, 6, 4, 7};
  EXPECT_EQ(points + displacements, moved);
  EXPECT_EQ(displacements + points, moved);
  EXPECT_EQ(moved - displacements, points);
  EXPECT_EQ(moved - points, displacements);
  EXPECT_NE(moved, points);
  // The affine combination p + (q - p) * t of two batches of points, column by column, e.g. their midpoints.
  using PointCloudD = Tensor<Indices<Up>, Quantities<ValueMatrix<double, 3, 2>, TypeMatrix<3, 2, Length>>, CoordinateSystemA, Absolute>;
  const PointCloudD starts{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const PointCloudD ends{3.0, 8.0, 4.0, 9.0, 5.0, 10.0};
  EXPECT_EQ(starts + (ends - starts) * 0.5, (PointCloudD{2.0, 6.0, 3.0, 7.0, 4.0, 8.0}));
}

// A batch of vectors supports sums, differences, comparisons, and products and quotients with scalars, which apply to each
// vector. The operations that are only defined for a single vector reject it (compile tests Tensor.*OfBatch).
TEST(TestTensor2, BatchOfVectors) {  // NOLINT
  using Displacements = Tensor<Indices<Up>, Quantities<ValueMatrix<double, 3, 2>, TypeMatrix<3, 2, Length>>, CoordinateSystemA, Relative>;
  using Areas = Tensor<Indices<Up>, Quantities<ValueMatrix<double, 3, 2>, TypeMatrix<3, 2, Area>>, CoordinateSystemA, Relative>;
  const Displacements displacements{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const Displacements doubled{2.0, 8.0, 4.0, 10.0, 6.0, 12.0};
  EXPECT_EQ(displacements + displacements, doubled);
  EXPECT_EQ(doubled - displacements, displacements);
  EXPECT_NE(doubled, displacements);
  EXPECT_TRUE(displacements.isApprox(Displacements{1.0, 4.0, 2.0, 5.0, 3.0, 6.0 + 1e-14}, 1e-12));
  EXPECT_EQ(2 * displacements, doubled);
  EXPECT_EQ(doubled / 2, displacements);
  EXPECT_EQ(displacements * QuantityD<Length>{2.0}, (Areas{2.0, 8.0, 4.0, 10.0, 6.0, 12.0}));
  const Tensor<Indices<>, TestQuantities<1, 1>, CoordinateSystemA, Relative> scalar{2.0};
  EXPECT_EQ(scalar * displacements, (Areas{2.0, 8.0, 4.0, 10.0, 6.0, 12.0}));
  EXPECT_EQ((Areas{2.0, 8.0, 4.0, 10.0, 6.0, 12.0} / scalar), displacements);
}

// A tensor of rank 2 with different units, in the canonical shape: a square matrix.
class TestTensorF : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 3, 3>;
  using MyUnitMatrix = TypeMatrix<3, 3, Length, Area, Volume, Mass, Force, Energy, Time, Velocity, Acceleration>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<Indices<Up, Down>, MyQuantityMatrix, CoordinateSystemA, Relative>;

  MyTensor& getTensor() { return tensor_; }

 private:
  MyTensor tensor_{1, 2, 3, 4, 5, 6, 7, 8, 9};
};

class TestHomogeneousTensorF : public ::testing::Test {
 public:
  using MyTensor = MatrixI<Indices<Up, Down>, 3, Length>;

  MyTensor& getTensor() { return tensor_; }

 private:
  MyTensor tensor_{1, 2, 3, 4, 5, 6, 7, 8, 9};
};

TEST_F(TestTensorF, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getTensor(), getTensor());
}

TEST_F(TestTensorF, StreamOperator) {  // NOLINT
  EXPECT_EQ(toString(getTensor()), toString(getTensor().getQuantities()));
}

TEST_F(TestTensorF, GetQuantityCompileTime) {  // NOLINT
  EXPECT_EQ((getTensor().getQuantity<0, 0>()), LengthQuantityI{1});
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI{4});
}

TEST_F(TestHomogeneousTensorF, GetQuantityRunTime) {  // NOLINT
  EXPECT_EQ(getTensor().getQuantity(0, 0), LengthQuantityI{1});
  EXPECT_EQ(getTensor().getQuantity(1, 0), LengthQuantityI{4});
}

TEST_F(TestHomogeneousTensorF, SetQuantityCompileTime) {  // NOLINT
  getTensor().setQuantity(1, 0, LengthQuantityI{1});
  EXPECT_EQ(getTensor().getQuantity(1, 0), LengthQuantityI{1});
}

TEST_F(TestTensorF, SetQuantityRunTime) {  // NOLINT
  getTensor().setQuantity<1, 0>(MassQuantityI{1});
  EXPECT_EQ((getTensor().getQuantity<1, 0>()), MassQuantityI{1});
}

TEST_F(TestTensorF, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = Quantities<MyValueMatrixB, MyUnitMatrixB>;
  using MyTensorB = Tensor<Indices<Up, Down>, MyQuantityMatrixB, CoordinateSystemA, Relative>;
  const MyTensorB tensorB{2, 3, 5, 6};
  EXPECT_EQ((getTensor().block<0, 1, 2, 2>()), tensorB);
}

// The transpose swaps the indices of a tensor of rank 2: (T^T)_j^i = T^i_j.
TEST_F(TestTensorF, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 3>;
  using MyUnitMatrixT = TypeMatrix<3, 3, Length, Mass, Time, Area, Force, Velocity, Volume, Energy, Acceleration>;
  using MyQuantityMatrixT = Quantities<MyValueMatrixT, MyUnitMatrixT>;
  using MyTensorT = Tensor<Indices<Down, Up>, MyQuantityMatrixT, CoordinateSystemA, Relative>;
  const MyTensorT tensorT{1, 4, 7, 2, 5, 8, 3, 6, 9};
  EXPECT_EQ(getTensor().transpose(), tensorT);
}

// The transpose of a vector, a column, is a covector, a row, and vice versa.
TEST(TestTensor2, TransposeOfVector) {  // NOLINT
  const VectorI<3, Length> vector{1, 2, 3};
  const CovectorI<3, Length> covector{1, 2, 3};
  EXPECT_EQ(vector.transpose(), covector);
  EXPECT_EQ(covector.transpose(), vector);
}

// The transpose of a tensor of rank 2 swaps its indices also if they are both upper or both lower, which keeps them, and the
// transpose of a tensor of rank 0 is the tensor itself.
TEST(TestTensor2, TransposeOfOtherIndices) {  // NOLINT
  EXPECT_EQ((MatrixI<Indices<Down, Up>, 2, Length>{1, 2, 3, 4}.transpose()), (MatrixI<Indices<Up, Down>, 2, Length>{1, 3, 2, 4}));
  EXPECT_EQ((MatrixI<Indices<Up, Up>, 2, Length>{1, 2, 3, 4}.transpose()), (MatrixI<Indices<Up, Up>, 2, Length>{1, 3, 2, 4}));
  EXPECT_EQ((MatrixI<Indices<Down, Down>, 2, Length>{1, 2, 3, 4}.transpose()), (MatrixI<Indices<Down, Down>, 2, Length>{1, 3, 2, 4}));
  EXPECT_EQ(ScalarI<Length>{2}.transpose(), ScalarI<Length>{2});
}

TEST_F(TestTensorF, Sum) {  // NOLINT
  const auto result{getTensor().getQuantities() + getTensor().getQuantities()};
  EXPECT_EQ(sum(getTensor(), getTensor()).getQuantities(), result);
  EXPECT_EQ((getTensor() + getTensor()).getQuantities(), result);
}

TEST_F(TestTensorF, Difference) {  // NOLINT
  const auto result{getTensor().getQuantities() - getTensor().getQuantities()};
  EXPECT_EQ(difference(getTensor(), getTensor()).getQuantities(), result);
  EXPECT_EQ((getTensor() - getTensor()).getQuantities(), result);
}

class TestPointF : public ::testing::Test {
 public:
  using MyQuantityMatrix = Quantities<ValueMatrix<int, 3, 1>, TypeMatrix<3, 1, Length, Mass, Time>>;
  using MyTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyAbsoluteTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;

  const MyTensor& getTensor() const { return tensor_; }
  const MyAbsoluteTensor& getAbsoluteTensor() const { return absoluteTensor_; }

 private:
  MyTensor tensor_{1, 2, 3};
  MyAbsoluteTensor absoluteTensor_{1, 4, 2};
};

TEST_F(TestPointF, SumRelativeAndAbsolute) {  // NOLINT
  const auto result{getTensor().getQuantities() + getAbsoluteTensor().getQuantities()};
  EXPECT_EQ(sum(getTensor(), getAbsoluteTensor()).getQuantities(), result);
  EXPECT_EQ((getTensor() + getAbsoluteTensor()).getQuantities(), result);
}

TEST_F(TestPointF, SumAbsoluteAndRelative) {  // NOLINT
  const auto result{getTensor().getQuantities() + getAbsoluteTensor().getQuantities()};
  EXPECT_EQ(sum(getAbsoluteTensor(), getTensor()).getQuantities(), result);
  EXPECT_EQ((getAbsoluteTensor() + getTensor()).getQuantities(), result);
}

TEST_F(TestPointF, DifferenceAbsoluteAndRelative) {  // NOLINT
  const auto result{getAbsoluteTensor().getQuantities() - getTensor().getQuantities()};
  EXPECT_EQ(difference(getAbsoluteTensor(), getTensor()).getQuantities(), result);
  EXPECT_EQ((getAbsoluteTensor() - getTensor()).getQuantities(), result);
}

TEST_F(TestPointF, DifferenceAbsoluteAndAbsolute) {  // NOLINT
  const auto result{getAbsoluteTensor().getQuantities() - getAbsoluteTensor().getQuantities()};
  EXPECT_EQ(difference(getAbsoluteTensor(), getAbsoluteTensor()).getQuantities(), result);
  EXPECT_EQ((getAbsoluteTensor() - getAbsoluteTensor()).getQuantities(), result);
}

// makeRelative() is the vector from the origin of the coordinate system to a point, makeAbsolute() the point at a vector from
// the origin: both keep the components.
TEST_F(TestPointF, MakeRelativeAndMakeAbsolute) {  // NOLINT
  EXPECT_EQ(makeRelative(getAbsoluteTensor()), (MyTensor{1, 4, 2}));
  EXPECT_EQ(makeAbsolute(getTensor()), (MyAbsoluteTensor{1, 2, 3}));
}

// The difference of two points is a vector, the sum of a point and a vector and the difference of a point and a vector are
// points.
TEST_F(TestPointF, AbsolutenessOfResults) {  // NOLINT
  EXPECT_EQ(getAbsoluteTensor() - getAbsoluteTensor(), (MyTensor{0, 0, 0}));
  EXPECT_EQ(getAbsoluteTensor() + getTensor(), (MyAbsoluteTensor{2, 6, 5}));
  EXPECT_EQ(getTensor() + getAbsoluteTensor(), (MyAbsoluteTensor{2, 6, 5}));
  EXPECT_EQ(getAbsoluteTensor() - getTensor(), (MyAbsoluteTensor{0, 2, -1}));
}

// An affine combination of points is written as a point plus a vector: p + (q - p) * t. It does not depend on the origin, unlike
// p * (1 - t) + q * t, which does not compile (compile tests Tensor.*OfPoint).
TEST(TestTensor2, AffineCombinationOfPoints) {  // NOLINT
  using MyPoint = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const MyPoint start{1.0, 2.0, 3.0};
  const MyPoint end{3.0, 6.0, 11.0};
  EXPECT_EQ(start + (end - start) * 0.5, (MyPoint{2.0, 4.0, 7.0}));
  EXPECT_EQ(start + (end - start) * 0.25, (MyPoint{1.5, 3.0, 5.0}));
  EXPECT_EQ(end - (end - start) * 0.5, (MyPoint{2.0, 4.0, 7.0}));
}

// An absolute scalar, e.g. a time point, is a point in a one-dimensional coordinate system whose origin is its zero, e.g. an
// epoch: the difference of two time points is a duration.
TEST(TestTensor2, TimePoints) {  // NOLINT
  using TimePointD = TimePoint<ValueMatrix, double, CoordinateSystemA>;
  using DurationD = Duration<ValueMatrix, double, CoordinateSystemA>;
  const TimePointD start{2.0};
  const TimePointD end{5.0};
  EXPECT_EQ(end - start, DurationD{3.0});
  EXPECT_EQ(start + DurationD{3.0}, end);
  EXPECT_EQ(start + (end - start) / 3.0, TimePointD{3.0});
}

TEST(TestTensor2, ProductWithBuiltinValue) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  using MyValue = int;
  const MyValue value{2};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values * value};
  EXPECT_EQ(product(tensor, value), result);
  EXPECT_EQ(product(value, tensor), result);
  EXPECT_EQ(tensor * value, result);
  EXPECT_EQ(value * tensor, result);
}

// An arithmetic scalar of another type is converted into the value type of the tensor, if their common type is the value type, e.g. 2 * v
// for a vector of doubles. Compile tests Tensor.ProductWithWiderScalar and Tensor.DivisionByWiderScalar check the others.
TEST(TestTensor2, ProductWithBuiltinValueOfAnotherType) {  // NOLINT
  using MyTensor = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const MyTensor tensor{1.0, 2.0, 3.0};
  EXPECT_EQ(2 * tensor, (MyTensor{2.0, 4.0, 6.0}));
  EXPECT_EQ(tensor * 2, (MyTensor{2.0, 4.0, 6.0}));
  EXPECT_EQ(tensor * 2.0F, (MyTensor{2.0, 4.0, 6.0}));
  EXPECT_EQ(tensor / 2, (MyTensor{0.5, 1.0, 1.5}));
  static_assert(std::is_same_v<decltype(2 * tensor), MyTensor>);
  static_assert(std::is_same_v<decltype(tensor / 2), MyTensor>);
  // An int scalar with float values, and a short scalar with int values. Compile tests Tensor.ProductWithWider*Scalar check that
  // a double scalar with float values and a long long scalar with int values are rejected.
  using FloatTensor = PositionVector3<ValueMatrix, float, CoordinateSystemA>;
  EXPECT_EQ(FloatTensor(1.0F, 2.0F, 3.0F) * 2, (FloatTensor{2.0F, 4.0F, 6.0F}));
  static_assert(std::is_same_v<decltype(FloatTensor::zero() * 2), FloatTensor>);
  using IntTensor = PositionVector3<ValueMatrix, int, CoordinateSystemA>;
  const short scalar{2};
  EXPECT_EQ(scalar * IntTensor(1, 2, 3), (IntTensor{2, 4, 6}));
  EXPECT_EQ(IntTensor(2, 4, 6) / scalar, (IntTensor{1, 2, 3}));
  static_assert(std::is_same_v<decltype(scalar * IntTensor::zero()), IntTensor>);
}

TEST(TestTensor2, ProductWithValue) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  const ValueI value{2};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values * value};
  EXPECT_EQ(product(tensor, value), result);
  EXPECT_EQ(product(value, tensor), result);
  EXPECT_EQ(tensor * value, result);
  EXPECT_EQ(value * tensor, result);
}

TEST(TestTensor2, ProductWithQuantity) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  using MyTensorResult = VectorI<4, Area>;
  using MyValue = int;
  const MyValue value{2};
  const LengthQuantityI quantity{value};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values * value};
  EXPECT_EQ(product(tensor, quantity), result);
  EXPECT_EQ(product(quantity, tensor), result);
  EXPECT_EQ(tensor * quantity, result);
  EXPECT_EQ(quantity * tensor, result);
}

// A tensor of rank 0, e.g. a scalar field, multiplies a tensor of any rank, also a matrix.
TEST(TestTensor2, ProductWithTensorOfRank0) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  using MyTensorResult = VectorI<4, Area>;
  const ScalarI<Length> scalar{2};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values * 2};
  EXPECT_EQ(product(tensor, scalar), result);
  EXPECT_EQ(product(scalar, tensor), result);
  EXPECT_EQ(tensor * scalar, result);
  EXPECT_EQ(scalar * tensor, result);
  EXPECT_EQ(scalar * scalar, (ScalarI<Area>{4}));
  const ScalarI<Force> force{2};
  EXPECT_EQ((force * MatrixI<Indices<Up, Down>, 2, Length>{-1, 3, 4, 2}), (MatrixI<Indices<Up, Down>, 2, Torque>{-2, 6, 8, 4}));
}

// The elementwise product of the components of two tensors is not a tensor, so it is a quantity matrix.
TEST(TestTensor2, CwiseProduct) {  // NOLINT
  using MyTensor = MatrixI<Indices<Up, Down>, 2, Length>;
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const Quantities<ValueMatrix<int, 2, 2>, TypeMatrix<2, 2, Area>> result{values * values};
  EXPECT_EQ(cwiseProduct(tensor, tensor), result);
  EXPECT_EQ(cwiseProduct(VectorI<3, Length>{1, 2, 3}, VectorI<3, Mass>{3, 2, 1}),
            (Quantities<ValueMatrix<int, 3, 1>, TypeMatrix<3, 1, Product<Length, Mass>::type>>{3, 4, 3}));
}

TEST(TestTensor2, CwiseQuotient) {  // NOLINT
  const VectorI<3, Length> lhs{6, 4, 3};
  const VectorI<3, Time> rhs{3, 2, 1};
  EXPECT_EQ(cwiseQuotient(lhs, rhs), (Quantities<ValueMatrix<int, 3, 1>, TypeMatrix<3, 1, Velocity>>{2, 2, 3}));
}

// The elementwise reciprocal of the components of a tensor is not a tensor, so it is a quantity matrix, like cwiseProduct.
TEST(TestTensor2, CwiseInverse) {  // NOLINT
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 4.0};
  EXPECT_TRUE(vector.cwiseInverse().isApprox(Quantities<TestValues<3, 1>, TypeMatrix<3, 1, Inverse<Length>::type>>{1.0, 0.5, 0.25}, 1e-12));
  using Metric = Tensor<Indices<Down, Down>, TestQuantities<2, 2>, CoordinateSystemA, Relative>;
  EXPECT_TRUE((Metric{1.0, 2.0, 4.0, 8.0}.cwiseInverse().isApprox(
      Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Inverse<Length>::type>>{1.0, 0.5, 0.25, 0.125}, 1e-12)));
  // The components of a covector stay a row, unlike those of the matrix inverse, and the units are inverted elementwise.
  const Tensor<Indices<Down>, Quantities<TestValues<1, 3>, TypeMatrix<1, 3, Length, Mass, Time>>, CoordinateSystemA, Relative> covector{
      1.0, 2.0, 4.0};
  EXPECT_TRUE(covector.cwiseInverse().isApprox(
      Quantities<TestValues<1, 3>, TypeMatrix<1, 3, Inverse<Length>::type, Inverse<Mass>::type, Inverse<Time>::type>>{1.0, 0.5, 0.25},
      1e-12));
}

// The inner product contracts the last index of the left-hand side with the first index of the right-hand side, whatever the
// shape of the operands, and stores its result in the canonical shape.
TEST(TestTensor2, InnerProductOfLinearMapAndVector) {  // NOLINT
  const MatrixI<Indices<Up, Down>, 3, Length> lhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  const VectorI<3, Force> rhs{-2, -1, 0};
  EXPECT_EQ(innerProduct(lhs, rhs), (VectorI<3, Torque>{-4, -13, -22}));
  EXPECT_EQ(lhs.innerProduct(rhs), (VectorI<3, Torque>{-4, -13, -22}));
}

TEST(TestTensor2, InnerProductOfCovectorAndVector) {  // NOLINT
  const CovectorI<3, Force> lhs{1, 2, 3};
  const VectorI<3, Length> rhs{4, 5, 6};
  EXPECT_EQ(innerProduct(lhs, rhs), (ScalarI<Torque>{32}));
}

// v^i w_i is a scalar, like w_i v^i: the only index of a vector is both its first and its last index. The vector, a column, is
// transposed into a row before the product of the matrices, which was the outer product before (#71).
TEST(TestTensor2, InnerProductOfVectorAndCovector) {  // NOLINT
  const VectorI<3, Length> lhs{4, 5, 6};
  const CovectorI<3, Force> rhs{1, 2, 3};
  EXPECT_EQ(innerProduct(lhs, rhs), (ScalarI<Product<Length, Force>::type>{32}));
}

// v^i M_ij is a covector, a row.
TEST(TestTensor2, InnerProductOfVectorAndBilinearForm) {  // NOLINT
  const VectorI<3, Velocity> lhs{1, 0, -1};
  const MatrixI<Indices<Down, Down>, 3, Mass> rhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  using Momentum = Product<Velocity, Mass>::type;
  EXPECT_EQ(innerProduct(lhs, rhs), (CovectorI<3, Momentum>{-6, -6, -6}));
  // M_ij v^i v^j, twice the kinetic energy, is a scalar.
  EXPECT_EQ(innerProduct(lhs, innerProduct(rhs, lhs)), (ScalarI<Product<Velocity, Product<Mass, Velocity>::type>::type>{0}));
}

// v^i T_i^j is a vector, so the row v^T T is transposed into a column.
TEST(TestTensor2, InnerProductOfVectorAndMixedTensor) {  // NOLINT
  const VectorI<3, Force> lhs{1, 0, -1};
  const MatrixI<Indices<Down, Up>, 3, Length> rhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(innerProduct(lhs, rhs), (VectorI<3, Torque>{-6, -6, -6}));
}

// T^ij w_j is a vector: the covector, a row, is transposed into a column before the product.
TEST(TestTensor2, InnerProductOfTensorWithTwoUpperIndicesAndCovector) {  // NOLINT
  const MatrixI<Indices<Up, Up>, 3, Length> lhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  const CovectorI<3, Force> rhs{1, 0, -1};
  EXPECT_EQ(innerProduct(lhs, rhs), (VectorI<3, Torque>{-2, -2, -2}));
}

// T_i^j w_j is a covector, so the column T w^T is transposed into a row.
TEST(TestTensor2, InnerProductOfMixedTensorAndCovector) {  // NOLINT
  const MatrixI<Indices<Down, Up>, 3, Length> lhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  const CovectorI<3, Force> rhs{1, 0, -1};
  EXPECT_EQ(innerProduct(lhs, rhs), (CovectorI<3, Torque>{-2, -2, -2}));
}

TEST(TestTensor2, InnerProductOfCovectorAndLinearMap) {  // NOLINT
  const CovectorI<3, Force> lhs{1, 0, -1};
  const MatrixI<Indices<Up, Down>, 3, Length> rhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(innerProduct(lhs, rhs), (CovectorI<3, Torque>{-6, -6, -6}));
}

// M_ij v^j is a covector, so the column M v is transposed into a row.
TEST(TestTensor2, InnerProductOfBilinearFormAndVector) {  // NOLINT
  const MatrixI<Indices<Down, Down>, 3, Mass> lhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  const VectorI<3, Velocity> rhs{1, 0, -1};
  using Momentum = Product<Mass, Velocity>::type;
  EXPECT_EQ(innerProduct(lhs, rhs), (CovectorI<3, Momentum>{-2, -2, -2}));
  // M_ij v^i v^j, twice the kinetic energy, is a scalar.
  EXPECT_EQ(innerProduct(innerProduct(lhs, rhs), rhs), (ScalarI<Product<Momentum, Velocity>::type>{0}));
}

// w_i T^ij is a vector, so the row w T is transposed into a column.
TEST(TestTensor2, InnerProductOfCovectorAndTensorWithTwoUpperIndices) {  // NOLINT
  const CovectorI<3, Force> lhs{1, 0, -1};
  const MatrixI<Indices<Up, Up>, 3, Length> rhs{1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(innerProduct(lhs, rhs), (VectorI<3, Torque>{-6, -6, -6}));
}

// The inner product of two tensors of rank 2 is the product of their matrices, e.g. T^i_k S^k_j or T^ik M_kj, whatever the other
// two indices. Its indices are the first index of the left-hand side and the second index of the right-hand side.
TEST(TestTensor2, InnerProductOfTensorsOfRank2) {  // NOLINT
  const MatrixI<Indices<Up, Up>, 2, Length> upUp{1, 2, 3, 4};
  const MatrixI<Indices<Down, Up>, 2, Length> downUp{1, 2, 3, 4};
  const MatrixI<Indices<Up, Down>, 2, Length> upDown{1, 2, 3, 4};
  const MatrixI<Indices<Down, Down>, 2, Length> downDown{1, 2, 3, 4};
  const MatrixI<Indices<Up, Up>, 2, Force> forceUpUp{1, 0, 1, 1};
  const MatrixI<Indices<Down, Up>, 2, Force> forceDownUp{1, 0, 1, 1};
  const MatrixI<Indices<Up, Down>, 2, Force> forceUpDown{1, 0, 1, 1};
  const MatrixI<Indices<Down, Down>, 2, Force> forceDownDown{1, 0, 1, 1};
  // An upper index of the left-hand side contracted with a lower index of the right-hand side.
  EXPECT_EQ(innerProduct(upUp, forceDownDown), (MatrixI<Indices<Up, Down>, 2, Torque>{3, 2, 7, 4}));
  EXPECT_EQ(innerProduct(upUp, forceDownUp), (MatrixI<Indices<Up, Up>, 2, Torque>{3, 2, 7, 4}));
  EXPECT_EQ(innerProduct(downUp, forceDownDown), (MatrixI<Indices<Down, Down>, 2, Torque>{3, 2, 7, 4}));
  EXPECT_EQ(innerProduct(downUp, forceDownUp), (MatrixI<Indices<Down, Up>, 2, Torque>{3, 2, 7, 4}));
  // A lower index of the left-hand side contracted with an upper index of the right-hand side.
  EXPECT_EQ(innerProduct(upDown, forceUpDown), (MatrixI<Indices<Up, Down>, 2, Torque>{3, 2, 7, 4}));
  EXPECT_EQ(innerProduct(upDown, forceUpUp), (MatrixI<Indices<Up, Up>, 2, Torque>{3, 2, 7, 4}));
  EXPECT_EQ(innerProduct(downDown, forceUpDown), (MatrixI<Indices<Down, Down>, 2, Torque>{3, 2, 7, 4}));
  EXPECT_EQ(innerProduct(downDown, forceUpUp), (MatrixI<Indices<Down, Up>, 2, Torque>{3, 2, 7, 4}));
}

// The outer product v^i w_j: the left-hand side as a column times the right-hand side as a row.
TEST(TestTensor2, OuterProductOfVectorAndCovector) {  // NOLINT
  const VectorI<3, Force> lhs{-1, 3, 4};
  const CovectorI<3, Length> rhs{1, 2, -2};
  EXPECT_EQ(outerProduct(lhs, rhs), (MatrixI<Indices<Up, Down>, 3, Torque>{-1, -2, 2, 3, 6, -6, 4, 8, -8}));
}

TEST(TestTensor2, OuterProductOfCovectorAndVector) {  // NOLINT
  const CovectorI<3, Force> lhs{1, 2, -2};
  const VectorI<3, Length> rhs{-1, 3, 4};
  EXPECT_EQ(outerProduct(lhs, rhs), (MatrixI<Indices<Down, Up>, 3, Torque>{-1, 3, 4, -2, 6, 8, 2, -6, -8}));
}

TEST(TestTensor2, OuterProductOfVectors) {  // NOLINT
  const VectorI<2, Force> lhs{1, 2};
  const VectorI<2, Length> rhs{3, 4};
  EXPECT_EQ(outerProduct(lhs, rhs), (MatrixI<Indices<Up, Up>, 2, Torque>{3, 4, 6, 8}));
}

TEST(TestTensor2, OuterProductOfCovectors) {  // NOLINT
  const CovectorI<2, Force> lhs{1, 2};
  const CovectorI<2, Length> rhs{3, 4};
  EXPECT_EQ(outerProduct(lhs, rhs), (MatrixI<Indices<Down, Down>, 2, Torque>{3, 4, 6, 8}));
}

TEST(TestTensor2, DivisionWithBuiltinValue) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  using MyValue = int;
  const MyValue value{2};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values / value};
  EXPECT_EQ(division(tensor, value), result);
  EXPECT_EQ(tensor / value, result);
}

TEST(TestTensor2, DivisionWithValue) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  const ValueI value{2};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensor result{values / value};
  EXPECT_EQ(division(tensor, value), result);
  EXPECT_EQ(tensor / value, result);
}

TEST(TestTensor2, DivisionWithQuantity) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  using MyTensorResult = VectorI<4, Dimensionless>;
  using MyValue = int;
  const MyValue value{2};
  const LengthQuantityI quantity{value};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values / value};
  EXPECT_EQ(division(tensor, quantity), result);
  EXPECT_EQ(tensor / quantity, result);
}

TEST(TestTensor2, DivisionWithTensorOfRank0) {  // NOLINT
  using MyTensor = VectorI<4, Length>;
  using MyTensorResult = VectorI<4, Dimensionless>;
  const int value{2};
  const ScalarI<Length> scalar{value};
  const MyTensor::ValuesT values{1, 2, 3, 4};
  const MyTensor tensor{values};
  const MyTensorResult result{values / value};
  EXPECT_EQ(division(tensor, scalar), result);
  EXPECT_EQ(tensor / scalar, result);
}

TEST(TestTensor2, DotProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  const MyTensor lhs{1, 2, 3, 4};
  const MyTensor rhs{0, 3, -1, 2};
  const AreaQuantityI result{11};
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestTensor2, CrossProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  using MyUnitMatrix = TypeMatrix<3, 1, Length>;
  using MyUnitMatrixResult = TypeMatrix<3, 1, Area>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using MyTensorResult = Tensor<Indices<Up>, MyQuantityMatrixResult, CoordinateSystemA, Relative>;
  const MyTensor lhs{1, 2, 3};
  const MyTensor rhs{-1, 3, 4};
  const MyTensorResult result{-1, -7, 5};
  EXPECT_EQ(lhs.crossProduct(rhs), result);
}

TEST(TestTensor2, Norm) {  // NOLINT
  const PositionVector3<ValueMatrix, int, CoordinateSystemA> vector{3, 0, 4};
  EXPECT_EQ(vector.norm(), LengthQuantityI{5});
}

TEST(TestTensor2, SquaredNorm) {  // NOLINT
  const PositionVector3<ValueMatrix, int, CoordinateSystemA> vector{3, 0, 4};
  EXPECT_EQ(vector.squaredNorm(), SquaredLengthQuantityI{25});
}

// The determinant of a tensor of rank 0 or of a mixed tensor of rank 2 is invariant, so it is defined for them.
TEST(TestTensor2, Determinant) {  // NOLINT
  const MatrixI<Indices<Up, Down>, 2, Length> linearMap{-1, 2, 3, 1};
  EXPECT_EQ(linearMap.determinant(), AreaQuantityI{-7});
  const MatrixI<Indices<Down, Up>, 2, Length> transposedLinearMap{-1, 3, 2, 1};
  EXPECT_EQ(transposedLinearMap.determinant(), AreaQuantityI{-7});
  EXPECT_EQ((MatrixI<Indices<Up, Down>, 1, Length>{3}.determinant()), LengthQuantityI{3});
  EXPECT_EQ(ScalarI<Length>{3}.determinant(), LengthQuantityI{3});
}

// The inverse of a tensor of rank 2 swaps and flips its indices, e.g. the inverse of a metric g_ij is g^ij (Carroll, eq. 1.56),
// and its units are (M^-1)_ij = b_i / a_j for M_ij = a_i / b_j (Hart, Multidimensional Analysis, 1995).
TEST(TestTensor2, Inverse) {  // NOLINT
  using Metric = Tensor<Indices<Down, Down>, TestQuantities<2, 2>, CoordinateSystemA, Relative>;
  using InverseMetric =
      Tensor<Indices<Up, Up>, Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Inverse<Length>::type>>, CoordinateSystemA, Relative>;
  const Metric metric{2.0, 0.0, 0.0, 4.0};
  EXPECT_TRUE(metric.inverse().isApprox(InverseMetric{0.5, 0.0, 0.0, 0.25}, 1e-12));
  using LinearMap = Tensor<Indices<Up, Down>, Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Dimensionless>>, CoordinateSystemA, Relative>;
  EXPECT_TRUE((LinearMap{2.0, 1.0, 0.0, 4.0}.inverse().isApprox(LinearMap{0.5, -0.125, 0.0, 0.25}, 1e-12)));
  // A linear map from (mass, dimensionless) to (length, time), M_ij = a_i / b_j with a = (m, s) and b = (kg, 1). Its inverse maps
  // (length, time) back to (mass, dimensionless): (M^-1)_ij = b_i / a_j.
  using MixedLinearMap =
      Tensor<Indices<Up, Down>,
             Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Division<Length, Mass>::type, Length, Division<Time, Mass>::type, Time>>,
             CoordinateSystemA, Relative>;
  using MixedInverse = Tensor<Indices<Up, Down>,
                              Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Division<Mass, Length>::type, Division<Mass, Time>::type,
                                                                      Inverse<Length>::type, Inverse<Time>::type>>,
                              CoordinateSystemA, Relative>;
  EXPECT_TRUE((MixedLinearMap{2.0, 0.0, 0.0, 4.0}.inverse().isApprox(MixedInverse{0.5, 0.0, 0.0, 0.25}, 1e-12)));
  // The inverse of a tensor of rank 0 is its reciprocal.
  EXPECT_TRUE((TestTensor<Indices<>, 1, 1>{4.0}.inverse().isApprox(
      Tensor<Indices<>, Quantities<TestValues<1, 1>, TypeMatrix<1, 1, Inverse<Length>::type>>, CoordinateSystemA, Relative>{0.25}, 1e-12)));
}

// The unit of the determinant is the product of the units on the diagonal, which is the unit of every term if the units describe
// a linear map, M_ij = a_i / b_j: (a_0 a_1) / (b_0 b_1).
TEST(TestTensor2, DeterminantOfHeterogeneousUnits) {  // NOLINT
  // a = (m, s), b = (kg, 1): det = 1 * 4 - 2 * 3 = -2, in m s / kg.
  using LinearMap =
      Tensor<Indices<Up, Down>,
             Quantities<ValueMatrix<int, 2, 2>, TypeMatrix<2, 2, Division<Length, Mass>::type, Length, Division<Time, Mass>::type, Time>>,
             CoordinateSystemA, Relative>;
  EXPECT_EQ((LinearMap{1, 2, 3, 4}.determinant()), (QuantityI<Division<Product<Length, Time>::type, Mass>::type>{-2}));
  // a = (m, 1), b = (1, 1/m): det = 1 * 4 - 2 * 3 = -2, in m^2.
  using TransposedLinearMap =
      Tensor<Indices<Down, Up>, Quantities<ValueMatrix<int, 2, 2>, TypeMatrix<2, 2, Length, Area, Dimensionless, Length>>,
             CoordinateSystemA, Relative>;
  EXPECT_EQ((TransposedLinearMap{1, 2, 3, 4}.determinant()), AreaQuantityI{-2});
}

// The inverse of every index pattern of rank 2: (X, Y) becomes (not Y, not X), and the units M_ij = a_i / b_j become
// (M^-1)_ij = b_i / a_j. The inverse of [[1, 2], [3, 4]] is [[-2, 1], [1.5, -0.5]].
TEST(TestTensor2, InverseOfEachIndexPattern) {  // NOLINT
  // a = (m, s), b = (kg, 1).
  using Units = TypeMatrix<2, 2, Division<Length, Mass>::type, Length, Division<Time, Mass>::type, Time>;
  using InverseUnits =
      TypeMatrix<2, 2, Division<Mass, Length>::type, Division<Mass, Time>::type, Inverse<Length>::type, Inverse<Time>::type>;
  using LinearMap = Tensor<Indices<Up, Down>, Quantities<TestValues<2, 2>, Units>, CoordinateSystemA, Relative>;
  using InverseLinearMap = Tensor<Indices<Up, Down>, Quantities<TestValues<2, 2>, InverseUnits>, CoordinateSystemA, Relative>;
  const InverseLinearMap inverseLinearMap = LinearMap{1.0, 2.0, 3.0, 4.0}.inverse();
  EXPECT_TRUE(inverseLinearMap.isApprox(InverseLinearMap{-2.0, 1.0, 1.5, -0.5}, 1e-12));
  using TransposedLinearMap = Tensor<Indices<Down, Up>, Quantities<TestValues<2, 2>, Units>, CoordinateSystemA, Relative>;
  using InverseTransposedLinearMap = Tensor<Indices<Down, Up>, Quantities<TestValues<2, 2>, InverseUnits>, CoordinateSystemA, Relative>;
  const InverseTransposedLinearMap inverseTransposedLinearMap = TransposedLinearMap{1.0, 2.0, 3.0, 4.0}.inverse();
  EXPECT_TRUE(inverseTransposedLinearMap.isApprox(InverseTransposedLinearMap{-2.0, 1.0, 1.5, -0.5}, 1e-12));
  using UpperTensor = Tensor<Indices<Up, Up>, Quantities<TestValues<2, 2>, Units>, CoordinateSystemA, Relative>;
  using InverseUpperTensor = Tensor<Indices<Down, Down>, Quantities<TestValues<2, 2>, InverseUnits>, CoordinateSystemA, Relative>;
  const InverseUpperTensor inverseUpperTensor = UpperTensor{1.0, 2.0, 3.0, 4.0}.inverse();
  EXPECT_TRUE(inverseUpperTensor.isApprox(InverseUpperTensor{-2.0, 1.0, 1.5, -0.5}, 1e-12));
  using LowerTensor = Tensor<Indices<Down, Down>, Quantities<TestValues<2, 2>, Units>, CoordinateSystemA, Relative>;
  using InverseLowerTensor = Tensor<Indices<Up, Up>, Quantities<TestValues<2, 2>, InverseUnits>, CoordinateSystemA, Relative>;
  const InverseLowerTensor inverseLowerTensor = LowerTensor{1.0, 2.0, 3.0, 4.0}.inverse();
  EXPECT_TRUE(inverseLowerTensor.isApprox(InverseLowerTensor{-2.0, 1.0, 1.5, -0.5}, 1e-12));
  // The product of a linear map and its inverse is the identity, T^i_j (T^-1)^j_k = delta^i_k, whose units are a_i / a_k.
  using Identity =
      Tensor<Indices<Up, Down>,
             Quantities<TestValues<2, 2>,
                        TypeMatrix<2, 2, Dimensionless, Division<Length, Time>::type, Division<Time, Length>::type, Dimensionless>>,
             CoordinateSystemA, Relative>;
  EXPECT_TRUE(innerProduct(LinearMap{1.0, 2.0, 3.0, 4.0}, inverseLinearMap).isApprox(Identity{1.0, 0.0, 0.0, 1.0}, 1e-12));
}

// The inverse of a mass matrix M_ij, which maps accelerations to forces and torques, M_ij = a_i / b_j with a = (N, N m) and
// b = (m/s^2, 1/s^2), has two upper indices and the units b_i / a_j: [[1/kg, 1/(kg m)], [1/(kg m), 1/(kg m^2)]]. Its products
// with the mass matrix are the Kronecker delta, M_ij (M^-1)^jk = delta_i^k with the units a_i / a_k, and (M^-1)^ij M_jk =
// delta^i_k with the units b_i / b_k.
TEST(TestTensor2, InverseOfMassMatrix) {  // NOLINT
  using MassLength = Product<Mass, Length>::type;
  using MassArea = Product<Mass, Area>::type;
  using MassMatrix = Tensor<Indices<Down, Down>, Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Mass, MassLength, MassLength, MassArea>>,
                            CoordinateSystemA, Relative>;
  using InverseMassMatrix = Tensor<Indices<Up, Up>,
                                   Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Inverse<Mass>::type, Inverse<MassLength>::type,
                                                                           Inverse<MassLength>::type, Inverse<MassArea>::type>>,
                                   CoordinateSystemA, Relative>;
  const MassMatrix massMatrix{2.0, 3.0, 3.0, 5.0};
  const InverseMassMatrix inverse = massMatrix.inverse();
  EXPECT_TRUE(inverse.isApprox(InverseMassMatrix{5.0, -3.0, -3.0, 2.0}, 1e-12));
  using LowerDelta =
      Tensor<Indices<Down, Up>, Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Dimensionless, Inverse<Length>::type, Length, Dimensionless>>,
             CoordinateSystemA, Relative>;
  EXPECT_TRUE(innerProduct(massMatrix, inverse).isApprox(LowerDelta{1.0, 0.0, 0.0, 1.0}, 1e-12));
  using UpperDelta =
      Tensor<Indices<Up, Down>, Quantities<TestValues<2, 2>, TypeMatrix<2, 2, Dimensionless, Length, Inverse<Length>::type, Dimensionless>>,
             CoordinateSystemA, Relative>;
  EXPECT_TRUE(innerProduct(inverse, massMatrix).isApprox(UpperDelta{1.0, 0.0, 0.0, 1.0}, 1e-12));
}

namespace {

using VectorInA = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
using VectorInB = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
using PointInA = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;

VectorInA makeVector() {
  return VectorInA{1.0, 2.0, 3.0};
}

int getCoordinateSystemId(const VectorInA& /*vector*/) {
  return 0;
}
int getCoordinateSystemId(const VectorInB& /*vector*/) {
  return 1;
}

}  // namespace

// Tensors have value semantics with every backend: a copy does not share the values of the original, although Blitz++'s copy
// constructor shares them (#60).
TEST(TestTensor, CopyIsIndependentOfTheOriginal) {  // NOLINT
  PointInA original{1.0, 2.0, 3.0};
  const PointInA copy{original};  // From a non-const object.
  const PointInA copyInitialized = original;
  original.setQuantity<0, 0>(QuantityD<Length>{42.0});
  EXPECT_EQ(copy, (PointInA{1.0, 2.0, 3.0}));
  EXPECT_EQ(copyInitialized, (PointInA{1.0, 2.0, 3.0}));
}

// The results of operations are evaluated, so that they do not refer to their operands, and can be stored even if the
// operands are temporaries (#69). A result that referred to a destroyed temporary would read freed memory, which the
// sanitizers job reports (AddressSanitizer: stack-use-after-scope).
TEST(TestTensor, ResultsOfTemporariesCanBeStored) {  // NOLINT
  static_assert(std::is_same_v<decltype(makeVector() + makeVector()), VectorInA>);
  static_assert(std::is_same_v<decltype(makeVector() - makeVector()), VectorInA>);
  static_assert(std::is_same_v<decltype(-makeVector()), VectorInA>);
  static_assert(std::is_same_v<decltype(makeVector().evaluate()), VectorInA>);
  const auto quotient = makeVector() / QuantityD<Dimensionless>{2.0};
  const auto quotientOfValue = makeVector() / 2.0;
  const auto product = makeVector() * QuantityD<Dimensionless>{2.0};
  const auto productWithValue = makeVector() * 2.0;
  const auto sum = makeVector() + makeVector();
  const auto difference = makeVector() - VectorInA{1.0, 1.0, 1.0};
  const auto negation = -makeVector();
  const auto transpose = makeVector().transpose();
  const auto block = makeVector().block<1, 0, 2, 1>();
  const auto evaluated = makeVector().evaluate();
  EXPECT_EQ(quotient, (VectorInA{0.5, 1.0, 1.5}));
  EXPECT_EQ(quotientOfValue, (VectorInA{0.5, 1.0, 1.5}));
  EXPECT_EQ(product, (VectorInA{2.0, 4.0, 6.0}));
  EXPECT_EQ(productWithValue, (VectorInA{2.0, 4.0, 6.0}));
  EXPECT_EQ(sum, (VectorInA{2.0, 4.0, 6.0}));
  EXPECT_EQ(difference, (VectorInA{0.0, 1.0, 2.0}));
  EXPECT_EQ(negation, (VectorInA{-1.0, -2.0, -3.0}));
  EXPECT_EQ(transpose.getValues(), (ValueMatrix<double, 1, 3>{1.0, 2.0, 3.0}));
  EXPECT_EQ(block.getValues(), (ValueMatrix<double, 2, 1>{2.0, 3.0}));
  EXPECT_EQ(evaluated, makeVector());
}

// Only valid conversions are implicit: type traits report the others as invalid, and overloads for different coordinate
// systems are not ambiguous (#70). Direct initialization fails with a static_assert that explains why (compile tests
// Tensor.Convert*).
TEST(TestTensor, ConversionsAreImplicitOnlyIfValid) {  // NOLINT
  static_assert(!std::is_convertible_v<VectorInA, VectorInB>);
  static_assert(!std::convertible_to<VectorInA, VectorInB>);
  static_assert(!std::is_convertible_v<PointInA, VectorInA>);
  static_assert(!std::is_convertible_v<VectorInA, PointInA>);
  static_assert(!std::is_convertible_v<VelocityVector3<ValueMatrix, double, CoordinateSystemA>, VectorInA>);
  // An unevaluated sum, of the type the traits compute, converts implicitly into a vector in the same coordinate system only.
  using UnevaluatedSum = Sum<VectorInA, VectorInA>::type;
  static_assert(!std::is_same_v<UnevaluatedSum, VectorInA>);
  static_assert(std::is_convertible_v<UnevaluatedSum, VectorInA>);
  static_assert(!std::is_convertible_v<UnevaluatedSum, VectorInB>);
  static_assert(requires(const UnevaluatedSum& unevaluatedSum) { getCoordinateSystemId(unevaluatedSum); });
  EXPECT_EQ(getCoordinateSystemId(makeVector() + makeVector()), 0);
  EXPECT_EQ(getCoordinateSystemId(VectorInB{1.0, 2.0, 3.0}), 1);
}

namespace {

template <typename TensorT_>
concept HasGetQuantity = requires(const TensorT_& tensor) { tensor.getQuantity(); };
template <typename TensorT_>
concept HasSetQuantity = requires(TensorT_& tensor) { tensor.setQuantity(QuantityD<Length>{1.0}); };
template <typename TensorT_>
concept HasNorm = requires(const TensorT_& tensor) { tensor.norm(); };
template <typename TensorT_>
concept HasSquaredNorm = requires(const TensorT_& tensor) { tensor.squaredNorm(); };
template <typename TensorT_>
concept HasDeterminant = requires(const TensorT_& tensor) { tensor.determinant(); };

using ScalarInA = Tensor<Indices<>, TestQuantities<1, 1>, CoordinateSystemA, Relative>;
using MatrixInA = Tensor<Indices<Up, Down>, TestQuantities<3, 3>, CoordinateSystemA, Relative>;

}  // namespace

// The members for single quantities only exist for tensors of dimension 0, and the norms for dimensions 0 and 1: their requires
// clauses remove them from the others. The determinant exists for every tensor, like the inverse, and rejects vectors and
// covectors with a static_assert (compile test Tensor.DeterminantOfVector).
TEST(TestTensor, MembersRequireTheirDimension) {  // NOLINT
  static_assert(HasGetQuantity<ScalarInA>);
  static_assert(!HasGetQuantity<VectorInA>);
  static_assert(HasSetQuantity<ScalarInA>);
  static_assert(!HasSetQuantity<VectorInA>);
  static_assert(HasNorm<ScalarInA>);
  static_assert(HasNorm<VectorInA>);
  static_assert(!HasNorm<MatrixInA>);
  static_assert(HasSquaredNorm<ScalarInA>);
  static_assert(HasSquaredNorm<VectorInA>);
  static_assert(!HasSquaredNorm<MatrixInA>);
  static_assert(HasDeterminant<ScalarInA>);
  static_assert(HasDeterminant<MatrixInA>);
}
