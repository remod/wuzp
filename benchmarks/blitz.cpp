// The computations of wuzp.cpp on plain Blitz++, without wuzp's checks. They use blitz::Array<double, 2> like wuzp's
// Blitz++ backend, so both allocate their results.

#include <cmath>

#include <benchmark/benchmark.h>
#include <blitz/array.h>

namespace {

using Array = blitz::Array<double, 2>;

Array createVector(double x, double y, double z) {
  Array vector(3, 1);
  vector = x, y, z;
  return vector;
}

// A passive rotation of pi/2 around x, and a translation of 1 along z.
Array createRotation() {
  Array rotation(3, 3);
  rotation = 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0;
  return rotation;
}
Array createTranslation() {
  return createVector(0.0, 0.0, -1.0);
}

void sum(benchmark::State& state) {
  Array lhs = createVector(1.0, 2.0, 3.0);
  Array rhs = createVector(4.0, 5.0, 6.0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(lhs);
    benchmark::DoNotOptimize(rhs);
    Array result(3, 1);
    result = lhs + rhs;
    benchmark::DoNotOptimize(result);
  }
}

void productWithScalar(benchmark::State& state) {
  double mass{2.0};
  Array acceleration = createVector(1.0, 2.0, 3.0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(mass);
    benchmark::DoNotOptimize(acceleration);
    Array result(3, 1);
    result = mass * acceleration;
    benchmark::DoNotOptimize(result);
  }
}

void norm(benchmark::State& state) {
  Array velocity = createVector(1.0, 2.0, 3.0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(velocity);
    double result = std::sqrt(blitz::sum(velocity * velocity));
    benchmark::DoNotOptimize(result);
  }
}

void transformVector(benchmark::State& state) {
  using blitz::tensor::i;
  using blitz::tensor::j;
  using blitz::tensor::k;
  Array rotation = createRotation();
  Array velocity = createVector(1.0, 2.0, 3.0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(rotation);
    benchmark::DoNotOptimize(velocity);
    Array result(3, 1);
    result = blitz::sum(rotation(i, k) * velocity(k, j), k);
    benchmark::DoNotOptimize(result);
  }
}

void transformPoint(benchmark::State& state) {
  using blitz::tensor::i;
  using blitz::tensor::j;
  using blitz::tensor::k;
  Array rotation = createRotation();
  Array translation = createTranslation();
  Array position = createVector(1.0, 2.0, 3.0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(rotation);
    benchmark::DoNotOptimize(translation);
    benchmark::DoNotOptimize(position);
    Array result(3, 1);
    result = blitz::sum(rotation(i, k) * position(k, j), k) + translation(i, j);
    benchmark::DoNotOptimize(result);
  }
}

}  // namespace

BENCHMARK(sum)->Name("Sum/blitz");
BENCHMARK(productWithScalar)->Name("ProductWithScalar/blitz");
BENCHMARK(norm)->Name("Norm/blitz");
BENCHMARK(transformVector)->Name("TransformVector/blitz");
BENCHMARK(transformPoint)->Name("TransformPoint/blitz");
