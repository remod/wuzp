// The computations of wuzp.cpp on plain Eigen, without wuzp's checks.

#include <benchmark/benchmark.h>
#include <Eigen/Core>

namespace {

// A passive rotation of pi/2 around x, and a translation of 1 along z.
Eigen::Matrix3d createRotation() {
  return Eigen::Matrix3d{{1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, -1.0, 0.0}};
}
Eigen::Vector3d createTranslation() {
  return Eigen::Vector3d{0.0, 0.0, -1.0};
}

void sum(benchmark::State& state) {
  Eigen::Vector3d lhs{1.0, 2.0, 3.0};
  Eigen::Vector3d rhs{4.0, 5.0, 6.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lhs);
    benchmark::DoNotOptimize(rhs);
    Eigen::Vector3d result = lhs + rhs;
    benchmark::DoNotOptimize(result);
  }
}

void productWithScalar(benchmark::State& state) {
  double mass{2.0};
  Eigen::Vector3d acceleration{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(mass);
    benchmark::DoNotOptimize(acceleration);
    Eigen::Vector3d result = mass * acceleration;
    benchmark::DoNotOptimize(result);
  }
}

void norm(benchmark::State& state) {
  Eigen::Vector3d velocity{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(velocity);
    double result = velocity.norm();
    benchmark::DoNotOptimize(result);
  }
}

void transformVector(benchmark::State& state) {
  Eigen::Matrix3d rotation{createRotation()};
  Eigen::Vector3d velocity{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(rotation);
    benchmark::DoNotOptimize(velocity);
    Eigen::Vector3d result = rotation * velocity;
    benchmark::DoNotOptimize(result);
  }
}

void transformPoint(benchmark::State& state) {
  Eigen::Matrix3d rotation{createRotation()};
  Eigen::Vector3d translation{createTranslation()};
  Eigen::Vector3d position{1.0, 2.0, 3.0};
  for (auto _ : state) {
    benchmark::DoNotOptimize(rotation);
    benchmark::DoNotOptimize(translation);
    benchmark::DoNotOptimize(position);
    Eigen::Vector3d result = rotation * position + translation;
    benchmark::DoNotOptimize(result);
  }
}

}  // namespace

BENCHMARK(sum)->Name("Sum/eigen");
BENCHMARK(productWithScalar)->Name("ProductWithScalar/eigen");
BENCHMARK(norm)->Name("Norm/eigen");
BENCHMARK(transformVector)->Name("TransformVector/eigen");
BENCHMARK(transformPoint)->Name("TransformPoint/eigen");
