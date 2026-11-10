// This example shows how to use wuzp to transform objects between coordinate systems, and how to compose and invert
// transformations.

#include <exception>
#include <iostream>

#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

struct A {};
struct B {};
struct C {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;
using CoordinateSystemB = wuzp::CoordinateSystem<B>;
using CoordinateSystemC = wuzp::CoordinateSystem<C>;

template <typename CoordinateSystem>
using ForceVector = wuzp::ForceVector3<wuzp_eigen::ValueMatrix, double, CoordinateSystem>;
template <typename CoordinateSystem>
using PositionPoint = wuzp::PositionPoint3<wuzp_eigen::ValueMatrix, double, CoordinateSystem>;

using ForceVectorInA = ForceVector<CoordinateSystemA>;
using ForceVectorInB = ForceVector<CoordinateSystemB>;

// The passive transformation from From to To. In the monogram notation of Drake, TransformationFromAToB is X_BA: it maps the
// coordinates of a point in A to its coordinates in B.
template <typename From, typename To>
using RigidTransformation = wuzp::PassiveProperRigidTransformation3<wuzp_eigen::ValueMatrix, From, To, double>;
using TransformationFromAToB = RigidTransformation<CoordinateSystemA, CoordinateSystemB>;
using TransformationFromBToC = RigidTransformation<CoordinateSystemB, CoordinateSystemC>;

int main() {
  // In debug builds, the rotations check that their matrices are orthogonal with determinant 1, and throw
  // std::invalid_argument otherwise.
  try {
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

    // The rotation, followed by the translation, is the same proper rigid transformation.
    TransformationFromAToB composedTransformation = translation * rotation;
    std::cout << "Same transformation: " << std::boolalpha << composedTransformation.isApprox(transformationFromAToB) << std::endl;

    // Transformations compose in the order of Drake's monogram notation, X_CB * X_BA = X_CA: first from A to B, then from B to C.
    // The coordinate systems that meet must match, e.g. transformationFromAToB * transformationFromBToC does not compile.
    TransformationFromBToC transformationFromBToC{TransformationFromBToC::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                                  TransformationFromBToC::Translation{1.0, 0.0, 0.0}};
    RigidTransformation<CoordinateSystemA, CoordinateSystemC> transformationFromAToC = transformationFromBToC * transformationFromAToB;
    PositionPoint<CoordinateSystemA> point{1.0, 2.0, 3.0};
    std::cout << "Point in C:\n" << transformationFromAToC.transform(point) << std::endl;

    // The inverse transforms back, from C to A.
    RigidTransformation<CoordinateSystemC, CoordinateSystemA> transformationFromCToA = transformationFromAToC.inverse();
    std::cout << "Point in A again:\n" << transformationFromCToA.transform(transformationFromAToC.transform(point)) << std::endl;
  } catch (const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }
  return 0;
}
