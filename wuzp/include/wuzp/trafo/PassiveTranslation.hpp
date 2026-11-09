#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/internal/translate.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * A passive translation from the coordinate system From to the coordinate system To, which have the same axes but different
 * origins: a point p in From is the point p + t in To, and vectors keep their components. The objects it transforms must be
 * expressed in From.
 * Its vector t is the vector from the origin of To to the origin of From, expressed in To, i.e. p_AoBo_A of X_AB in the monogram
 * notation of Drake (https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) for From = B and To = A. From and To have
 * the same axes, so it has the same components in From.
 */
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, AnyCoordinateSystem FromT_,
          AnyCoordinateSystem ToT_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class PassiveTranslation
    : public PassiveTransformation<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>, FromT_, ToT_> {
 public:
  using ValueT = ValueT_;
  using ValuesT = ValuesT_<ValueT_, DimensionV_, 1>;
  using UnitsT = TypeMatrix<DimensionV_, 1, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using TensorT = Tensor<Indices<Up>, QuantitiesT, ToT_, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<PassiveTranslation, Args...>)
  explicit PassiveTranslation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  //! The vector t from the origin of To to the origin of From, expressed in To.
  const TensorT& getTensor() const { return tensor_; }
  //! The components of the translation, like getQuantities() of the linear transformations.
  const QuantitiesT& getQuantities() const { return tensor_.getQuantities(); }

  //! The inverse translation, from To to From, whose vector from the origin of From to the origin of To is -t, expressed in From.
  auto inverse() const { return typename internal::WithFrames<PassiveTranslation, ToT_, FromT_>::type{-getQuantities()}; }

  //! Whether both translations, which must transform between the same coordinate systems, are approximately equal: their vectors
  //! are compared like tensors (see Tensor::isApprox()).
  template <template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_, typename ValueOtherT_, typename FromOtherT_,
            typename ToOtherT_, typename... UnitsOtherT_>
  bool isApprox(const PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, FromOtherT_, ToOtherT_, UnitsOtherT_...>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameFrames<PassiveTranslation, std::remove_cvref_t<decltype(other)>>::value);
    return tensor_.isApprox(other.getTensor(), precision);
  }

 private:
  //! The vector from the origin of To to the origin of From, expressed in To. Adding it to a point in From gives the point in To.
  TensorT tensor_;
};

namespace internal {

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename FromT_, typename ToT_,
          typename... UnitsT_, typename NewFromT_, typename NewToT_>
struct WithFrames<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>, NewFromT_, NewToT_> {
  using type = PassiveTranslation<ValuesT_, DimensionV_, ValueT_, NewFromT_, NewToT_, UnitsT_...>;
};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename FromT_, typename ToT_,
          typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>& transformation,
                        const Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // The object is expressed in From (see PassiveTransformation::transform()), whose axes are those of To, in which the vector of
    // the translation is expressed.
    return castCoordinateSystem<ToT_>(internal::translate(object, transformation.getQuantities()));
  }
};

// The composition X_AB * X_BC = X_AC of two translations is the translation by the sum of their vectors, p_AoCo_A = p_AoBo_A +
// p_BoCo_B: A, B and C have the same axes, so the components of p_BoCo are the same in B and A.
template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename FromRhsT_, typename ToRhsT_, typename... UnitsRhsT_>
struct CompositionTraits<PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>,
                         PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>> {
  using LhsT = PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>;
  static auto compose(const LhsT& lhs,
                      const PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>& rhs) {
    return typename internal::WithFrames<LhsT, FromRhsT_, ToLhsT_>::type{lhs.getQuantities() + rhs.getQuantities()};
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename FromRhsT_, typename ToRhsT_, typename... UnitsRhsT_>
bool operator==(const PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>& lhs,
                const PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>& rhs) {
  static_assert(internal::SameFrames<std::remove_cvref_t<decltype(lhs)>, std::remove_cvref_t<decltype(rhs)>>::value);
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename FromRhsT_, typename ToRhsT_, typename... UnitsRhsT_>
bool operator!=(const PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>& lhs,
                const PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename FromT_, typename ToT_,
          typename... UnitsT_>
std::ostream& operator<<(std::ostream& lhs, const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, UnitsT_...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename FromT_, typename ToT_>
using PassiveSpatialTranslation =
    PassiveTranslation<ValuesT_, DimensionV_, ValueT_, FromT_, ToT_, typename AccessUnits<AccessImplementation<ValueT_>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using PassiveSpatialTranslation2 = PassiveSpatialTranslation<ValuesT_, 2, ValueT_, FromT_, ToT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using PassiveSpatialTranslation3 = PassiveSpatialTranslation<ValuesT_, 3, ValueT_, FromT_, ToT_>;

// v == 0 -> Translation
// a == 0 -> Uniform motion
// da/dt == 0 -> Motion with constant acceleration.

// omega == 0 -> Rotation
// phi == 0 -> Uniform circular motion
// phi/dt == 0 -> Circular motion with constant acceleration.

}  // namespace wuzp
