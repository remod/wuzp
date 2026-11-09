#pragma once

#include <algorithm>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

// TODO(remod): Are two indices sufficient or do we need a boolean per dimension?
template <int ContravariantIndicesV_, int CovariantIndicesV_, AnyQuantities QuantitiesT_, AnyCoordinateSystem CoordinateSystemT_,
          AbsoluteOrRelative AbsoluteT_>
class Tensor {
 public:
  using QuantitiesT = QuantitiesT_;
  using CoordinateSystemT = CoordinateSystemT_;
  using UnitsT = typename QuantitiesT::UnitsT;
  using ValuesT = typename QuantitiesT::ValuesT;
  using ValueT = typename QuantitiesT::ValueT;
  using AbsoluteT = AbsoluteT_;
  using ThisT = Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT, CoordinateSystemT, AbsoluteT>;

  static_assert(ContravariantIndicesV_ >= 0, "Contravariant index must not be negative.");
  static_assert(CovariantIndicesV_ >= 0, "Covariant index must not be negative.");
  static_assert(ContravariantIndicesV_ + CovariantIndicesV_ <= 2, "Tensors of rank 3 or more are not supported at the moment.");

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

  template <int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
            typename AbsoluteOtherT_>
  bool isApprox(const Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_,
                             AbsoluteOtherT_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    // Note: Allow other tensor types to support lazy evaluation.
    static_assert(ContravariantIndicesV_ == ContravariantIndicesOtherV_, "Contravariant indices do not match.");
    static_assert(CovariantIndicesV_ == CovariantIndicesOtherV_, "Covariant indices do not match.");
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

  auto transpose() const { return typename Evaluate<typename Transpose<ThisT>::type>::type{quantities_.transpose()}; }

  // TODO(remod): Turn this to free functions? https://stackoverflow.com/a/8108821/2968115
  template <int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
            typename AbsoluteOtherT_>
  auto tensorProduct(const Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_,
                                  AbsoluteOtherT_>& other) const {
    return typename Evaluate<typename TensorProduct<ThisT, Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_,
                                                                  CoordinateSystemOtherT_, AbsoluteOtherT_>>::type>::type{
        quantities_.tensorProduct(other.getQuantities())};
  }

  template <int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
            typename AbsoluteOtherT_>
  auto innerProduct(const Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_,
                                 AbsoluteOtherT_>& other) const {
    return typename Evaluate<typename InnerProduct<ThisT, Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_,
                                                                 CoordinateSystemOtherT_, AbsoluteOtherT_>>::type>::type{
        quantities_.innerProduct(other.getQuantities())};
  }

  template <int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
            typename AbsoluteOtherT_>
  auto dotProduct(const Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_,
                               AbsoluteOtherT_>& other) const {
    return typename Evaluate<typename DotProduct<ThisT, Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_,
                                                               CoordinateSystemOtherT_, AbsoluteOtherT_>>::type>::type{
        quantities_.dotProduct(other.getQuantities())};
  }

  template <int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
            typename AbsoluteOtherT_>
  auto crossProduct(const Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_,
                                 AbsoluteOtherT_>& other) const {
    return typename Evaluate<typename CrossProduct<ThisT, Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_,
                                                                 CoordinateSystemOtherT_, AbsoluteOtherT_>>::type>::type{
        quantities_.crossProduct(other.getQuantities())};
  }

  auto inverse() const { return typename Evaluate<typename Inverse<ThisT>::type>::type{quantities_.inverse()}; }
  auto tensorInverse() const { return typename Evaluate<typename TensorInverse<ThisT>::type>::type{quantities_.tensorInverse()}; }

  auto norm() const
    requires(Dimension<ThisT>::value <= 1)
  {
    static_assert(!AbsoluteT_::value, "Tensor is not relative.");
    return quantities_.norm();
  }

  auto squaredNorm() const
    requires(Dimension<ThisT>::value <= 1)
  {
    static_assert(!AbsoluteT_::value, "Tensor is not relative.");
    return quantities_.squaredNorm();
  }

  //! Get the determinant of a tensor of rank 2.
  auto determinant() const
    requires(Dimension<ThisT>::value == 2)
  {
    return quantities_.determinant();
  }

  void hyperdeterminant() const { static_assert(FailOnInstantiation<ThisT>::value, "Not implemented"); }

  auto evaluate() const& { return typename Evaluate<ThisT>::type{quantities_.evaluate()}; }
  auto evaluate() && { return typename Evaluate<ThisT>::type{std::move(quantities_).evaluate()}; }

 private:
  template <int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
            typename AbsoluteOtherT_>
  // Allow other tensor types to support lazy evaluation.
  Tensor(FromSimilar /*tag*/,
         const Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>&
             other)
      : quantities_(other.getQuantities()) {
    // The converting constructor of the quantities checks the units.
    using ConversionT = IsValidConversion<
        ThisT, Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>>;
    static_assert(ConversionT::ContravariantIndicesMatchV, "Contravariant indices do not match.");
    static_assert(ConversionT::CovariantIndicesMatchV, "Covariant indices do not match.");
    static_assert(ConversionT::CoordinateSystemsMatchV, "Coordinate systems do not match.");
    // Points and vectors must not be converted into each other implicitly; use makeRelative() and makeAbsolute() instead.
    static_assert(ConversionT::AbsoluteMatchV, "Absolute/relative mismatch.");
  }

  template <typename... Ts>
  explicit Tensor(FromImplementationArgs /*tag*/, Ts&&... ts) : quantities_{std::forward<Ts>(ts)...} {}

  QuantitiesT quantities_;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_,
          int ContravariantIndicesOtherV_, int CovariantIndicesOtherV_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct IsValidConversion<
    Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>,
    Tensor<ContravariantIndicesOtherV_, CovariantIndicesOtherV_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  // The converting constructor checks these with static_asserts, which explain why a conversion fails, and the converting
  // constructor of the quantities checks their units.
  static constexpr bool ContravariantIndicesMatchV = ContravariantIndicesV_ == ContravariantIndicesOtherV_;
  static constexpr bool CovariantIndicesMatchV = CovariantIndicesV_ == CovariantIndicesOtherV_;
  static constexpr bool CoordinateSystemsMatchV = std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>;
  static constexpr bool AbsoluteMatchV = std::is_same_v<AbsoluteT_, AbsoluteOtherT_>;
  static constexpr bool value = ContravariantIndicesMatchV && CovariantIndicesMatchV && CoordinateSystemsMatchV && AbsoluteMatchV &&
                                IsValidConversion<QuantitiesT_, QuantitiesOtherT_>::value;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_,
          typename... Ts>
struct GetTag<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>, Ts...> {
  using first = std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>;
  using type = typename IfThenElse<sizeof...(Ts) == 1 && AnyTensor<first>, FromSimilar, FromImplementationArgs>::type;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Dimension<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr int value = Dimension<QuantitiesT_>::value;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Rows<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr int value = Rows<QuantitiesT_>::value;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Cols<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr int value = Cols<QuantitiesT_>::value;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Sum<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
           Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, typename Sum<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Sum<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Sum<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Difference<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                  Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  using type = Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, typename Difference<QuantitiesLhsT_, QuantitiesRhsT_>::type,
                      typename Difference<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type,
                      typename Difference<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Negation<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = Tensor<ContravariantIndicesV_, CovariantIndicesV_, typename Negation<QuantitiesT_>::type, CoordinateSystemT_, AbsoluteT_>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Product<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
               Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  using type =
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, typename Product<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
struct Product<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
               Quantities<ValuesRhsT_, UnitsRhsT_>> {
  using type = Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_,
                      typename Product<QuantitiesLhsT_, Quantities<ValuesRhsT_, UnitsRhsT_>>::type, CoordinateSystemLhsT_, AbsoluteLhsT_>;
};
template <typename ValuesLhsT_, typename UnitsLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Product<Quantities<ValuesLhsT_, UnitsLhsT_>,
               Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  using type =
      typename Product<Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>,
                       Quantities<ValuesLhsT_, UnitsLhsT_>>::type;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct TensorProduct<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                     Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  using type =
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, typename TensorProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct InnerProduct<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                    Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ + ContravariantIndicesRhsV_ >= 1, "Total contravariant indices must be at least 1.");
  static_assert(CovariantIndicesLhsV_ + CovariantIndicesRhsV_ >= 1, "Total covariant indices must be at least 1.");
  using type =
      Tensor<ContravariantIndicesLhsV_ + ContravariantIndicesRhsV_ - 1, CovariantIndicesLhsV_ + CovariantIndicesRhsV_ - 1,
             typename InnerProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct OuterProduct<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                    Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(std::min(ContravariantIndicesLhsV_, ContravariantIndicesRhsV_) <= 1,
                "Tensors with rank 3 or more are not supported. Smaller contravariant incides must be at most 1.");
  static_assert(std::min(CovariantIndicesLhsV_, CovariantIndicesRhsV_) <= 1,
                "Tensors with rank 3 or more are not supported. Smaller covariant incides must be at most 1.");
  using type =
      Tensor<ContravariantIndicesLhsV_ + ContravariantIndicesRhsV_, CovariantIndicesLhsV_ + CovariantIndicesRhsV_,
             typename OuterProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Product<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Product<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct DotProduct<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                  Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  static_assert(std::is_same_v<AbsoluteLhsT_, AbsoluteRhsT_>, "Absolute/relative mismatch.");
  using type = typename DotProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct CrossProduct<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                    Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == 1 && CovariantIndicesLhsV_ == 0, "Cross product is only defined for vectors.");
  static_assert(ContravariantIndicesRhsV_ == 1 && CovariantIndicesRhsV_ == 0, "Cross product is only defined for vectors.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  static_assert(!AbsoluteLhsT_::value && !AbsoluteRhsT_::value, "Cross product is only defined for relative tensors.");
  using type = Tensor<1, 0, typename CrossProduct<QuantitiesLhsT_, QuantitiesRhsT_>::type, CoordinateSystemLhsT_, AbsoluteLhsT_>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
struct Division<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>> {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  using type =
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, typename Division<QuantitiesLhsT_, QuantitiesRhsT_>::type,
             typename Division<CoordinateSystemLhsT_, CoordinateSystemRhsT_>::type, typename Division<AbsoluteLhsT_, AbsoluteRhsT_>::type>;
};
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Division<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_,
                      typename Division<QuantitiesLhsT_, Quantity<ValueRhsT_, UnitRhsT_>>::type, CoordinateSystemLhsT_, AbsoluteLhsT_>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Transpose<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = Tensor<ContravariantIndicesV_, CovariantIndicesV_, typename Transpose<QuantitiesT_>::type, CoordinateSystemT_, AbsoluteT_>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_,
          int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>, BlockRowIdV_, BlockColIdV_,
             BlockRowsV_, BlockColsV_> {
  using type =
      Tensor<ContravariantIndicesV_, CovariantIndicesV_,
             typename Block<QuantitiesT_, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_>::type, CoordinateSystemT_, AbsoluteT_>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Inverse<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = Tensor<CovariantIndicesV_, ContravariantIndicesV_, typename Inverse<QuantitiesT_>::type, CoordinateSystemT_,
                      typename Inverse<AbsoluteT_>::type>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TensorInverse<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  // TODO(remod): Is this correct?
  using type = Tensor<CovariantIndicesV_, ContravariantIndicesV_, typename TensorInverse<QuantitiesT_>::type, CoordinateSystemT_,
                      typename Inverse<AbsoluteT_>::type>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct GetCoordinateSystem<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = CoordinateSystemT_;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct IsAbsolute<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static constexpr bool value = AbsoluteT_::value;
};
template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct MakeRelative<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto make(Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_> object) {
    return Tensor<ContravariantIndicesV_, CovariantIndicesV_, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_, Relative>(
        std::move(object).getQuantities());
  }
};
template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct MakeAbsolute<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto make(Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_> object) {
    return Tensor<ContravariantIndicesV_, CovariantIndicesV_, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_, Absolute>(
        std::move(object).getQuantities());
  }
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_,
          typename NewCoordinateSystemT_>
struct CastCoordinateSystem<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>,
                            NewCoordinateSystemT_> {
  using type = Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, NewCoordinateSystemT_, AbsoluteT_>;
};

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct Evaluate<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  using type = Tensor<ContravariantIndicesV_, CovariantIndicesV_, typename Evaluate<QuantitiesT_>::type, CoordinateSystemT_, AbsoluteT_>;
};

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto sum(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
         const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Sum<
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
      Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      sum(lhs.getQuantities(), rhs.getQuantities())};
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator+(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return sum(lhs, rhs);
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto difference(
    const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
    const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Difference<
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
      Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      difference(lhs.getQuantities(), rhs.getQuantities())};
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator-(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return difference(lhs, rhs);
}

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
auto negation(const Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& rhs) {
  return typename Evaluate<typename Negation<Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_,
                                                    AbsoluteT_>>::type>::type(negation(rhs.getQuantities()));
}
template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
auto operator-(const Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& rhs) {
  return negation(rhs);
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_>
auto product(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const ValueRhsT_ rhs) {
  return product(lhs, Quantity<typename AccessValues<AccessImplementation<ValueRhsT_>>::template Value<ValueRhsT_>,
                               typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>(rhs));
}
template <typename ValueLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto product(const ValueLhsT_ lhs,
             const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(rhs, lhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto product(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, Quantities<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>,
                                 TypeMatrix<Rows<MatrixSizeRhsT_>::value, Cols<MatrixSizeRhsT_>::value,
                                            typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>>(rhs));
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, int ContravariantIndicesRhsV_,
          int CovariantIndicesRhsV_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto product(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
             const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(rhs, lhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto product(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return typename Evaluate<
      typename Product<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                       Quantities<ValuesRhsT_, UnitsRhsT_>>::type>::type{product(lhs.getQuantities(), rhs)};
}
template <typename ValuesLhsT_, typename UnitsLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto product(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs,
             const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(rhs, lhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto product(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
             const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Product<
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
      Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      product(lhs.getQuantities(), rhs.getQuantities())};
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const ValueRhsT_ rhs) {
  return product(lhs, rhs);
}
template <typename ValueLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator*(const ValueLhsT_ lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename MatrixSizeLhsT_, typename ValueLhsT_, typename ImplementationLhsT_, int ContravariantIndicesRhsV_,
          int CovariantIndicesRhsV_, typename QuantitiesRhsT_, typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator*(const ValuesWrapper<MatrixSizeLhsT_, ValueLhsT_, ImplementationLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValuesRhsT_, typename UnitsRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Quantities<ValuesRhsT_, UnitsRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <typename ValuesLhsT_, typename UnitsLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator*(const Quantities<ValuesLhsT_, UnitsLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return product(lhs, rhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator*(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  // TODO(remod): Introduce a macro to make the choice between innerProduct and tensor product configurable?
  return product(lhs, rhs);
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto tensorProduct(
    const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
    const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return lhs.tensorProduct(rhs);
}

// Currently limited to taking the last index of the lhs and the first index of the rhs.
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto innerProduct(
    const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
    const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return lhs.innerProduct(rhs);
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto outerProduct(
    const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
    const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename OuterProduct<
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
      Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      lhs.getQuantities().outerProduct(rhs.getQuantities())};
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_>
auto division(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const ValueRhsT_ rhs) {
  return division(lhs, Quantity<typename AccessValues<AccessImplementation<ValueRhsT_>>::template Value<ValueRhsT_>,
                                typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>(rhs));
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto division(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return division(lhs, Quantities<ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>,
                                  TypeMatrix<Rows<MatrixSizeRhsT_>::value, Cols<MatrixSizeRhsT_>::value,
                                             typename AccessUnits<AccessImplementation<ValueRhsT_>>::Dimensionless>>(rhs));
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto division(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Evaluate<
      typename Division<Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
                        Quantity<ValueRhsT_, UnitRhsT_>>::type>::type{division(lhs.getQuantities(), rhs)};
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto division(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
              const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return typename Evaluate<typename Division<
      Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>,
      Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>>::type>::type{
      lhs.getValues() / rhs.getValues()};
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_>
auto operator/(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const ValueRhsT_ rhs) {
  return division(lhs, rhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename MatrixSizeRhsT_, typename ValueRhsT_, typename ImplementationRhsT_>
auto operator/(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const ValuesWrapper<MatrixSizeRhsT_, ValueRhsT_, ImplementationRhsT_>& rhs) {
  return division(lhs, rhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator/(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return division(lhs, rhs);
}
template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
auto operator/(const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
               const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return division(lhs, rhs);
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
bool operator==(
    const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
    const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  static_assert(ContravariantIndicesLhsV_ == ContravariantIndicesRhsV_, "Contravariant indices do not match.");
  static_assert(CovariantIndicesLhsV_ == CovariantIndicesRhsV_, "Covariant indices do not match.");
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  static_assert(std::is_same_v<AbsoluteLhsT_, AbsoluteRhsT_>, "Absolute/relative mismatch.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <int ContravariantIndicesLhsV_, int CovariantIndicesLhsV_, typename QuantitiesLhsT_, typename CoordinateSystemLhsT_,
          typename AbsoluteLhsT_, int ContravariantIndicesRhsV_, int CovariantIndicesRhsV_, typename QuantitiesRhsT_,
          typename CoordinateSystemRhsT_, typename AbsoluteRhsT_>
bool operator!=(
    const Tensor<ContravariantIndicesLhsV_, CovariantIndicesLhsV_, QuantitiesLhsT_, CoordinateSystemLhsT_, AbsoluteLhsT_>& lhs,
    const Tensor<ContravariantIndicesRhsV_, CovariantIndicesRhsV_, QuantitiesRhsT_, CoordinateSystemRhsT_, AbsoluteRhsT_>& rhs) {
  return !(lhs == rhs);
}

template <int ContravariantIndicesV_, int CovariantIndicesV_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
std::ostream& operator<<(std::ostream& lhs,
                         const Tensor<ContravariantIndicesV_, CovariantIndicesV_, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Vector = Tensor<1, 0, QuantityVectorT_, CoordinateSystemT_, Relative>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
using HomogeneousVector = Vector<Quantities<ValuesT_<ValueT_, DimensionV_, 1>, TypeMatrix<DimensionV_, 1, UnitT_>>, CoordinateSystemT_>;

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Covector = Tensor<0, 1, QuantityVectorT_, CoordinateSystemT_, Relative>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
using HomogeneousCovector = Covector<Quantities<ValuesT_<ValueT_, 1, DimensionV_>, TypeMatrix<1, DimensionV_, UnitT_>>, CoordinateSystemT_>;

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Point = Tensor<1, 0, QuantityVectorT_, CoordinateSystemT_, Absolute>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
using HomogeneousPoint = Point<Quantities<ValuesT_<ValueT_, DimensionV_, 1>, TypeMatrix<DimensionV_, 1, UnitT_>>, CoordinateSystemT_>;

template <typename QuantityVectorT_, typename CoordinateSystemT_>
using Copoint = Tensor<0, 1, QuantityVectorT_, CoordinateSystemT_, Absolute>;
template <int DimensionV_, template <typename, int, int> typename ValuesT_, typename ValueT_, typename UnitT_, typename CoordinateSystemT_>
using HomogeneousCopoint = Copoint<Quantities<ValuesT_<ValueT_, 1, DimensionV_>, TypeMatrix<1, DimensionV_, UnitT_>>, CoordinateSystemT_>;

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
