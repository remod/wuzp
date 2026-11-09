#pragma once

#include <tuple>
#include <type_traits>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

// The indices of a tensor, and how operations combine them. The index placement follows chapter 1 of Carroll, Lecture Notes on
// General Relativity (https://arxiv.org/abs/gr-qc/9712019), on which his book Spacetime and Geometry is based: an upper index
// is contravariant, a lower index covariant, a contraction pairs an upper with a lower index, and the order of the indices
// matters (eqs. 1.59 and 1.60). The traits of this file check the indices of the operands and compute the indices of the
// result, like the traits of CoordinateSystem.hpp do for coordinate systems; Tensor.hpp combines them with the traits of the
// quantities.

namespace wuzp {

//! An upper, or contravariant, index, e.g. of a vector v^i, whose components transform like the coordinates.
struct Up {};

//! A lower, or covariant, index, e.g. of a covector w_i, whose components transform with the inverse of the coordinates.
struct Down {};

/**
 * The indices of a tensor, in their order, e.g. Indices<Up, Down> for a linear map T^i_j, Indices<Down, Down> for a bilinear
 * form such as a mass matrix M_ij, Indices<Up> for a vector and Indices<> for a scalar.
 *
 * The matrix of the components stores the first index as the row and the second index as the column, like the matrix M^i_j in
 * Carroll, eq. 1.54 (https://arxiv.org/abs/gr-qc/9712019), with vectors as columns and covectors as rows (eq. 1.39). A tensor of
 * rank 0 is stored as a 1x1 matrix, a vector (an upper index) as a column, a covector (a lower index) as a row, and a tensor of
 * rank 2 as a square matrix. A tensor with an upper index may also store a batch of vectors or points, one per column, e.g. a
 * point cloud, which transformations, sums, differences, comparisons, and products and quotients with scalars accept, but not the
 * operations that are only defined for a single vector, e.g. the inner product.
 */
template <UpOrDown... VariancesT_>
struct Indices {
  //! The number of indices.
  static constexpr int RankV = static_cast<int>(sizeof...(VariancesT_));
};

namespace internal {

//! The other variance: Down for Up and Up for Down.
template <typename VarianceT_>
struct Flip;
template <>
struct Flip<Up> {
  using type = Down;
};
template <>
struct Flip<Down> {
  using type = Up;
};

//! The first index, or void for rank 0.
template <typename IndicesT_>
struct FirstIndex {
  using type = void;
};
template <typename Variance0T_, typename... VariancesT_>
struct FirstIndex<Indices<Variance0T_, VariancesT_...>> {
  using type = Variance0T_;
};

//! The last index, or void for rank 0.
template <typename IndicesT_>
struct LastIndex {
  using type = void;
};
template <typename Variance0T_, typename... VariancesT_>
struct LastIndex<Indices<Variance0T_, VariancesT_...>> {
  using type = std::tuple_element_t<sizeof...(VariancesT_), std::tuple<Variance0T_, VariancesT_...>>;
};

//! The indices without the first one. The indices of tensors have rank 2 at most.
template <typename IndicesT_>
struct DropFirstIndex {
  using type = Indices<>;
};
template <typename Variance0T_, typename... VariancesT_>
struct DropFirstIndex<Indices<Variance0T_, VariancesT_...>> {
  using type = Indices<VariancesT_...>;
};

//! The indices without the last one. The indices of tensors have rank 2 at most.
template <typename IndicesT_>
struct DropLastIndex {
  using type = Indices<>;
};
template <typename Variance0T_, typename Variance1T_>
struct DropLastIndex<Indices<Variance0T_, Variance1T_>> {
  using type = Indices<Variance0T_>;
};

//! The indices of the left-hand side, followed by those of the right-hand side.
template <typename IndicesLhsT_, typename IndicesRhsT_>
struct ConcatenateIndices;
template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct ConcatenateIndices<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
  using type = Indices<VariancesLhsT_..., VariancesRhsT_...>;
};

}  // namespace internal

template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct Sum<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
  static_assert(std::is_same_v<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>>, "Indices do not match.");
  using type = Indices<VariancesLhsT_...>;
};

template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct Difference<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
  static_assert(std::is_same_v<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>>, "Indices do not match.");
  using type = Indices<VariancesLhsT_...>;
};

/**
 * The indices of the product of two tensors, one of which has rank 0: those of the other one. An elementwise (Hadamard) product of
 * tensors of rank 1 or more depends on the basis, so it is not a tensor. Like Eigen, which distinguishes matrices, whose product
 * is the matrix product, from arrays, whose product is elementwise
 * (https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html), and like mp-units, which defines `*` of vector quantities
 * only with a scalar (https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/character_of_a_quantity/), the other
 * products have names: innerProduct, outerProduct, and cwiseProduct for the elementwise product of the components.
 */
template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct Product<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
  static_assert(sizeof...(VariancesLhsT_) == 0 || sizeof...(VariancesRhsT_) == 0,
                "A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0. Use innerProduct, outerProduct or "
                "cwiseProduct for other products.");
  using type = std::conditional_t<sizeof...(VariancesLhsT_) == 0, Indices<VariancesRhsT_...>, Indices<VariancesLhsT_...>>;
};

//! The indices of the quotient of a tensor and a tensor of rank 0: those of the tensor. See Product for the reasons.
template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct Division<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
  static_assert(sizeof...(VariancesRhsT_) == 0,
                "A tensor can only be divided by a scalar, a quantity or a tensor of rank 0. Use cwiseQuotient for elementwise "
                "quotients.");
  using type = Indices<VariancesLhsT_...>;
};

/**
 * The indices of the inner product, which contracts the last index of the left-hand side with the first index of the right-hand
 * side: the remaining indices, in their order. A contraction pairs an upper with a lower index (Carroll, eq. 1.59,
 * https://arxiv.org/abs/gr-qc/9712019), e.g. w_i v^i, v^i w_i, T^i_j v^j, M_ij v^j or v^i M_ij. The only index of a vector or a
 * covector is both its first and its last index.
 */
template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct InnerProduct<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
 private:
  using LhsT = Indices<VariancesLhsT_...>;
  using RhsT = Indices<VariancesRhsT_...>;
  static constexpr bool HasIndicesV = sizeof...(VariancesLhsT_) > 0 && sizeof...(VariancesRhsT_) > 0;
  static constexpr bool OppositeV = !std::is_same_v<typename internal::LastIndex<LhsT>::type, typename internal::FirstIndex<RhsT>::type>;

 public:
  // Each failure reports one message: the second check assumes that the first one passes.
  static_assert(HasIndicesV,
                "An inner product contracts an index of each tensor, but a tensor of rank 0 has none. Multiply by it instead.");
  static_assert(!HasIndicesV || OppositeV,
                "An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index "
                "of the right-hand side are both upper or both lower.");
  using type = typename internal::ConcatenateIndices<typename internal::DropLastIndex<LhsT>::type,
                                                     typename internal::DropFirstIndex<RhsT>::type>::type;
};

//! The indices of the outer (tensor) product: those of the left-hand side, followed by those of the right-hand side, e.g. v^i w_j
//! has the indices (Up, Down). Tensors have rank 2 at most, so both tensors are vectors or covectors.
template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct OuterProduct<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
 private:
  static constexpr bool HasIndicesV = sizeof...(VariancesLhsT_) > 0 && sizeof...(VariancesRhsT_) > 0;

 public:
  static_assert(HasIndicesV, "An outer product with a tensor of rank 0 is a product. Multiply by it instead.");
  static_assert(!HasIndicesV || sizeof...(VariancesLhsT_) + sizeof...(VariancesRhsT_) <= 2,
                "The outer product would have rank 3 or more, which is not supported.");
  using type = Indices<VariancesLhsT_..., VariancesRhsT_...>;
};

template <typename... VariancesLhsT_, typename... VariancesRhsT_>
struct CrossProduct<Indices<VariancesLhsT_...>, Indices<VariancesRhsT_...>> {
  static_assert(std::is_same_v<Indices<VariancesLhsT_...>, Indices<Up>> && std::is_same_v<Indices<VariancesRhsT_...>, Indices<Up>>,
                "Cross product is only defined for vectors.");
  using type = Indices<Up>;
};

//! The indices of the transpose: the transpose of a tensor of rank 2 swaps its indices, e.g. (T^T)_j^i = T^i_j, and the transpose
//! of a vector, a column, is a covector, a row, and vice versa. The latter lowers or raises the index with the Euclidean metric,
//! which keeps the components (Carroll, eq. 1.62 and the text after it, https://arxiv.org/abs/gr-qc/9712019), like dotProduct
//! and norm (see "Metric" in DESIGN.md).
template <>
struct Transpose<Indices<>> {
  using type = Indices<>;
};
template <typename VarianceT_>
struct Transpose<Indices<VarianceT_>> {
  using type = Indices<typename internal::Flip<VarianceT_>::type>;
};
template <typename Variance0T_, typename Variance1T_>
struct Transpose<Indices<Variance0T_, Variance1T_>> {
  using type = Indices<Variance1T_, Variance0T_>;
};

//! The indices of the inverse: the indices are swapped and flipped, e.g. the inverse of a linear map T^i_j is a linear map, and
//! the inverse of a metric g_ij is g^ij (Carroll, eq. 1.56, https://arxiv.org/abs/gr-qc/9712019).
template <>
struct Inverse<Indices<>> {
  using type = Indices<>;
};
template <typename VarianceT_>
struct Inverse<Indices<VarianceT_>> {
  using type = Indices<typename internal::Flip<VarianceT_>::type>;
};
template <typename Variance0T_, typename Variance1T_>
struct Inverse<Indices<Variance0T_, Variance1T_>> {
  using type = Indices<typename internal::Flip<Variance1T_>::type, typename internal::Flip<Variance0T_>::type>;
};

template <typename... VariancesT_>
struct TensorInverse<Indices<VariancesT_...>> {
  using type = typename Inverse<Indices<VariancesT_...>>::type;
};

}  // namespace wuzp
