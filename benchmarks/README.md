# Benchmarks

*wuzp* checks units, coordinate systems and points vs. vectors at compile time.
These benchmarks measure what that costs at run time and at compile time, by comparing *wuzp* with the same computations on the plain value matrix libraries:

* `wuzp.cpp`: the computations with *wuzp*, compiled once with each value matrix backend.
* `eigen.cpp`, `blitz.cpp`: the same computations on plain Eigen and plain Blitz++, without *wuzp*.

Each benchmark has the same name in both, e.g. `TransformPoint/wuzp` and `TransformPoint/eigen`.
The inputs are hidden from the optimizer (`benchmark::DoNotOptimize`), so that nothing is computed at compile time.

## Running them

In the `latest` dev container, `ci/benchmarks.sh` builds the benchmarks in release mode with GCC and Clang, and runs them:

```bash
ci/benchmarks.sh                                  # Accurate numbers.
ci/benchmarks.sh --benchmark_repetitions=10 --benchmark_report_aggregates_only=true  # Medians, like the results below.
ci/benchmarks.sh --benchmark_min_time=0.01s       # Quick check that they run, like CI.
```

## Results

Measured in the `latest` dev container (GCC 15.2, Clang 21.1, Eigen 5.0.1, Blitz++ 1.0.2, `-O3`) on an AMD Ryzen 9 8940HX, as the median of 10 repetitions (`--benchmark_repetitions=10`).
Times are per operation; differences below about 0.2 ns are noise.
Google Benchmark warns that Ubuntu's build of it is a debug build; this affects its own bookkeeping, for both sides alike.

**Eigen:**

| Benchmark | GCC, plain | GCC, *wuzp* | Clang, plain | Clang, *wuzp* |
| --- | --- | --- | --- | --- |
| `Sum`: vector + vector | 0.40 ns | 0.40 ns | 0.40 ns | 0.40 ns |
| `ProductWithScalar`: mass * acceleration | 1.33 ns | 0.40 ns | 0.40 ns | 0.40 ns |
| `Norm` | 1.69 ns | 1.70 ns | 1.66 ns | 1.68 ns |
| `TransformVector`: rigid transformation of a vector | 1.01 ns | 0.92 ns | 0.92 ns | 1.02 ns |
| `TransformPoint`: rigid transformation of a point | 1.21 ns | 1.60 ns | 1.15 ns | 1.19 ns |

With Eigen, *wuzp* is as fast as plain Eigen: the units, coordinate systems and absolute/relative flags are types, which the compiler removes, and evaluating the result of every operation costs nothing measurable.
The generated code differs slightly, in both directions (e.g. GCC compiles plain Eigen's `mass * acceleration` to slower code).
Transforming a point with GCC is about 0.4 ns slower.

**Blitz++:**

| Benchmark | GCC, plain | GCC, *wuzp* | Clang, plain | Clang, *wuzp* |
| --- | --- | --- | --- | --- |
| `Sum` | 18 ns | 35 ns | 27 ns | 44 ns |
| `ProductWithScalar` | 17 ns | 31 ns | 25 ns | 45 ns |
| `Norm` | 3 ns | 16 ns | 7 ns | 29 ns |
| `TransformVector` | 17 ns | 40 ns | 34 ns | 78 ns |
| `TransformPoint` | 18 ns | 88 ns | 36 ns | 142 ns |

With Blitz++, *wuzp* is slower.
Its Blitz++ backend stores every value matrix in a dynamically allocated `blitz::Array`, even a single value: the norm, for example, allocates for its result.
Plain Blitz++ code avoids this with plain scalars.
Values also have value semantics with every backend, so the backend copies an array where plain Blitz++ code would share it, e.g. for the vector that a translation returns unchanged.
This is tracked in [#58](https://github.com/remod/wuzp/issues/58).

## Compile time

The same computations take longer to compile with *wuzp*: it computes the types of all results, and it includes more headers (`wuzp.cpp` includes all of *wuzp* through `<wuzp/wuzp.hpp>`, and the Eigen backend includes `<Eigen/Geometry>` and `<Eigen/LU>` besides `<Eigen/Core>`).
Compiling `wuzp.cpp` vs. `eigen.cpp` (best of three runs, one translation unit each):

| Compiler | Plain Eigen | *wuzp* with Eigen |
| --- | --- | --- |
| GCC, `-O3` | 0.68 s | 0.88 s (+29%) |
| GCC, `-O0 -g` | 0.84 s | 1.28 s (+54%) |
| Clang, `-O3` | 1.01 s | 1.25 s (+25%) |
| Clang, `-O0 -g` | 1.03 s | 1.31 s (+27%) |

The overhead grows with the number of different types a translation unit uses, since each combination of units and coordinate systems is instantiated separately.
