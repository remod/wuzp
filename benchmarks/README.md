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
ci/benchmarks.sh --benchmark_min_time=0.01s       # Quick check that they run, like CI.
```

## Results

Measured in the `latest` dev container (GCC 15.2, Clang 21.1, Eigen 5.0.1, Blitz++ 1.0.2, `-O3`) on an AMD Ryzen 9 8940HX.
Times are per operation; differences below about 0.2 ns are noise.
Google Benchmark warns that Ubuntu's build of it is a debug build; this affects its own bookkeeping, for both sides alike.

**Eigen:**

| Benchmark | GCC, plain | GCC, *wuzp* | Clang, plain | Clang, *wuzp* |
| --- | --- | --- | --- | --- |
| `Sum`: vector + vector | 0.44 ns | 0.52 ns | 0.45 ns | 0.43 ns |
| `ProductWithScalar`: mass * acceleration | 1.64 ns | 0.52 ns | 0.43 ns | 0.44 ns |
| `Norm` | 2.14 ns | 2.05 ns | 1.76 ns | 1.85 ns |
| `TransformVector`: rigid transformation of a vector | 1.26 ns | 1.11 ns | 1.00 ns | 1.13 ns |
| `TransformPoint`: rigid transformation of a point | 1.33 ns | 1.65 ns | 1.29 ns | 1.33 ns |

With Eigen, *wuzp* is as fast as plain Eigen: the units, coordinate systems and absolute/relative flags are types, which the compiler removes.
The generated code differs slightly, in both directions (e.g. GCC compiles plain Eigen's `mass * acceleration` to slower code).
Transforming a point with GCC is about 0.3 ns slower: the translation adds its components one by one, and GCC passes them through the stack.

**Blitz++:**

| Benchmark | GCC, plain | GCC, *wuzp* | Clang, plain | Clang, *wuzp* |
| --- | --- | --- | --- | --- |
| `Sum` | 19 ns | 29 ns | 29 ns | 44 ns |
| `ProductWithScalar` | 19 ns | 26 ns | 28 ns | 44 ns |
| `Norm` | 3 ns | 19 ns | 8 ns | 33 ns |
| `TransformVector` | 19 ns | 37 ns | 39 ns | 47 ns |
| `TransformPoint` | 20 ns | 137 ns | 40 ns | 197 ns |

With Blitz++, *wuzp* is slower.
Its Blitz++ backend stores every value matrix in a dynamically allocated `blitz::Array`, even a single value: the norm, for example, allocates for its result, and the translation of a point allocates for each component it adds.
Plain Blitz++ code avoids this with plain scalars.
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
