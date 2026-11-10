#pragma once

#include <concepts>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Integer.hpp"
#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

// Concepts for the template parameters of the core. They check that a parameter has the right kind of type, e.g. that a
// coordinate system is a coordinate system. Invalid combinations of valid types, e.g. matrix sizes that do not match, are
// rejected with a static_assert and a clear message instead.
//
// The second part of this file defines the interface of the backends: ValueMatrixBackend and InvertibleValueMatrixBackend for
// value matrix backends, UnitSystem for units backends. See https://en.cppreference.com/w/cpp/language/constraints for concepts.

namespace wuzp {

//! Any coordinate system: a specialization of `CoordinateSystem`, including the intermediate ones of transformations.
template <typename T>
concept AnyCoordinateSystem = IsSpecialization<T, CoordinateSystem>::value;

//! A coordinate system with the Euclidean metric, the default, e.g. CoordinateSystem<World>: the coordinate system of rotations
//! and proper rigid transformations, whose orthogonality is defined by this metric (see Metric).
template <typename T>
concept EuclideanCoordinateSystem = AnyCoordinateSystem<T> && std::same_as<typename Metric<T>::type, Euclidean>;

//! A coordinate system with the Minkowski metric, e.g. CoordinateSystem<Spacetime, Minkowski>: an inertial frame of special
//! relativity, in which spacetime vectors are expressed and between which Lorentz transformations transform (see Metric).
template <typename T>
concept MinkowskiCoordinateSystem = AnyCoordinateSystem<T> && std::same_as<typename Metric<T>::type, Minkowski>;

//! `Absolute` or `Relative`: whether a tensor is a point or a vector.
template <typename T>
concept AbsoluteOrRelative = std::same_as<T, Absolute> || std::same_as<T, Relative>;

//! Any unit of any units backend: a specialization of `Unit`.
template <typename T>
concept AnyUnit = IsSpecialization<T, Unit>::value;

namespace internal {

template <typename T>
struct IsUnitMatrix : std::false_type {};
template <int RowsV_, int ColsV_, typename... UnitsT_>
struct IsUnitMatrix<TypeMatrixImpl<RowsV_, ColsV_, std::tuple<UnitsT_...>>> : std::bool_constant<(AnyUnit<UnitsT_> && ...)> {};

template <typename T>
struct IsMatrixSize : std::false_type {};
template <int RowsV_, int ColsV_>
struct IsMatrixSize<MatrixSize<RowsV_, ColsV_>> : std::true_type {};

}  // namespace internal

//! Any unit matrix: a `TypeMatrix` of units.
template <typename T>
concept AnyUnitMatrix = internal::IsUnitMatrix<T>::value;

//! Any size of a matrix: a `MatrixSize` with rows and columns.
template <typename T>
concept AnyMatrixSize = internal::IsMatrixSize<T>::value;

//! An arithmetic type, e.g. `int`, `float` or `double`: the type of the elements of value matrices.
template <typename T>
concept ArithmeticValue = std::is_arithmetic_v<T>;

// The following class templates are defined in their own headers, which use the concepts above. Their definitions repeat the
// constraints of these declarations.
template <AnyMatrixSize MatrixSizeT_, ArithmeticValue ValueT_, typename ImplementationT_>
class ValuesWrapper;

//! Any value matrix: a specialization of `ValuesWrapper`.
template <typename T>
concept AnyValuesWrapper = IsSpecialization<T, ValuesWrapper>::value;

//! The alias template ValueMatrix<ValueT_, RowsV_, ColsV_> of any value matrix backend, e.g. wuzp_eigen::ValueMatrix, rather
//! than e.g. the matrices of its linear algebra library. It only checks the kind of the template, which costs no compile time;
//! ValueMatrixBackend checks the traits of the backend.
template <template <typename, int, int> typename ValueMatrixT_>
concept AnyValueMatrix = AnyValuesWrapper<ValueMatrixT_<double, 1, 1>>;

template <AnyValuesWrapper ValuesT_, AnyUnitMatrix UnitsT_>
class Quantities;

//! Any quantity matrix: a specialization of `Quantities`, including `Quantity`.
template <typename T>
concept AnyQuantities = IsSpecialization<T, Quantities>::value;

// An upper and a lower index of a tensor, defined in Indices.hpp.
struct Up;
struct Down;

//! `Up` or `Down`: whether an index of a tensor is upper (contravariant) or lower (covariant).
template <typename T>
concept UpOrDown = std::same_as<T, Up> || std::same_as<T, Down>;

template <UpOrDown... VariancesT_>
struct Indices;

//! Any indices of a tensor: a specialization of `Indices`, e.g. `Indices<Up, Down>`.
template <typename T>
concept AnyIndices = IsSpecialization<T, Indices>::value;

template <AnyIndices IndicesT_, AnyQuantities QuantitiesT_, AnyCoordinateSystem CoordinateSystemT_, AbsoluteOrRelative AbsoluteT_>
class Tensor;

//! Any tensor: a specialization of `Tensor`, including points, vectors and their aliases.
template <typename T>
concept AnyTensor = IsSpecialization<T, Tensor>::value;

// ---------------------------------------------------------------------------------------------------------------------------------
// The backend interface
// ---------------------------------------------------------------------------------------------------------------------------------
//
// A value matrix backend adapts a linear algebra library, e.g. Eigen, by specializing the traits of traits.hpp for
// AccessImplementation<ImplementationT_>, where ImplementationT_ is a matrix or an expression of the library (its
// implementation types), and provides an alias template ValueMatrix<ValueT_, RowsV_, ColsV_> of the ValuesWrapper of its
// matrices. Its specializations are constrained to its own implementation types, so that several value matrix backends can be
// used in one program. A units backend specializes the arithmetic traits for its units, e.g. Product<Unit<...>, Unit<...>>, and
// AccessUnits for its units. A program uses exactly one units backend. The core uses nothing beyond these concepts: the tests
// pass with the archetypes, minimal backends that provide exactly what the concepts require (tests/src/archetype_values and
// tests/src/archetype_units). See "Writing a backend" in CONTRIBUTING.md.
//
// The concepts check the syntax of the traits, on representative matrices and units; the tests check their semantics, e.g. that
// Copy does not share the values. A backend checks the concepts in its tests, e.g.
// `static_assert(wuzp::ValueMatrixBackend<wuzp_eigen::ValueMatrix>);`, so that a backend that misses a trait fails there,
// rather than deep inside the core in the code of its users. The checks instantiate many expressions of the backend, so the
// backends do not check them in their headers, which would cost compile time in every translation unit of their users. A trait
// that is not specialized at all fails with the message of its primary template, e.g. "'Norm' trait not specialized for given
// type."; a specialization with the wrong interface makes the concept unsatisfied, and the notes of the compiler name it.

namespace internal {

//! The implementation type, i.e. the matrix of the linear algebra library, of a value matrix of a backend.
template <template <typename, int, int> typename ValueMatrixT_, typename ValueT_, int RowsV_, int ColsV_>
using ImplementationOf = typename ValueMatrixT_<ValueT_, RowsV_, ColsV_>::ImplementationT;

//! The result of a trait of a value matrix backend, e.g. of Sum<AccessImplementation<ImplementationT_>, ...>: `value` returns
//! `type`, a matrix or a lazy expression of the backend, which Evaluate evaluates into EvaluatedT_. The core moves the result
//! into the values of its type, and the converting constructor of ValuesWrapper constructs the matrix EvaluatedT_ from a lazy
//! result, e.g. when an unevaluated sum is converted into a vector, so a lazy result must be move constructible and the matrix
//! constructible from it.
template <typename ResultT_, typename TraitT_, typename EvaluatedT_>
concept TraitResult =
    std::same_as<ResultT_, typename TraitT_::type> && std::same_as<typename Evaluate<AccessImplementation<ResultT_>>::type, EvaluatedT_> &&
    std::move_constructible<ResultT_> &&
    (std::same_as<ResultT_, EvaluatedT_> ||
     (std::constructible_from<EvaluatedT_, ResultT_> && std::constructible_from<EvaluatedT_, const ResultT_&>));

//! Whether Initialize initializes a matrix with the arguments, as the constructor of ValuesWrapper does with its elements.
template <typename ImplementationT_, typename ArgsTupleT_>
struct IsInitializable : std::false_type {};
template <typename ImplementationT_, typename... ArgsT_>
  requires requires {
    Initialize<ImplementationT_, std::tuple<ArgsT_...>>::run(std::declval<ImplementationT_&>(), std::declval<ArgsT_>()...);
  }
struct IsInitializable<ImplementationT_, std::tuple<ArgsT_...>> : std::true_type {};

//! Random values of the matrices of a value matrix backend, which are only supported for floating point types. A disjunction of
//! concepts rather than a nested requirement of BackendMatrix: Clang 14 checked both operands of a disjunction in a nested
//! requirement, which instantiated Random for integer matrices too, and failed for the backends that only provide it for
//! floating point matrices.
template <typename ImplementationT_, typename ValueT_>
concept BackendRandom =
    !std::floating_point<ValueT_> || requires(ImplementationT_& target) { Random<AccessImplementation<ImplementationT_>>::run(target); };

//! The requirements on the matrices of a value matrix backend with one value type and size: they are allocated, initialized,
//! evaluated, copied and moved, their elements are accessed, and they are combined elementwise, inverted elementwise, transposed,
//! multiplied with their transposes, compared and printed. Lazy results evaluate to the matrix of the backend with the size of
//! the result, which is constructible from them (see TraitResult).
template <typename ImplementationT_, template <typename, int, int> typename ValueMatrixT_, typename ValueT_, int RowsV_, int ColsV_>
concept BackendMatrix =
    std::same_as<ValueMatrixT_<ValueT_, RowsV_, ColsV_>, ValuesWrapper<MatrixSize<RowsV_, ColsV_>, ValueT_, ImplementationT_>> &&
    // The core default constructs a matrix before it initializes it, and moves the results of the traits. It copies and assigns
    // matrices only with Copy and Move, so they need not be copyable or assignable.
    std::default_initializable<ImplementationT_> && std::move_constructible<ImplementationT_> &&
    requires(const ImplementationT_& impl, ImplementationT_& target,
             const ImplementationOf<ValueMatrixT_, ValueT_, ColsV_, RowsV_>& transposed, const ValueT_ value, const int index,
             std::ostream& stream) {
      ReserveMemory<ImplementationT_, MatrixSize<RowsV_, ColsV_>>::run(target);
      requires IsInitializable<ImplementationT_, HomogeneousTuple<RowsV_ * ColsV_, ValueT_>>::value;
      {
        Evaluate<AccessImplementation<ImplementationT_>>::value(impl)
      } -> TraitResult<Evaluate<AccessImplementation<ImplementationT_>>, ImplementationT_>;
      requires std::same_as<typename Rebind<AccessImplementation<ImplementationT_>, ValueT_, RowsV_, ColsV_>::type, ImplementationT_>;
      requires std::same_as<typename Rebind<AccessImplementation<ImplementationT_>, ValueT_, 1, 1>::type,
                            ImplementationOf<ValueMatrixT_, ValueT_, 1, 1>>;
      { Copy<AccessImplementation<ImplementationT_>>::value(impl) } -> std::same_as<ImplementationT_>;
      { Move<AccessImplementation<ImplementationT_>>::construct(target) } -> std::same_as<ImplementationT_>;
      Move<AccessImplementation<ImplementationT_>>::assign(target, target);
      { Coefficient<AccessImplementation<ImplementationT_>>::get(impl, index, index) } -> std::same_as<ValueT_>;
      Coefficient<AccessImplementation<ImplementationT_>>::set(target, index, index, value);
      Zero<AccessImplementation<ImplementationT_>>::run(target);
      {
        Comparison<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::equal(impl, impl)
      } -> std::same_as<bool>;
      {
        Comparison<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::approxEqual(impl, impl, value)
      } -> std::same_as<bool>;
      {
        Sum<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::value(impl, impl)
      } -> TraitResult<Sum<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>, ImplementationT_>;
      // Rebind also takes the expressions of the backend, e.g. for the type of an element of an unevaluated sum.
      requires std::same_as<
          typename Rebind<
              AccessImplementation<typename Sum<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::type>,
              ValueT_, RowsV_, ColsV_>::type,
          ImplementationT_>;
      {
        Difference<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::value(impl, impl)
      } -> TraitResult<Difference<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>, ImplementationT_>;
      {
        Negation<AccessImplementation<ImplementationT_>>::value(impl)
      } -> TraitResult<Negation<AccessImplementation<ImplementationT_>>, ImplementationT_>;
      {
        Product<AccessImplementation<ImplementationT_>, ValueT_>::value(impl, value)
      } -> TraitResult<Product<AccessImplementation<ImplementationT_>, ValueT_>, ImplementationT_>;
      {
        Product<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::value(impl, impl)
      } -> TraitResult<Product<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>, ImplementationT_>;
      {
        Division<AccessImplementation<ImplementationT_>, ValueT_>::value(impl, value)
      } -> TraitResult<Division<AccessImplementation<ImplementationT_>, ValueT_>, ImplementationT_>;
      {
        Division<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>::value(impl, impl)
      } -> TraitResult<Division<AccessImplementation<ImplementationT_>, AccessImplementation<ImplementationT_>>, ImplementationT_>;
      {
        CwiseInverse<AccessImplementation<ImplementationT_>>::value(impl)
      } -> TraitResult<CwiseInverse<AccessImplementation<ImplementationT_>>, ImplementationT_>;
      {
        Transpose<AccessImplementation<ImplementationT_>>::value(impl)
      } -> TraitResult<Transpose<AccessImplementation<ImplementationT_>>, ImplementationOf<ValueMatrixT_, ValueT_, ColsV_, RowsV_>>;
      {
        Block<AccessImplementation<ImplementationT_>, 0, 0, RowsV_, 1>::value(impl)
      } -> TraitResult<Block<AccessImplementation<ImplementationT_>, 0, 0, RowsV_, 1>, ImplementationOf<ValueMatrixT_, ValueT_, RowsV_, 1>>;
      {
        InnerProduct<AccessImplementation<ImplementationT_>,
                     AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, ColsV_, RowsV_>>>::value(impl, transposed)
      } -> TraitResult<InnerProduct<AccessImplementation<ImplementationT_>,
                                    AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, ColsV_, RowsV_>>>,
                       ImplementationOf<ValueMatrixT_, ValueT_, RowsV_, RowsV_>>;
      { Norm<AccessImplementation<ImplementationT_>>::value(impl) } -> std::same_as<ValueT_>;
      { SquaredNorm<AccessImplementation<ImplementationT_>>::value(impl) } -> std::same_as<ValueT_>;
      stream << impl;
    } &&
    BackendRandom<ImplementationT_, ValueT_>;

//! The matrices of a value matrix backend with a value type and a size.
template <template <typename, int, int> typename ValueMatrixT_, typename ValueT_, int RowsV_, int ColsV_>
concept BackendMatrixOfSize =
    AnyValuesWrapper<ValueMatrixT_<ValueT_, RowsV_, ColsV_>> &&
    BackendMatrix<ImplementationOf<ValueMatrixT_, ValueT_, RowsV_, ColsV_>, ValueMatrixT_, ValueT_, RowsV_, ColsV_>;

//! The products of three-dimensional vectors of a value matrix backend: the cross product, and the outer product of a vector
//! and a covector.
template <template <typename, int, int> typename ValueMatrixT_, typename ValueT_>
concept BackendVectorProducts =
    requires(const ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>& vector, const ImplementationOf<ValueMatrixT_, ValueT_, 1, 3>& covector) {
      {
        CrossProduct<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>,
                     AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>>::value(vector, vector)
      } -> TraitResult<CrossProduct<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>,
                                    AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>>,
                       ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>;
      {
        OuterProduct<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>,
                     AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 1, 3>>>::value(vector, covector)
      } -> TraitResult<OuterProduct<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 3, 1>>,
                                    AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, 1, 3>>>,
                       ImplementationOf<ValueMatrixT_, ValueT_, 3, 3>>;
    };

//! The values of the type ValueT_ of a value matrix backend: scalars, vectors, covectors and square matrices.
template <template <typename, int, int> typename ValueMatrixT_, typename ValueT_>
concept BackendValues = BackendMatrixOfSize<ValueMatrixT_, ValueT_, 1, 1> && BackendMatrixOfSize<ValueMatrixT_, ValueT_, 3, 1> &&
                        BackendMatrixOfSize<ValueMatrixT_, ValueT_, 1, 3> && BackendMatrixOfSize<ValueMatrixT_, ValueT_, 3, 3> &&
                        BackendVectorProducts<ValueMatrixT_, ValueT_>;

//! The matrix inverse (Inverse) of square matrices of a value matrix backend.
template <template <typename, int, int> typename ValueMatrixT_, typename ValueT_, int DimensionV_>
concept BackendInverse = requires(const ImplementationOf<ValueMatrixT_, ValueT_, DimensionV_, DimensionV_>& impl) {
  {
    Inverse<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, DimensionV_, DimensionV_>>>::value(impl)
  } -> TraitResult<Inverse<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, DimensionV_, DimensionV_>>>,
                   ImplementationOf<ValueMatrixT_, ValueT_, DimensionV_, DimensionV_>>;
};

//! The determinant (Determinant) of square matrices of a value matrix backend, of the value type.
template <template <typename, int, int> typename ValueMatrixT_, typename ValueT_, int DimensionV_>
concept BackendDeterminant = requires(const ImplementationOf<ValueMatrixT_, ValueT_, DimensionV_, DimensionV_>& impl) {
  {
    Determinant<AccessImplementation<ImplementationOf<ValueMatrixT_, ValueT_, DimensionV_, DimensionV_>>>::value(impl)
  } -> std::same_as<ValueT_>;
};

}  // namespace internal

/**
 * A value matrix backend, given as its alias template ValueMatrix<ValueT_, RowsV_, ColsV_>, e.g. wuzp_eigen::ValueMatrix: its
 * value matrices are ValuesWrappers of the given value type and size, and it specializes the traits the core needs for their
 * implementation types. They are checked for scalars, vectors, covectors and square matrices of double and int: allocation
 * (default construction and ReserveMemory), initialization with the elements as values of the value type (Initialize),
 * evaluation (Evaluate of the results of the traits, which may be lazy: they are move constructible, and the matrices are
 * constructible from them), Copy and Move (the matrices need a move constructor, but need not be copyable or assignable), element access
 * (Coefficient), Rebind, Zero and Random, elementwise Sum, Difference, Negation, Product and Division (also with a scalar of the
 * value type), the elementwise reciprocal CwiseInverse, Transpose, Block, InnerProduct, OuterProduct and CrossProduct, Norm and
 * SquaredNorm, Comparison and printing. The matrix inverse and the determinant are checked by InvertibleValueMatrixBackend.
 *
 * A backend checks it in its tests, e.g. `static_assert(wuzp::ValueMatrixBackend<wuzp_eigen::ValueMatrix>);`: the check
 * instantiates many expressions of the backend, which takes about 0.7 s with Eigen. The core constrains its parameters with
 * the cheap AnyValueMatrix instead.
 */
template <template <typename, int, int> typename ValueMatrixT_>
concept ValueMatrixBackend =
    AnyValueMatrix<ValueMatrixT_> && internal::BackendValues<ValueMatrixT_, double> && internal::BackendValues<ValueMatrixT_, int>;

//! A value matrix backend that also provides the matrix inverse (Inverse) of floating point matrices, and the determinant
//! (Determinant) of floating point and integer matrices: the core rejects the inverse of an integer matrix, which is not an
//! integer matrix in general, but not its determinant (see "Inverses and determinants" in DESIGN.md). The transformations of
//! covectors and of tensors with covariant indices need them. A backend checks it in its tests, like ValueMatrixBackend.
template <template <typename, int, int> typename ValueMatrixT_>
concept InvertibleValueMatrixBackend =
    ValueMatrixBackend<ValueMatrixT_> && internal::BackendInverse<ValueMatrixT_, double, 1> &&
    internal::BackendInverse<ValueMatrixT_, double, 3> && internal::BackendDeterminant<ValueMatrixT_, double, 1> &&
    internal::BackendDeterminant<ValueMatrixT_, double, 3> && internal::BackendDeterminant<ValueMatrixT_, int, 1> &&
    internal::BackendDeterminant<ValueMatrixT_, int, 3>;

namespace internal {

//! The unit of the product of units.
template <typename LhsT_, typename RhsT_>
using UnitProduct = typename Product<LhsT_, RhsT_>::type;

//! The unit of the quotient of units.
template <typename LhsT_, typename RhsT_>
using UnitQuotient = typename Division<LhsT_, RhsT_>::type;

//! Whether the units of a units backend are units, and consistent: e.g. a velocity is a length divided by a time.
template <typename UnitsT_>
concept ConsistentUnits =
    AnyUnit<typename UnitsT_::Dimensionless> && AnyUnit<typename UnitsT_::Angle> && AnyUnit<typename UnitsT_::Length> &&
    AnyUnit<typename UnitsT_::Mass> && AnyUnit<typename UnitsT_::Time> && AnyUnit<typename UnitsT_::Area> &&
    AnyUnit<typename UnitsT_::Volume> && AnyUnit<typename UnitsT_::Velocity> && AnyUnit<typename UnitsT_::Acceleration> &&
    AnyUnit<typename UnitsT_::Frequency> && AnyUnit<typename UnitsT_::AngularVelocity> && AnyUnit<typename UnitsT_::AngularAcceleration> &&
    AnyUnit<typename UnitsT_::Force> && AnyUnit<typename UnitsT_::Energy> && AnyUnit<typename UnitsT_::Torque> &&
    // Angles are dimensionless (see "Angles are dimensionless" in DESIGN.md).
    std::same_as<typename UnitsT_::Angle, typename UnitsT_::Dimensionless> &&
    std::same_as<typename Identity<typename UnitsT_::Length>::type, typename UnitsT_::Dimensionless> &&
    std::same_as<typename Power<typename UnitsT_::Length, Integer<2>>::type, typename UnitsT_::Area> &&
    // Exponents are rational, e.g. for pow<1, 2>().
    std::same_as<typename Power<typename UnitsT_::Area, Rational<1, 2>>::type, typename UnitsT_::Length> &&
    std::same_as<UnitProduct<typename UnitsT_::Area, typename UnitsT_::Length>, typename UnitsT_::Volume> &&
    std::same_as<UnitQuotient<typename UnitsT_::Length, typename UnitsT_::Time>, typename UnitsT_::Velocity> &&
    std::same_as<UnitQuotient<typename UnitsT_::Velocity, typename UnitsT_::Time>, typename UnitsT_::Acceleration> &&
    std::same_as<typename Inverse<typename UnitsT_::Time>::type, typename UnitsT_::Frequency> &&
    std::same_as<UnitQuotient<typename UnitsT_::Angle, typename UnitsT_::Time>, typename UnitsT_::AngularVelocity> &&
    std::same_as<UnitQuotient<typename UnitsT_::AngularVelocity, typename UnitsT_::Time>, typename UnitsT_::AngularAcceleration> &&
    std::same_as<UnitProduct<typename UnitsT_::Mass, typename UnitsT_::Acceleration>, typename UnitsT_::Force> &&
    std::same_as<UnitProduct<typename UnitsT_::Force, typename UnitsT_::Length>, typename UnitsT_::Energy> &&
    // Torque is N m, the unit of energy, so that the cross product of a length and a force is a torque.
    std::same_as<UnitProduct<typename UnitsT_::Length, typename UnitsT_::Force>, typename UnitsT_::Torque> &&
    std::same_as<typename UnitsT_::Torque, typename UnitsT_::Energy>;

}  // namespace internal

/**
 * The units of a units backend, e.g. AccessUnits<AccessImplementation<double>>: the base and derived units that the core and
 * its aliases use, e.g. `Length` and `Force`, which are consistent with the arithmetic traits of the backend, Product,
 * Division, Power (with rational exponents), Inverse and Identity, e.g. `Velocity` is `Length` divided by `Time`. The core
 * defines the sum and the difference of units, and compares units by their types, so equal units must have the same type,
 * however they are computed, e.g. `Torque` and `Energy`. The backend specializes AccessUnits<AccessImplementation<ValueT_>>
 * for every arithmetic value type. A backend checks it in its tests, e.g.
 * `static_assert(wuzp::UnitSystem<wuzp::AccessUnits<wuzp::AccessImplementation<double>>>);`.
 */
template <typename UnitsT_>
concept UnitSystem = requires {
  typename UnitsT_::Dimensionless;
  typename UnitsT_::Angle;
  typename UnitsT_::Length;
  typename UnitsT_::Mass;
  typename UnitsT_::Time;
  typename UnitsT_::Area;
  typename UnitsT_::Volume;
  typename UnitsT_::Velocity;
  typename UnitsT_::Acceleration;
  typename UnitsT_::Frequency;
  typename UnitsT_::AngularVelocity;
  typename UnitsT_::AngularAcceleration;
  typename UnitsT_::Force;
  typename UnitsT_::Energy;
  typename UnitsT_::Torque;
} && internal::ConsistentUnits<UnitsT_>;

}  // namespace wuzp
