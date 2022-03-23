#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int Dimension>
using LorentzTransformation = PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, Dimension, Dimension,
                                                          typename AccessUnits<SpecializationDummy<Scalar>>::Dimensionless>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using LorentzTransformation2 = LorentzTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, 2>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using LorentzTransformation3 = LorentzTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, 3>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using LorentzTransformation4 = LorentzTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, 4>;

template <typename ValueT_>
auto getSpeedOfLight() {
  return Quantity<ValueT_, typename AccessUnits<SpecializationDummy<ValueT_>>::Velocity>(static_cast<ValueT_>(299792458));
}

// TODO(remod): Why do we need to explicitly specialize the template arguments when using this function?
template <int DimensionV, template <typename, int, int> typename ValueMatrixT, typename ScalarT, typename CoordinateSystemT>
auto getInvariant(const SpacetimeVector<DimensionV, ValueMatrixT, ScalarT, CoordinateSystemT>& vector) {
  const auto quantityMatrix = vector.getQuantityMatrix();
  return pow<2>(quantityMatrix.template getQuantity<0, 0>()) - quantityMatrix.template block<1, 0, DimensionV - 1, 1>().squaredNorm();
}

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int Dimension>
LorentzTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, Dimension> createBoostMatrix(
    const VelocityVector<Dimension - 1, ValueMatrix, Scalar, CoordinateSystem>& velocity) {
  using QuantityMatrixT = typename LorentzTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, Dimension>::QuantityMatrixT;
  using QuantityT = Quantity<Scalar, typename AccessUnits<SpecializationDummy<Scalar>>::Dimensionless>;
  const auto squaredNorm = velocity.squaredNorm();
  const auto lorentzFactor = pow<-1, 2>(QuantityT(1.0) - squaredNorm / pow<2>(getSpeedOfLight<Scalar>()));
  QuantityMatrixT quantityMatrix;
  // Set the lower left matrix inclusive the diagonal.
  for (int row = 0; row < Dimension; ++row) {
    for (int col = 0; col < row + 1; ++col) {
      QuantityT quantity;
      if (row == 0 && col == 0) {
        quantity = lorentzFactor;
      } else if (col == 0) {
        quantity = -lorentzFactor / getSpeedOfLight<Scalar>() * velocity.getQuantityMatrix().getQuantity(row - 1, 0);
      } else if (col == row) {
        quantity =
            QuantityT(1.0) + (lorentzFactor - QuantityT(1.0)) * pow<2>(velocity.getQuantityMatrix().getQuantity(row - 1, 0)) / squaredNorm;
      } else {
        quantity = (lorentzFactor - QuantityT(1.0)) * velocity.getQuantityMatrix().getQuantity(col - 1, 0) *
                   velocity.getQuantityMatrix().getQuantity(row - 1, 0) / squaredNorm;
      }
      quantityMatrix.setQuantity(row, col, quantity);
    }
  }
  // Upper right matrix is mirrored.
  for (int row = 0; row < Dimension; ++row) {
    for (int col = row + 1; col < Dimension; ++col) {
      quantityMatrix.setQuantity(row, col, quantityMatrix.getQuantity(col, row));
    }
  }
  return LorentzTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, Dimension>(quantityMatrix);
}

}  // namespace wuzp
