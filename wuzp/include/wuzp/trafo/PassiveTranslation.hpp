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

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_,
          AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class PassiveTranslation
    : public PassiveTransformation<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>, FromT_,
                                   ToT_> {
 public:
  using ValuesT = ValuesT_<ValueT_, DimensionV_, 1>;
  using UnitsT = TypeMatrix<DimensionV_, 1, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<Indices<Up>, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<PassiveTranslation, Args...>)
  explicit PassiveTranslation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  const TensorT& getTensor() const { return tensor_; }
  //! The components of the translation, like getQuantities() of the linear transformations.
  const QuantitiesT& getQuantities() const { return tensor_.getQuantities(); }

 private:
  //! Vector from the origin of coordinate system To to the origin of coordinate system From, expressed in coordinate system
  //! CoordinateSystemT_. Adding it to a vector in the coordinate system From transforms it into the coordinate system To.
  TensorT tensor_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_, typename... UnitsT_>
struct GetCoordinateSystem<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>> {
  using type = CoordinateSystemT_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_, typename... UnitsT_, typename NewCoordinateSystemT_>
struct CastCoordinateSystem<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>,
                            NewCoordinateSystemT_> {
  using type = PassiveTranslation<ValuesT_, DimensionV_, ValueT_, NewCoordinateSystemT_, FromT_, ToT_, UnitsT_...>;
};

}  // namespace wuzp

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>& transformation,
      const Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // From and To have the same axes, so the translation has the same components in both. In any other coordinate system, its
    // components would have to be transformed first.
    static_assert(std::is_same_v<CoordinateSystemT_, FromT_> || std::is_same_v<CoordinateSystemT_, ToT_>,
                  "The translation must be expressed in the coordinate system From or To.");
    return castCoordinateSystem<ToT_>(internal::translate(object, transformation.getTensor().getQuantities()));
  }
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_,
          typename ValueOtherT_, typename CoordinateSystemOtherT_, typename FromOtherT_, typename ToOtherT_, typename... UnitsOtherT_>
struct TransformationTraits<
    PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>,
    PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_, UnitsOtherT_...>> {
  static auto transform(
      const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>& transformation,
      const PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_,
                               UnitsOtherT_...>& object) {
    return PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, ToT_, FromOtherT_, ToOtherT_, UnitsOtherT_...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_,
          typename CoordinateSystemOtherT_, typename FromOtherT_, typename ToOtherT_, int DimensionFromOtherV_, int DimensionToOtherV_,
          typename... UnitsOtherT_>
struct TransformationTraits<PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>,
                            PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_,
                                                        DimensionFromOtherV_, DimensionToOtherV_, UnitsOtherT_...>> {
  static auto transform(
      const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>& /*transformation*/,
      const PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_, DimensionFromOtherV_,
                                        DimensionToOtherV_, UnitsOtherT_...>& object) {
    // TODO(remod): We could use castCoordinateSystem instead, but we need to specialize it.
    return PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, ToT_, FromOtherT_, ToOtherT_, DimensionFromOtherV_, DimensionToOtherV_,
                                       UnitsOtherT_...>(object.getQuantities());
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename FromLhsT_, typename ToLhsT_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          int DimensionRhsV_, typename ValueRhsT_, typename CoordinateSystemRhsT_, typename FromRhsT_, typename ToRhsT_,
          typename... UnitsRhsT_>
bool operator==(
    const PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>& lhs,
    const PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>& rhs) {
  static_assert(std::is_same<FromLhsT_, FromRhsT_>::value, "From coordinates frames do not match.");
  static_assert(std::is_same<ToLhsT_, ToRhsT_>::value, "To coordinates frames do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename FromLhsT_, typename ToLhsT_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          int DimensionRhsV_, typename ValueRhsT_, typename CoordinateSystemRhsT_, typename FromRhsT_, typename ToRhsT_,
          typename... UnitsRhsT_>
bool operator!=(
    const PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>& lhs,
    const PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_, typename... UnitsT_>
std::ostream& operator<<(std::ostream& lhs,
                         const PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_, UnitsT_...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_, typename FromT_,
          typename ToT_>
using PassiveSpatialTranslation = PassiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, FromT_, ToT_,
                                                     typename AccessUnits<AccessImplementation<ValueT_>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_>
using PassiveSpatialTranslation2 = PassiveSpatialTranslation<ValuesT_, 2, ValueT_, CoordinateSystemT_, FromT_, ToT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_>
using PassiveSpatialTranslation3 = PassiveSpatialTranslation<ValuesT_, 3, ValueT_, CoordinateSystemT_, FromT_, ToT_>;

// v == 0 -> Translation
// a == 0 -> Uniform motion
// da/dt == 0 -> Motion with constant acceleration.

// omega == 0 -> Rotation
// phi == 0 -> Uniform circular motion
// phi/dt == 0 -> Circular motion with constant acceleration.

}  // namespace wuzp
