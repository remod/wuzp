// This example shows how to use wuzp to transform objects between coordinate systems.

#include <iostream>

#include <wuzp/core/CoordinateSystem.hpp>
#include <wuzp/core/Tensor.hpp>
#include <wuzp/trafo/PassiveProperRigidTransformation.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

struct A {};
struct B {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;
using CoordinateSystemB = wuzp::CoordinateSystem<B>;

template <typename CoordinateSystem>
using ForceVector = wuzp::ForceVector3<wuzp_eigen::ValueMatrix, double, CoordinateSystem>;

using ForceVectorInA = ForceVector<CoordinateSystemA>;
using ForceVectorInB = ForceVector<CoordinateSystemB>;

using TransformationFromAToB =
    wuzp::PassiveProperRigidTransformation3<wuzp_eigen::ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;

int main() {
  // Two forces to add:
  ForceVectorInA forceInA{1.0, 2.0, 3.0};
  ForceVectorInB forceInB{4.0, 5.0, 6.0};

  // This does not compile:
  // ForceInB totalForce = forceInA + forceInB;  // error: static assertion failed: Coordinate systems do not match.

  // Instead, we need to transform one of the forces into the coordinate system of the other force:
  TransformationFromAToB::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  TransformationFromAToB::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  TransformationFromAToB transformationFromAToB{rotation, translation};
  ForceVectorInB totalForce = transformationFromAToB.transform(forceInA) + forceInB;

  std::cout << "Resulting force:\n" << totalForce << std::endl;
  return 0;
}