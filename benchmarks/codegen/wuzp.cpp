// The operations of eigen.cpp with wuzp and Eigen, which check units, coordinate systems and points vs. vectors at compile time,
// for the comparison of the generated code (see ../compare_codegen.py). Each operation is a function with external linkage that
// takes its inputs by reference, so that the compiler generates it on its own and computes nothing at compile time.

#include "../wuzp_types.hpp"

namespace codegen {

using wuzp_benchmarks::Acceleration;
using wuzp_benchmarks::CoordinateSystemA;
using wuzp_benchmarks::CoordinateSystemB;
using wuzp_benchmarks::CoordinateSystemC;
using wuzp_benchmarks::Force;
using wuzp_benchmarks::Mass;
using wuzp_benchmarks::Position;
using wuzp_benchmarks::Speed;
using wuzp_benchmarks::Transformation;
using wuzp_benchmarks::Velocity;

Velocity<CoordinateSystemA> sum(const Velocity<CoordinateSystemA>& lhs, const Velocity<CoordinateSystemA>& rhs);
Force productWithScalar(const Mass& mass, const Acceleration& acceleration);
Speed norm(const Velocity<CoordinateSystemA>& velocity);
Velocity<CoordinateSystemB> transformVector(const Transformation<CoordinateSystemA, CoordinateSystemB>& transformation,
                                            const Velocity<CoordinateSystemA>& velocity);
Position<CoordinateSystemB> transformPoint(const Transformation<CoordinateSystemA, CoordinateSystemB>& transformation,
                                           const Position<CoordinateSystemA>& position);
Transformation<CoordinateSystemA, CoordinateSystemC> compose(const Transformation<CoordinateSystemB, CoordinateSystemC>& lhs,
                                                             const Transformation<CoordinateSystemA, CoordinateSystemB>& rhs);

Velocity<CoordinateSystemA> sum(const Velocity<CoordinateSystemA>& lhs, const Velocity<CoordinateSystemA>& rhs) {
  return lhs + rhs;
}

Force productWithScalar(const Mass& mass, const Acceleration& acceleration) {
  return mass * acceleration;
}

Speed norm(const Velocity<CoordinateSystemA>& velocity) {
  return velocity.norm();
}

// A vector is only rotated, a point is rotated and translated.
Velocity<CoordinateSystemB> transformVector(const Transformation<CoordinateSystemA, CoordinateSystemB>& transformation,
                                            const Velocity<CoordinateSystemA>& velocity) {
  return transformation.transform(velocity);
}

Position<CoordinateSystemB> transformPoint(const Transformation<CoordinateSystemA, CoordinateSystemB>& transformation,
                                           const Position<CoordinateSystemA>& position) {
  return transformation.transform(position);
}

// X_CB * X_BA = X_CA: the composition of the passive transformations from A to B and from B to C.
Transformation<CoordinateSystemA, CoordinateSystemC> compose(const Transformation<CoordinateSystemB, CoordinateSystemC>& lhs,
                                                             const Transformation<CoordinateSystemA, CoordinateSystemB>& rhs) {
  return lhs * rhs;
}

}  // namespace codegen
