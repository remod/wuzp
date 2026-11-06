#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int Dimension>
using LorentzTransformation = PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, Dimension, Dimension,
                                                          typename AccessUnits<AccessImplementation<Value>>::Dimensionless>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using LorentzTransformation2 = LorentzTransformation<ValuesT_, Value, CoordinateSystem, From, To, 2>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using LorentzTransformation3 = LorentzTransformation<ValuesT_, Value, CoordinateSystem, From, To, 3>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using LorentzTransformation4 = LorentzTransformation<ValuesT_, Value, CoordinateSystem, From, To, 4>;

template <typename ValueT_>
auto getSpeedOfLight() {
  return Quantity<typename AccessValues<AccessImplementation<ValueT_>>::template Value<ValueT_>,
                  typename AccessUnits<AccessImplementation<ValueT_>>::Velocity>{static_cast<ValueT_>(299792458)};
}

//! The Minkowski invariant v_0^2 - v_1^2 - ... - v_n^2 of a vector whose first component is the time component, e.g. of a
//! spacetime vector (c t, x, y, z). All components need the same unit.
//! It takes any vector rather than SpacetimeVector, whose alias template parameters could not be deduced.
template <typename QuantitiesT_, typename CoordinateSystemT_>
auto getInvariant(const Vector<QuantitiesT_, CoordinateSystemT_>& vector) {
  constexpr int dimension = Rows<QuantitiesT_>::value;
  const auto& quantities = vector.getQuantities();
  // Evaluate the result, which would otherwise refer to the quantities after returning.
  return (pow<2>(quantities.template get<0, 0>()) - quantities.template block<1, 0, dimension - 1, 1>().squaredNorm()).evaluate();
}

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int Dimension>
LorentzTransformation<ValuesT_, Value, CoordinateSystem, From, To, Dimension> createBoostMatrix(
    const VelocityVector<Dimension - 1, ValuesT_, Value, CoordinateSystem>& velocity) {
  using QuantitiesT = typename LorentzTransformation<ValuesT_, Value, CoordinateSystem, From, To, Dimension>::QuantitiesT;
  using QuantityT = Quantity<typename AccessValues<AccessImplementation<Value>>::template Value<Value>,
                             typename AccessUnits<AccessImplementation<Value>>::Dimensionless>;
  const auto squaredNorm{velocity.squaredNorm()};
  const auto lorentzFactor{pow<-1, 2>(QuantityT{1.0} - squaredNorm / pow<2>(getSpeedOfLight<Value>()))};
  // The spatial part uses (lorentzFactor - 1) / |v|^2 = lorentzFactor^2 / ((lorentzFactor + 1) c^2). The right-hand side is
  // finite for v = 0, where the left-hand side divides zero by zero, and numerically more stable for small velocities.
  const auto spatialFactor{(pow<2>(lorentzFactor) / ((lorentzFactor + QuantityT{1.0}) * pow<2>(getSpeedOfLight<Value>()))).evaluate()};
  QuantitiesT quantities;
  // Set the lower left matrix inclusive the diagonal.
  for (int row = 0; row < Dimension; ++row) {
    for (int col = 0; col < row + 1; ++col) {
      QuantityT quantity;
      // For row 0, the column is 0 too.
      if (row == 0) {
        quantity = lorentzFactor;
      } else if (col == 0) {
        quantity = -lorentzFactor / getSpeedOfLight<Value>() * velocity.getQuantities().get(row - 1, 0);
      } else if (col == row) {
        quantity = QuantityT{1.0} + spatialFactor * pow<2>(velocity.getQuantities().get(row - 1, 0));
      } else {
        quantity = spatialFactor * velocity.getQuantities().get(col - 1, 0) * velocity.getQuantities().get(row - 1, 0);
      }
      quantities.set(row, col, quantity);
    }
  }
  // Upper right matrix is mirrored.
  for (int row = 0; row < Dimension; ++row) {
    for (int col = row + 1; col < Dimension; ++col) {
      quantities.set(row, col, quantities.get(col, row));
    }
  }
  return LorentzTransformation<ValuesT_, Value, CoordinateSystem, From, To, Dimension>{quantities};
}

}  // namespace wuzp
