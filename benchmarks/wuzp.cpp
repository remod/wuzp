// The computations of eigen.cpp and blitz.cpp with wuzp, which checks units, coordinate systems and points vs. vectors at
// compile time. Each benchmark has the same name as its counterpart on the plain library, with the suffix "wuzp".

#include <benchmark/benchmark.h>

#include <wuzp/wuzp.hpp>
#include <wuzp_units/Unit.hpp>

#include "value_matrix.hpp"

namespace {

struct A {};
struct B {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;
using CoordinateSystemB = wuzp::CoordinateSystem<B>;

template <typename CoordinateSystem>
using Velocity = wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystem>;
template <typename CoordinateSystem>
using Position = wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystem>;
using Acceleration = wuzp::AccelerationVector3<ValueMatrix, double, CoordinateSystemA>;
using Force = wuzp::ForceVector3<ValueMatrix, double, CoordinateSystemA>;
using Mass = QuantityD<wuzp_units::Mass>;
using Speed = QuantityD<wuzp_units::Velocity>;
using Transformation =
    wuzp::PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;

// A passive rotation of pi/2 around x, and a translation of 1 along z.
Transformation createTransformation() {
  return Transformation{Transformation::Rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0},
                        Transformation::Translation{0.0, 0.0, -1.0}};
}

void sum(benchmark::State& state) {
  Velocity<CoordinateSystemA> lhs{1.0, 2.0, 3.0};
  Velocity<CoordinateSystemA> rhs{4.0, 5.0, 6.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lhs);
    benchmark::DoNotOptimize(rhs);
    Velocity<CoordinateSystemA> result = lhs + rhs;
    benchmark::DoNotOptimize(result);
  }
}

void productWithScalar(benchmark::State& state) {
  Mass mass{2.0};
  Acceleration acceleration{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(mass);
    benchmark::DoNotOptimize(acceleration);
    Force result = mass * acceleration;
    benchmark::DoNotOptimize(result);
  }
}

void norm(benchmark::State& state) {
  Velocity<CoordinateSystemA> velocity{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(velocity);
    Speed result = velocity.norm();
    benchmark::DoNotOptimize(result);
  }
}

// A vector is only rotated, a point is rotated and translated.
void transformVector(benchmark::State& state) {
  Transformation transformation{createTransformation()};
  Velocity<CoordinateSystemA> velocity{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(transformation);
    benchmark::DoNotOptimize(velocity);
    Velocity<CoordinateSystemB> result = transformation.transform(velocity);
    benchmark::DoNotOptimize(result);
  }
}

void transformPoint(benchmark::State& state) {
  Transformation transformation{createTransformation()};
  Position<CoordinateSystemA> position{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(transformation);
    benchmark::DoNotOptimize(position);
    Position<CoordinateSystemB> result = transformation.transform(position);
    benchmark::DoNotOptimize(result);
  }
}

}  // namespace

BENCHMARK(sum)->Name("Sum/wuzp");
BENCHMARK(productWithScalar)->Name("ProductWithScalar/wuzp");
BENCHMARK(norm)->Name("Norm/wuzp");
BENCHMARK(transformVector)->Name("TransformVector/wuzp");
BENCHMARK(transformPoint)->Name("TransformPoint/wuzp");
