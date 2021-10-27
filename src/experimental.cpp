#include <iostream>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/CoordinateSystemMatrix.hpp"
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/Rotation.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

int main() {
  using namespace wuzp;
  struct FromCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  QuantityMatrix<ValueMatrix<double, 3, 1>, UnitMatrix<3, 1, Length>> q(1.0, 2.0, 3.0);
  CoordinateMatrix<ValueMatrix<double, 3, 1>, UnitMatrix<3, 1, Length>, CoordinateSystemMatrix<3, 1, FromCoordinateSystem>> c(1.0, 2.0,
                                                                                                                              3.0);
  Tensor<1, 0, CoordinateMatrix<ValueMatrix<double, 3, 1>, UnitMatrix<3, 1, Length>, CoordinateSystemMatrix<3, 1, FromCoordinateSystem>>> t(
      1.0, 2.0, 3.0);
  std::cout << q << std::endl;
  std::cout << c << std::endl;
  std::cout << t << std::endl;
  //  struct FromCoordinateSystemName {};
  //  struct ToCoordinateSystemName {};
  //  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  //  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  //  using Object = PositionVector3D<double, FromCoordinateSystem>;
  //  using Transformation = Rotation3D<double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  //  using TransformedObject = PositionVector3D<double, ToCoordinateSystem>;
  //  const Object object(1.0, 2.0, 3.0);
  //  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  //  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  //  std::cout << getTypeName<GetCoordinateSystem<std::decay_t<decltype(transformation.transform(object))>>::type>() << std::endl;
  //  std::cout << getTypeName<GetCoordinateSystem<std::decay_t<decltype(transformedObject)>>::type>() << std::endl;
  //  transformation.transform(object) == transformedObject;
  return 0;
}
