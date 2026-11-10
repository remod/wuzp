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
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/internal/translate.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * An active translation, which moves points by a vector t, p' = p + t, and leaves vectors, covectors and tensors with two upper
 * indices, e.g. covariances, unchanged (see the conventions in DESIGN.md). Other tensors are rejected, since they may depend on
 * the origin, e.g. an inertia about it (see internal::isTranslationInvariant). It only transforms objects expressed in its
 * coordinate system.
 */
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_,
          typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class ActiveTranslation : public ActiveTransformation<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>> {
 public:
  using ValueT = ValueT_;
  using ValuesT = ValuesT_<ValueT_, DimensionV_, 1>;
  using UnitsT = TypeMatrix<DimensionV_, 1, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<Indices<Up>, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<ActiveTranslation, Args...>)
  explicit ActiveTranslation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  const TensorT& getTensor() const { return tensor_; }
  //! The components of the translation, like getQuantities() of the linear transformations.
  const QuantitiesT& getQuantities() const { return tensor_.getQuantities(); }

  //! The inverse translation, by -t.
  ActiveTranslation inverse() const { return ActiveTranslation{-tensor_}; }

  //! Whether both translations, which must be expressed in the same coordinate system, are approximately equal: their vectors are
  //! compared like tensors (see Tensor::isApprox()).
  template <template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_, typename ValueOtherT_,
            typename CoordinateSystemOtherT_, typename... UnitsOtherT_>
  bool isApprox(const ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    return tensor_.isApprox(other.getTensor(), precision);
  }

 private:
  //! The vector t that points are moved by.
  TensorT tensor_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_>
struct GetCoordinateSystem<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>> {
  using type = CoordinateSystemT_;
};

// An active translation moves points, leaves vectors, covectors and tensors with two upper indices unchanged, and rejects the
// other tensors (see internal::translate()).
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_, typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>,
                            Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& transformation,
                        const Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    return internal::translate(object, transformation.getTensor().getQuantities());
  }
};

// The conjugation T_t T_s T_-t of a translation by a translation is the translation itself: translations commute.
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_, typename ValueOtherT_,
          typename CoordinateSystemOtherT_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>> {
  static auto transform(
      const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& transformation,
      const ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>(
        transformation.transform(object.getTensor()));
  }
};

// The composition of two translations is the translation by the sum of their vectors, t_lhs + t_rhs.
template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, typename... UnitsRhsT_>
struct CompositionTraits<ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>,
                         ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>> {
  static auto compose(const ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>& lhs,
                      const ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>& rhs) {
    return ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>{lhs.getTensor() +
                                                                                                            rhs.getTensor()};
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, typename... UnitsRhsT_>
bool operator==(const ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>& lhs,
                const ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, typename... UnitsRhsT_>
bool operator!=(const ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>& lhs,
                const ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_>
std::ostream& operator<<(std::ostream& lhs, const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialTranslation =
    ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, typename AccessUnits<AccessImplementation<ValueT_>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialTranslation2 = ActiveSpatialTranslation<ValuesT_, 2, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialTranslation3 = ActiveSpatialTranslation<ValuesT_, 3, ValueT_, CoordinateSystemT_>;

}  // namespace wuzp
