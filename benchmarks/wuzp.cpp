// The computations of eigen.cpp and blitz.cpp with wuzp, which checks units, coordinate systems and points vs. vectors at
// compile time. Each benchmark has the same name as its counterpart on the plain library, with the suffix "wuzp".

#include <benchmark/benchmark.h>

#include "wuzp_types.hpp"

namespace {

using wuzp_benchmarks::Acceleration;
using wuzp_benchmarks::CoordinateSystemA;
using wuzp_benchmarks::CoordinateSystemB;
using wuzp_benchmarks::Force;
using wuzp_benchmarks::Mass;
using wuzp_benchmarks::Position;
using wuzp_benchmarks::Speed;
using wuzp_benchmarks::Velocity;
using Transformation = wuzp_benchmarks::Transformation<CoordinateSystemA, CoordinateSystemB>;

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
