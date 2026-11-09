#pragma once

#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

namespace internal {

//! The quantities as they are, or transposed if TransposeV_: e.g. the components of a vector, a column, as the row that a matrix
//! product needs. These are specializations rather than `if constexpr`: MSVC compiles both branches of an `if constexpr` in
//! member functions of class templates (see mapRelative).
template <bool TransposeV_>
struct TransposeIf {
  template <typename QuantitiesT_>
  using type = QuantitiesT_;
  //! The quantities themselves, which the caller uses within the same expression.
  template <typename QuantitiesT_>
  static QuantitiesT_&& value(QuantitiesT_&& quantities) {
    return std::forward<QuantitiesT_>(quantities);
  }
};
template <>
struct TransposeIf<true> {
  template <typename QuantitiesT_>
  using type = typename Evaluate<typename Transpose<QuantitiesT_>::type>::type;
  template <typename QuantitiesT_>
  static auto value(const QuantitiesT_& quantities) {
    return quantities.transpose();
  }
};

//! Whether a tensor with these indices and quantities is a batch of vectors or points, one per column, e.g. a point cloud (see
//! Indices). The operations that are only defined for a single vector, e.g. the inner product, reject batches.
template <typename IndicesT_, typename QuantitiesT_>
inline constexpr bool isBatch = std::is_same_v<IndicesT_, Indices<Up>> && Cols<QuantitiesT_>::value > 1;

//! Rejects a batch of vectors or points in an operation that is only defined for a single vector, with the same message for
//! all of them: `static_assert(RejectBatch<IndicesT_, QuantitiesT_>::value)`. The value is always true, so that a batch fails
//! with this message only.
template <typename IndicesT_, typename QuantitiesT_>
struct RejectBatch {
  static_assert(!isBatch<IndicesT_, QuantitiesT_>,
                "A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and "
                "quotients with scalars.");
  static constexpr bool value = true;
};

}  // namespace internal

/**
 * A tensor of rank 0, 1 or 2, whose components are quantities, expressed in a coordinate system, e.g. a vector, a covector, a
 * linear map or a mass matrix.
 * @tparam IndicesT_ The indices, e.g. Indices<Up> for a vector or Indices<Down, Down> for a mass matrix (see Indices).
 * @tparam QuantitiesT_ The components, a matrix with the first index as the row: a 1x1 matrix for rank 0, a column for a vector,
 * a row for a covector, a square matrix for rank 2. A tensor with an upper index may also store a batch of vectors or points,
 * one per column, e.g. a point cloud.
 * @tparam CoordinateSystemT_ The coordinate system that the components are expressed in.
 * @tparam AbsoluteT_ Absolute for points, Relative for vectors and all other tensors (see Absolute.hpp). Only a tensor with one
 * upper index, a vector or a batch of vectors, can be absolute: the components of a point are those of the vector from the
 * origin of the coordinate system to it. An absolute scalar, e.g. a time point or a temperature on the Celsius scale, is a point
 * in a one-dimensional coordinate system whose origin is its zero, e.g. an epoch or the ice point, like the Celsius temperature
 * t = T - T0 of the SI Brochure, section 2.3.1 (https://www.bipm.org/en/publications/si-brochure) (see TimePoint).
 */
template <AnyIndices IndicesT_, AnyQuantities QuantitiesT_, AnyCoordinateSystem CoordinateSystemT_, AbsoluteOrRelative AbsoluteT_>
class Tensor {
 public:
  using IndicesT = IndicesT_;
  using QuantitiesT = QuantitiesT_;
  using CoordinateSystemT = CoordinateSystemT_;
  using UnitsT = typename QuantitiesT::UnitsT;
  using ValuesT = typename QuantitiesT::ValuesT;
  using ValueT = typename QuantitiesT::ValueT;
  using AbsoluteT = AbsoluteT_;
  using ThisT = Tensor<IndicesT, QuantitiesT, CoordinateSystemT, AbsoluteT>;

  static_assert(IndicesT::RankV <= 2, "Tensors of rank 3 or more are not supported at the moment.");
  // The components are stored in a canonical shape, so that the shape of a tensor follows from its indices (see Indices).
  static_assert(IndicesT::RankV != 0 || (Rows<QuantitiesT>::value == 1 && Cols<QuantitiesT>::value == 1),
                "A tensor of rank 0 is stored as a 1x1 matrix.");
  static_assert(!std::is_same_v<IndicesT, Indices<Down>> || Rows<QuantitiesT>::value == 1,
                "A covector, a tensor with one lower index, is stored as a row.");
  static_assert(IndicesT::RankV != 2 || Rows<QuantitiesT>::value == Cols<QuantitiesT>::value,
                "A tensor of rank 2 is stored as a square matrix.");
  // Points have no counterpart in other ranks: e.g. a covector pairs with vectors, not with points, and an absolute scalar is a
  // one-dimensional point (see "Points and vectors" in DESIGN.md).
  static_assert(!AbsoluteT::value || std::is_same_v<IndicesT, Indices<Up>>,
                "Only a tensor with one upper index, a vector or a batch of vectors, can be absolute (a point).");

  Tensor() = default;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename T0, typename... Ts>
    requires(!IsSpecialization<T0, Tag>::value && !CopiesOrMoves<Tensor, T0, Ts...>)
  // Allow implicit construction from similar tensors if the conversion is valid. NOLINTNEXTLINE(google-explicit-constructor)
  explicit(!IsValidConversion<ThisT, std::remove_cvref_t<T0>, std::remove_cvref_t<Ts>...>::value) Tensor(T0&& t0, Ts&&... ts)
      : Tensor(typename GetTag<ThisT, T0, Ts...>::type{}, std::forward<T0>(t0), std::forward<Ts>(ts)...) {}

  static ThisT zero() { return ThisT(QuantitiesT::zero()); }
  static ThisT random() { return ThisT(QuantitiesT::random()); }

  template <typename ValuesOtherT_, typename UnitsOtherT_>
  void setQuantities(const Quantities<ValuesOtherT_, UnitsOtherT_>& component) {
    // Direct initialization checks the units, see the converting constructor of Quantities.
    quantities_ = QuantitiesT(component);
  }
  const QuantitiesT& getQuantities() const& { return quantities_; }
  //! The quantities of a temporary are moved out of it, which avoids a copy and does not refer to the destroyed temporary.
  QuantitiesT getQuantities() && { return std::move(quantities_); }

  template <typename ValuesOtherT_>
  void setValues(const ValuesOtherT_& values) {
    quantities_.setValues(values);
  }
  const ValuesT& getValues() const& { return quantities_.getValues(); }
  ValuesT getValues() && { return std::move(quantities_).getValues(); }

  template <typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
  bool isApprox(const Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    // Note: Allow other tensor types to support lazy evaluation.
    static_assert(std::is_same_v<IndicesT, IndicesOtherT_>, "Indices do not match.");
    static_assert(std::is_same_v<CoordinateSystemT, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    static_assert(std::is_same_v<AbsoluteT, AbsoluteOtherT_>, "Absolute/relative mismatch.");
    return quantities_.isApprox(other.getQuantities(), precision);
  }

  auto getQuantity() const
    requires(Dimension<ThisT>::value == 0)
  {
    return quantities_.get();
  }

  template <int RowIdV_, int ColIdV_>
  auto getQuantity() const {
    return quantities_.template get<RowIdV_, ColIdV_>();
  }

  auto getQuantity(const int rowId, const int colId) const { return quantities_.get(rowId, colId); }

  template <typename QuantityT_>
  void setQuantity(const QuantityT_& quantity)
    requires(Dimension<ThisT>::value == 0)
  {
    return quantities_.set(quantity);
  }

  template <int RowIdV_, int ColIdV_, typename QuantityT_>
  void setQuantity(const QuantityT_& quantity) {
    return quantities_.template set<RowIdV_, ColIdV_>(quantity);
  }

  template <typename QuantityT_>
  void setQuantity(const int rowId, const int colId, const QuantityT_& quantity) {
    quantities_.set(rowId, colId, quantity);
  }

  // The member functions and operators return evaluated results (see ValuesWrapper), of the evaluated types of the traits.

  // TODO(remod): Rename to "slice"?
  template <int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
  auto block() const {
    return typename Evaluate<typename Block<ThisT, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type>::type(
        quantities_.template block<BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>());
  }

  //! The transpose of the matrix of the components: a vector becomes a covector and vice versa, and the indices of a tensor of
  //! rank 2 are swapped, e.g. a linear map T^i_j becomes (T^T)_j^i (see Transpose<Indices<...>>).
  auto transpose() const { return typename Evaluate<typename Transpose<ThisT>::type>::type{quantities_.transpose()}; }

  //! The inner product, see innerProduct().
  template <typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
  auto innerProduct(const Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& other) const {
    using TraitT = InnerProduct<ThisT, Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>>;
    return typename Evaluate<typename TraitT::type>::type{internal::TransposeIf<TraitT::TransposeResultV>::value(
        wuzp::innerProduct(internal::TransposeIf<TraitT::TransposeLhsV>::value(quantities_),
                           internal::TransposeIf<TraitT::TransposeRhsV>::value(other.getQuantities())))};
  }

  template <typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
  auto dotProduct(const Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& other) const {
    return typename Evaluate<
        typename DotProduct<ThisT, Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>>::type>::type{
        quantities_.dotProduct(other.getQuantities())};
  }

  template <typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
  auto crossProduct(const Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& other) const {
    return typename Evaluate<
        typename CrossProduct<ThisT, Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>>::type>::type{
        quantities_.crossProduct(other.getQuantities())};
  }

  /**
   * The matrix inverse of a tensor of rank 0 or 2, like Eigen's MatrixBase::inverse(): e.g. the inverse of a linear map T^i_j is
   * a linear map, and the inverse of a metric g_ij, or of a mass matrix M_ij, has two upper indices, g^ij (see
   * Inverse<Indices<...>>). The units of the components must describe a linear map, M_ij = a_i / b_j, and those of the inverse
   * are (M^-1)_ij = b_i / a_j (see Inverse<TypeMatrixImpl<...>> in Units.hpp). Vectors, covectors and points have no inverse.
   */
  auto inverse() const { return typename Evaluate<typename Inverse<ThisT>::type>::type{quantities_.inverse()}; }

  //! The elementwise reciprocal of the components, like Eigen's ArrayBase::inverse(). It depends on the basis, so it is not a
  //! tensor: it returns quantities, without indices or a coordinate system, like cwiseProduct(). Points are rejected, since their
  //! components depend on the origin.
  auto cwiseInverse() const {
    static_assert(std::is_same_v<typename Inverse<AbsoluteT>::type, Relative>);
    return quantities_.cwiseInverse();
  }

  // The norms are defined for tensors of dimension 0 and 1. Batches of vectors have them too, so that they are rejected with a
  // clear message. The norms of a point depend on the origin, so Norm<Absolute> rejects them.

  auto norm() const
    requires(Dimension<ThisT>::value <= 1 || internal::isBatch<IndicesT, QuantitiesT>)
  {
    static_assert(internal::RejectBatch<IndicesT, QuantitiesT>::value);
    static_assert(std::is_same_v<typename Norm<AbsoluteT>::type, Relative>);
    return quantities_.norm();
  }

  auto squaredNorm() const
    requires(Dimension<ThisT>::value <= 1 || internal::isBatch<IndicesT, QuantitiesT>)
  {
    static_assert(internal::RejectBatch<IndicesT, QuantitiesT>::value);
    static_assert(std::is_same_v<typename Norm<AbsoluteT>::type, Relative>);
    return quantities_.squaredNorm();
  }

  //! The determinant of a tensor of rank 0 or of a mixed tensor of rank 2, e.g. a linear map T^i_j, which is invariant. The
  //! determinant of a tensor with two upper or two lower indices depends on the basis, so it is rejected (see
  //! Determinant<Indices<...>>); getQuantities().determinant() computes the determinant of its components. Like inverse(), it
  //! exists for every tensor, so that vectors, covectors and batches are rejected with a clear message.
  auto determinant() const { return typename Determinant<ThisT>::type{quantities_.determinant()}; }

  void hyperdeterminant() const { static_assert(FailOnInstantiation<ThisT>::value, "Not implemented"); }

  auto evaluate() const& { return typename Evaluate<ThisT>::type{quantities_.evaluate()}; }
  auto evaluate() && { return typename Evaluate<ThisT>::type{std::move(quantities_).evaluate()}; }

 private:
  template <typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
  // Allow other tensor types to support lazy evaluation.
  Tensor(FromSimilar /*tag*/, const Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& other)
      : quantities_(other.getQuantities()) {
    // The converting constructor of the quantities checks the units.
    using ConversionT = IsValidConversion<ThisT, Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>>;
    static_assert(ConversionT::IndicesMatchV, "Indices do not match.");
    static_assert(ConversionT::CoordinateSystemsMatchV, "Coordinate systems do not match.");
    // Points and vectors must not be converted into each other implicitly; use makeRelative() and makeAbsolute() instead.
    static_assert(ConversionT::AbsoluteMatchV, "Absolute/relative mismatch.");
  }

  template <typename... Ts>
  explicit Tensor(FromImplementationArgs /*tag*/, Ts&&... ts) : quantities_{std::forward<Ts>(ts)...} {}

  QuantitiesT quantities_;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_, typename IndicesOtherT_,
          typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct IsValidConversion<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>,
                         Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  // The converting constructor checks these with static_asserts, which explain why a conversion fails, and the converting
  // constructor of the quantities checks their units.
  static constexpr bool IndicesMatchV = std::is_same_v<IndicesT_, IndicesOtherT_>;
  static constexpr bool CoordinateSystemsMatchV = std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>;
  static constexpr bool AbsoluteMatchV = std::is_same_v<AbsoluteT_, AbsoluteOtherT_>;
  static constexpr bool value =
      IndicesMatchV && CoordinateSystemsMatchV && AbsoluteMatchV && IsValidConversion<QuantitiesT_, QuantitiesOtherT_>::value;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_, typename... Ts>
struct GetTag<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename IfThenElse<sizeof...(Ts) == 1 && AnyTensor<first>, FromSimilar, FromImplementationArgs>::type;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Dimension<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr int value = Dimension<QuantitiesT_>::value;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Rows<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr int value = Rows<QuantitiesT_>::value;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Cols<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr int value = Cols<QuantitiesT_>::value;
};

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Sum<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
           Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  using type = Tensor<typename Sum<IndicesLhsT_, IndicesRhsT_>::type, typename Sum<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Sum<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Sum<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Difference<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                  Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  using type = Tensor<typename Difference<IndicesLhsT_, IndicesRhsT_>::type, typename Difference<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Difference<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type,
                      typename Difference<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Negation<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = Tensor<IndicesT_, typename Negation<QuantitiesT_>::type, CoordinateSystemT_, typename Negation<AbsoluteT_>::type>;
};

//! The product of two tensors, one of which has rank 0, e.g. of a scalar field and a vector. It has the indices of the other
//! tensor (see Product<Indices<...>, Indices<...>>).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Product<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
               Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  using type =
      Tensor<typename Product<IndicesLhsT_, IndicesRhsT_>::type, typename Product<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

//! The product of a tensor and a quantity, a 1x1 matrix of quantities. The quantities of other sizes have no indices, so their
//! elementwise product with a tensor is not a tensor (see Product<Indices<...>, Indices<...>>). A quantity is relative, so the
//! product of a point is rejected (see Product<Absolute, Relative>): it depends on the origin.
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename ValuesRhsT_,
          typename UnitsRhsT_>
struct Product<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  static_assert(Rows<UnitsRhsT_>::value == 1 && Cols<UnitsRhsT_>::value == 1,
                "A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0. Use innerProduct, outerProduct or "
                "cwiseProduct for other products.");
  using type = Tensor<IndicesLhsT_, typename Product<QuantitiesLhsT_, Quantities<ValuesRhsT_, UnitsRhsT_>>::type, CoordinateSystemLhsT_,
                      typename Product<AbsoluteLhsT_, Relative>::type>;
};
template <typename ValuesLhsT_, typename UnitsLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_,
          typename AbsoluteRhsT_>
struct Product<Quantities<ValuesLhsT_, UnitsLhsT_>, Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  using type = typename Product<Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>,
                                Quantities<ValuesLhsT_, UnitsLhsT_>>::type;
};

/**
 * The inner product, which contracts the last index of the left-hand side with the first index of the right-hand side (see
 * InnerProduct<Indices<...>, Indices<...>>). It is the product of the matrices of the components, with the last index of the
 * left-hand side along its columns and the first index of the right-hand side along its rows: a vector, a column, on the
 * left-hand side is transposed into a row, and a covector, a row, on the right-hand side into a column. All summands of an
 * element of the product must have the same unit, which holds for the matrices of linear maps (Hart, Multidimensional Analysis,
 * 1995). A result of rank 1 is transposed into the canonical shape, e.g. the product M v of a mass matrix M_ij and a vector v^j
 * is a column, but the result, the covector M_ij v^j, is a row.
 */
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct InnerProduct<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                    Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(internal::RejectBatch<IndicesLhsT_, QuantitiesLhsT_>::value && internal::RejectBatch<IndicesRhsT_, QuantitiesRhsT_>::value);
  using IndicesT = typename InnerProduct<IndicesLhsT_, IndicesRhsT_>::type;
  static constexpr bool TransposeLhsV = std::is_same_v<IndicesLhsT_, Indices<Up>>;
  static constexpr bool TransposeRhsV = std::is_same_v<IndicesRhsT_, Indices<Down>>;
  //! The product of a row and a tensor of rank 2 is a row, the product of a tensor of rank 2 and a column is a column.
  static constexpr bool TransposeResultV = IndicesT::RankV == 1 && ((IndicesLhsT_::RankV == 1) == std::is_same_v<IndicesT, Indices<Up>>);
  using type =
      Tensor<IndicesT,
             typename internal::TransposeIf<TransposeResultV>::template type<typename Evaluate<
                 typename InnerProduct<typename internal::TransposeIf<TransposeLhsV>::template type<QuantitiesLhsT_>,
                                       typename internal::TransposeIf<TransposeRhsV>::template type<QuantitiesRhsT_>>::type>::type>,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

//! The outer product, whose indices are those of the left-hand side, followed by those of the right-hand side (see
//! OuterProduct<Indices<...>, Indices<...>>): the left-hand side as a column times the right-hand side as a row, e.g. v^i w_j.
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct OuterProduct<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                    Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(internal::RejectBatch<IndicesLhsT_, QuantitiesLhsT_>::value && internal::RejectBatch<IndicesRhsT_, QuantitiesRhsT_>::value);
  static constexpr bool TransposeLhsV = !std::is_same_v<IndicesLhsT_, Indices<Up>>;
  static constexpr bool TransposeRhsV = !std::is_same_v<IndicesRhsT_, Indices<Down>>;
  using type =
      Tensor<typename OuterProduct<IndicesLhsT_, IndicesRhsT_>::type,
             typename OuterProduct<typename internal::TransposeIf<TransposeLhsV>::template type<QuantitiesLhsT_>,
                                   typename internal::TransposeIf<TransposeRhsV>::template type<QuantitiesRhsT_>>::type,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct DotProduct<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                  Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(std::is_same_v<IndicesLhsT_, IndicesRhsT_>, "Indices do not match.");
  static_assert(internal::RejectBatch<IndicesLhsT_, QuantitiesLhsT_>::value && internal::RejectBatch<IndicesRhsT_, QuantitiesRhsT_>::value);
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  //! The dot product of a point depends on the origin, so Product<Absolute, ...> rejects it.
  using AbsoluteT = typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type;
  using type = typename DotProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type;
};

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct CrossProduct<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                    Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(internal::RejectBatch<IndicesLhsT_, QuantitiesLhsT_>::value && internal::RejectBatch<IndicesRhsT_, QuantitiesRhsT_>::value);
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  //! The cross product of a point depends on the origin, so Product<Absolute, ...> rejects it.
  using AbsoluteT = typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type;
  using type = Tensor<typename CrossProduct<IndicesLhsT_, IndicesRhsT_>::type,
                      typename CrossProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type, CoordinateSystemLhsT_, AbsoluteT>;
};

//! The quotient of a tensor and a tensor of rank 0 (see Division<Indices<...>, Indices<...>>).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Division<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  using type =
      Tensor<typename Division<IndicesLhsT_, IndicesRhsT_>::type, typename Division<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Division<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Division<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};
//! The quotient of a tensor and a quantity, a 1x1 matrix of quantities (see Product<Tensor<...>, Quantities<...>>). The quotient
//! of a point is rejected (see Division<Absolute, Relative>).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename ValuesRhsT_,
          typename UnitsRhsT_>
struct Division<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>, Quantities<ValuesRhsT_, UnitsRhsT_>> {
  static_assert(Rows<UnitsRhsT_>::value == 1 && Cols<UnitsRhsT_>::value == 1,
                "A tensor can only be divided by a scalar, a quantity or a tensor of rank 0. Use cwiseQuotient for elementwise "
                "quotients.");
  using type = Tensor<IndicesLhsT_, typename Division<QuantitiesLhsT_, Quantities<ValuesRhsT_, UnitsRhsT_>>::type, CoordinateSystemLhsT_,
                      typename Division<AbsoluteLhsT_, Relative>::type>;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Transpose<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static_assert(internal::RejectBatch<IndicesT_, QuantitiesT_>::value);
  using type = Tensor<typename Transpose<IndicesT_>::type, typename Transpose<QuantitiesT_>::type, CoordinateSystemT_, AbsoluteT_>;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_, int BlockRowIdV_, int BlockColIdV_,
          int BlockRowsV_, int BlockColsV_>
struct Block<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  using type = Tensor<IndicesT_, typename Block<QuantitiesT_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type,
                      CoordinateSystemT_, AbsoluteT_>;
};

//! The matrix inverse of a tensor of rank 0 or 2 (see Tensor::inverse()).
template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Inverse<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static_assert(internal::RejectBatch<IndicesT_, QuantitiesT_>::value);
  // The absoluteness and the indices first, which reject points, vectors and covectors with a clearer message than the size of
  // their components.
  using AbsoluteT = typename Inverse<AbsoluteT_>::type;
  using IndicesT = typename Inverse<IndicesT_>::type;
  using type = Tensor<IndicesT, typename Inverse<QuantitiesT_>::type, CoordinateSystemT_, AbsoluteT>;
};

//! The determinant of a tensor of rank 0 or of a mixed tensor of rank 2, a quantity (see Tensor::determinant()).
template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Determinant<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static_assert(internal::RejectBatch<IndicesT_, QuantitiesT_>::value);
  using IndicesT = typename Determinant<IndicesT_>::type;
  using type = typename Determinant<QuantitiesT_>::type;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct GetCoordinateSystem<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = CoordinateSystemT_;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct IsAbsolute<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr bool value = AbsoluteT_::value;
};
//! The vector from the origin of the coordinate system to a point, p - o, which has the components of the point. A vector is
//! rejected (see MakeRelative<Relative>).
template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct MakeRelative<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto make(Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_> object) {
    return Tensor<IndicesT_, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_, typename MakeRelative<AbsoluteT_>::type>(
        std::move(object).getQuantities());
  }
};
//! The point at a vector from the origin of the coordinate system, o + v, which has the components of the vector. A point is
//! rejected (see MakeAbsolute<Absolute>).
template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct MakeAbsolute<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto make(Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_> object) {
    return Tensor<IndicesT_, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_, typename MakeAbsolute<AbsoluteT_>::type>(
        std::move(object).getQuantities());
  }
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_, typename NewCoordinateSystemT_>
struct CastCoordinateSystem<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>, NewCoordinateSystemT_> {
  using type = Tensor<IndicesT_, QuantitiesT_, NewCoordinateSystemT_, AbsoluteT_>;
};

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Evaluate<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = Tensor<IndicesT_, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_, AbsoluteT_>;
};

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto sum(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
         const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Sum<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                                        Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      sum(lhs.getQuantities(), rhs.getQuantities())};
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator+(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return sum(lhs, rhs);
}

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto difference(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Difference<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                                               Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      difference(lhs.getQuantities(), rhs.getQuantities())};
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator-(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return difference(lhs, rhs);
}

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
auto negation(const Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& rhs) {
  return typename Evaluate<typename Negation<Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>>::type>::type(
      negation(rhs.getQuantities()));
}
template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
auto operator-(const Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& rhs) {
  return negation(rhs);
}

// The products with scalars: arithmetic values, values and quantities of size 1x1, and tensors of rank 0. The result has the
// indices and the coordinate system of the tensor (see Product<Indices<...>, Indices<...>>). The product of a point depends on
// the origin, so it is rejected (see Product<Absolute, Relative>).

//! The product with an arithmetic scalar, which is converted into the value type of the tensor if their common type is the value
//! type, e.g. 2 * vector for a vector of doubles (see internal::toDimensionlessQuantity).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_,
          ArithmeticValue ValueRhsT_>
auto product(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs, const ValueRhsT_ rhs) {
  // The scalar has the value matrix backend of the tensor.
  return product(lhs, internal::toDimensionlessQuantity<typename QuantitiesLhsT_::ValuesT>(rhs));
}
template <ArithmeticValue ValueLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_,
          typename AbsoluteRhsT_>
auto product(const ValueLhsT_ lhs, const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(rhs, lhs);
}
//! The product with dimensionless values, which must be a scalar, 1x1 values.
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename MatrixSizeRhsT_,
          typename ValueRhsT_, typename ImplementationRhsT_>
auto product(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, Quantities<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>,
                                 TypeMatrix<Rows<MatrixSizeRhsT_>::value, Cols<MatrixSizeRhsT_>::value,
                                            typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>>(rhs));
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
             const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(rhs, lhs);
}
//! The product with a quantity, 1x1 quantities (see Product<Tensor<...>, Quantities<...>>).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename ValuesRhsT_,
          typename UnitsRhsT_>
auto product(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<typename Product<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                                            Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type{product(lhs.getQuantities(), rhs)};
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_,
          typename AbsoluteRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs,
             const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(rhs, lhs);
}
//! The product of two tensors, one of which has rank 0 (see Product<Tensor<...>, Tensor<...>>).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto product(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Product<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                                            Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      product(lhs.getQuantities(), rhs.getQuantities())};
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_,
          ArithmeticValue ValueRhsT_>
auto operator*(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs, const ValueRhsT_ rhs) {
  return product(lhs, rhs);
}
template <ArithmeticValue ValueLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_,
          typename AbsoluteRhsT_>
auto operator*(const ValueLhsT_ lhs, const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename MatrixSizeRhsT_,
          typename ValueRhsT_, typename ImplementationRhsT_>
auto operator*(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename ValuesRhsT_,
          typename UnitsRhsT_>
auto operator*(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, typename IndicesRhsT_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_,
          typename AbsoluteRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs,
               const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator*(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}

/**
 * The inner product, which contracts the last index of the left-hand side with the first index of the right-hand side, e.g. w_i
 * v^i of a covector and a vector, T^i_j v^j of a linear map and a vector, or M_ij v^j of a mass matrix and a vector, a covector.
 * The indices must be an upper and a lower one (see InnerProduct<Indices<...>, Indices<...>>).
 */
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto innerProduct(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                  const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return lhs.innerProduct(rhs);
}

/**
 * The outer (tensor) product of two vectors or covectors, e.g. v^i w_j, a tensor of rank 2 whose indices are those of the
 * left-hand side, followed by those of the right-hand side (see OuterProduct<Indices<...>, Indices<...>>).
 */
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto outerProduct(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                  const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  using TraitT = OuterProduct<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                              Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>;
  return typename Evaluate<typename TraitT::type>::type{
      outerProduct(internal::TransposeIf<TraitT::TransposeLhsV>::value(lhs.getQuantities()),
                   internal::TransposeIf<TraitT::TransposeRhsV>::value(rhs.getQuantities()))};
}

/**
 * The elementwise (Hadamard) product of the components of two tensors in the same coordinate system. It depends on the basis, so
 * it is not a tensor: it returns quantities, without indices or a coordinate system, like the products of Eigen's arrays
 * (https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html). Points are rejected, since their components depend on the
 * origin. The product of the quantities, `lhs.getQuantities() * rhs.getQuantities()`, has none of these checks.
 */
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto cwiseProduct(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                  const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  static_assert(std::is_same_v<typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type, Relative>);
  return product(lhs.getQuantities(), rhs.getQuantities());
}

//! The elementwise quotient of the components of two tensors in the same coordinate system, as quantities (see cwiseProduct()). The
//! quotient of the quantities, `division(lhs.getQuantities(), rhs.getQuantities())`, has none of these checks.
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto cwiseQuotient(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                   const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  static_assert(std::is_same_v<typename Division<AbsoluteLhsT_, AbsoluteRhsT_>::type, Relative>);
  return division(lhs.getQuantities(), rhs.getQuantities());
}

// The quotients by scalars: arithmetic values, values and quantities of size 1x1, and tensors of rank 0 (see
// Division<Indices<...>, Indices<...>>).

//! The quotient by an arithmetic scalar, which is converted into the value type of the tensor (see product()).
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_,
          ArithmeticValue ValueRhsT_>
auto division(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs, const ValueRhsT_ rhs) {
  // The scalar has the value matrix backend of the tensor.
  return division(lhs, internal::toDimensionlessQuantity<typename QuantitiesLhsT_::ValuesT>(rhs));
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename MatrixSizeRhsT_,
          typename ValueRhsT_, typename ImplementationRhsT_>
auto division(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return division(lhs, Quantities<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>,
                                  TypeMatrix<Rows<MatrixSizeRhsT_>::value, Cols<MatrixSizeRhsT_>::value,
                                             typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>>(rhs));
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename ValuesRhsT_,
          typename UnitsRhsT_>
auto division(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<typename Division<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                                             Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type{division(lhs.getQuantities(), rhs)};
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto division(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Division<Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                                             Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      division(lhs.getQuantities(), rhs.getQuantities())};
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_,
          ArithmeticValue ValueRhsT_>
auto operator/(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs, const ValueRhsT_ rhs) {
  return division(lhs, rhs);
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename MatrixSizeRhsT_,
          typename ValueRhsT_, typename ImplementationRhsT_>
auto operator/(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return division(lhs, rhs);
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename ValuesRhsT_,
          typename UnitsRhsT_>
auto operator/(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return division(lhs, rhs);
}
template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator/(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return division(lhs, rhs);
}

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
bool operator==(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  static_assert(std::is_same_v<IndicesLhsT_, IndicesRhsT_>, "Indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  static_assert(std::is_same_v<AbsoluteLhsT_, AbsoluteRhsT_>, "Absolute/relative mismatch.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <typename IndicesLhsT_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_, typename AbsoluteLhsT_, typename IndicesRhsT_,
          typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
bool operator!=(const Tensor<IndicesLhsT_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
                const Tensor<IndicesRhsT_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
std::ostream& operator<<(std::ostream& lhs, const Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

// The aliases with a value matrix backend, e.g. HomogeneousVector<3, wuzp_eigen::ValueMatrix, double, Length, CoordinateSystemT_>,
// take it as its alias template ValueMatrix (AnyValueMatrix).

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Vector = Tensor<Indices<Up>, QuantityVectorT_, CoordinateSystemT_, Relative>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
  requires AnyValueMatrix<ValuesT_>
using HomogeneousVector = Vector<Quantities<ValuesT_<ValueT_, DimensionV_, 1>, TypeMatrix<DimensionV_, 1, UnitT_>>, CoordinateSystemT_>;

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Covector = Tensor<Indices<Down>, QuantityVectorT_, CoordinateSystemT_, Relative>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
  requires AnyValueMatrix<ValuesT_>
using HomogeneousCovector = Covector<Quantities<ValuesT_<ValueT_, 1, DimensionV_>, TypeMatrix<1, DimensionV_, UnitT_>>, CoordinateSystemT_>;

/**
 * A point, or a batch of points, one per column, e.g. a point cloud. Its components are those of the vector from the origin of the
 * coordinate system to it. Points follow the rules of affine spaces (see Absolute.hpp): p - q is a vector, p + v, v + p and p - v
 * are points, and everything else that depends on the origin is rejected, e.g. p * 2.0, -p or the norm of p. Affine combinations
 * are written p + (q - p) * t, e.g. the midpoint p + (q - p) * 0.5.
 */
template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Point = Tensor<Indices<Up>, QuantityVectorT_, CoordinateSystemT_, Absolute>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
  requires AnyValueMatrix<ValuesT_>
using HomogeneousPoint = Point<Quantities<ValuesT_<ValueT_, DimensionV_, 1>, TypeMatrix<DimensionV_, 1, UnitT_>>, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using DirectionVector = HomogeneousVector<DimensionV_, ValuesT_, ValueT_,
                                          typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using DirectionVector2 = DirectionVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using DirectionVector3 = DirectionVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using PositionPoint =
    HomogeneousPoint<DimensionV_, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Length, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using PositionPoint2 = PositionPoint<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using PositionPoint3 = PositionPoint<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using PositionVector =
    HomogeneousVector<DimensionV_, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Length, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using PositionVector2 = PositionVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using PositionVector3 = PositionVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using VelocityVector =
    HomogeneousVector<DimensionV_, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Velocity, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using VelocityVector2 = VelocityVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using VelocityVector3 = VelocityVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AccelerationVector = HomogeneousVector<DimensionV_, ValuesT_, ValueT_,
                                             typename AccessUnits<AccessImplementation<ValueT_>>::Acceleration, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AccelerationVector2 = AccelerationVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AccelerationVector3 = AccelerationVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AngularVelocityVector = HomogeneousVector<DimensionV_, ValuesT_, ValueT_,
                                                typename AccessUnits<AccessImplementation<ValueT_>>::AngularVelocity, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AngularVelocityVector2 = AngularVelocityVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AngularVelocityVector3 = AngularVelocityVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AngularAccelerationVector =
    HomogeneousVector<DimensionV_, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::AngularAcceleration,
                      CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AngularAccelerationVector2 = AngularAccelerationVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using AngularAccelerationVector3 = AngularAccelerationVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ForceVector =
    HomogeneousVector<DimensionV_, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Force, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ForceVector2 = ForceVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ForceVector3 = ForceVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using TorqueVector =
    HomogeneousVector<DimensionV_, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Torque, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using TorqueVector2 = TorqueVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using TorqueVector3 = TorqueVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;

//! A point in time, a one-dimensional point in a coordinate system whose origin is an epoch, e.g. CoordinateSystem<Epoch>. The
//! difference of two time points is a Duration; a change of the epoch is a translation.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using TimePoint = HomogeneousPoint<1, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Time, CoordinateSystemT_>;

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using Duration = HomogeneousVector<1, ValuesT_, ValueT_, typename AccessUnits<AccessImplementation<ValueT_>>::Time, CoordinateSystemT_>;

template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimePoint = PositionPoint<DimensionV_, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimePoint2 = SpacetimePoint<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimePoint3 = SpacetimePoint<3, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimePoint4 = SpacetimePoint<4, ValuesT_, ValueT_, CoordinateSystemT_>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimeVector = PositionVector<DimensionV_, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimeVector2 = SpacetimeVector<2, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimeVector3 = SpacetimeVector<3, ValuesT_, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using SpacetimeVector4 = SpacetimeVector<4, ValuesT_, ValueT_, CoordinateSystemT_>;

}  // namespace wuzp
