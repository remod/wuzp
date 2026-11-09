#pragma once

#include <stdexcept>

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionV_>
using LorentzTransformation = PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionV_, DimensionV_,
                                                          typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_>
using LorentzTransformation2 = LorentzTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, 2>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_>
using LorentzTransformation3 = LorentzTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, 3>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_>
using LorentzTransformation4 = LorentzTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, 4>;

//! The speed of light, as a quantity of the value matrix backend ValuesT_, e.g. getSpeedOfLight<wuzp_eigen::ValueMatrix, double>().
template <template <typename, int, int> typename ValuesT_, typename ValueT_>
auto getSpeedOfLight() {
  return Quantity<ValuesT_<ValueT_, 1, 1>, typename AccessUnits<AccessImplementation<ValueT_>>::Velocity>{static_cast<ValueT_>(299792458)};
}

//! The Minkowski invariant v_0^2 - v_1^2 - ... - v_n^2 of a vector whose first component is the time component, e.g. of a
//! spacetime vector (c t, x, y, z). All components need the same unit.
//! It takes any vector rather than SpacetimeVector, whose alias template parameters could not be deduced.
template <typename QuantitiesT_, typename CoordinateSystemT_>
auto getInvariant(const Vector<QuantitiesT_, CoordinateSystemT_>& vector) {
  static_assert(internal::RejectBatch<Indices<Up>, QuantitiesT_>::value);
  constexpr int dimension = Rows<QuantitiesT_>::value;
  const auto& quantities = vector.getQuantities();
  return pow<2>(quantities.template get<0, 0>()) - quantities.template block<1, 0, dimension - 1, 1>().squaredNorm();
}

/**
 * The passive Lorentz boost from From to To, in the standard form with the time component first and in length units
 * (x^0 = c t), and the signature (+, -, ..., -): x'^0 = gamma (x^0 - beta . x), with beta = v / c and the Lorentz factor
 * gamma = 1 / sqrt(1 - beta^2) (see the boost matrix B(v) in https://en.wikipedia.org/wiki/Lorentz_transformation#Proper_transformations).
 * @param velocity Velocity of To relative to From, expressed in From. Its magnitude must be smaller than the speed of light.
 * @throws std::invalid_argument If the magnitude of the velocity is not smaller than the speed of light, for which there is no
 * Lorentz boost (the Lorentz factor is infinite or not real).
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionV_>
LorentzTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionV_> createBoostMatrix(
    const VelocityVector<DimensionV_ - 1, ValuesT_, ValueT_, CoordinateSystemT_>& velocity) {
  using QuantitiesT = typename LorentzTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionV_>::QuantitiesT;
  using QuantityT = Quantity<ValuesT_<ValueT_, 1, 1>, typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  const QuantityT squaredBeta{velocity.squaredNorm() / pow<2>(getSpeedOfLight<ValuesT_, ValueT_>())};
  // Also rejects NaN.
  if (!(squaredBeta.getValue() < 1)) {
    throw std::invalid_argument("The magnitude of the velocity of a Lorentz boost must be smaller than the speed of light.");
  }
  const auto lorentzFactor{pow<-1, 2>(QuantityT{1.0} - squaredBeta)};
  // The spatial part uses (lorentzFactor - 1) / |v|^2 = lorentzFactor^2 / ((lorentzFactor + 1) c^2). The right-hand side is
  // finite for v = 0, where the left-hand side divides zero by zero, and numerically more stable for small velocities.
  const auto spatialFactor{pow<2>(lorentzFactor) / ((lorentzFactor + QuantityT{1.0}) * pow<2>(getSpeedOfLight<ValuesT_, ValueT_>()))};
  QuantitiesT quantities;
  // Set the lower left matrix inclusive the diagonal.
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = 0; col < row + 1; ++col) {
      QuantityT quantity;
      // For row 0, the column is 0 too.
      if (row == 0) {
        quantity = lorentzFactor;
      } else if (col == 0) {
        quantity = -lorentzFactor / getSpeedOfLight<ValuesT_, ValueT_>() * velocity.getQuantities().get(row - 1, 0);
      } else if (col == row) {
        quantity = QuantityT{1.0} + spatialFactor * pow<2>(velocity.getQuantities().get(row - 1, 0));
      } else {
        quantity = spatialFactor * velocity.getQuantities().get(col - 1, 0) * velocity.getQuantities().get(row - 1, 0);
      }
      quantities.set(row, col, quantity);
    }
  }
  // Upper right matrix is mirrored.
  for (int row = 0; row < DimensionV_; ++row) {
    for (int col = row + 1; col < DimensionV_; ++col) {
      quantities.set(row, col, quantities.get(col, row));
    }
  }
  return LorentzTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionV_>{quantities};
}

}  // namespace wuzp
