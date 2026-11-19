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
The generated code differs slightly, in both directions (e.g. GCC compiles plain Eigen's `mass * acceleration` in the benchmark loop to slower code).
Transforming a point with GCC is about 0.4 ns slower: GCC copies the point first (see [Generated code](#generated-code)).

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

## Generated code

`compare_codegen.py` compares the machine code that the compilers generate for each operation on its own, with Eigen.
`codegen/eigen.cpp` and `codegen/wuzp.cpp` define the operations of the benchmarks, and the composition of two rigid transformations, as functions that take their inputs by reference.
The script disassembles them from the object files, and counts their instructions, including those of the functions that the compiler did not inline, if the object file defines them (e.g. not those of the C library): it follows every call and jump out of a function, also to helpers in anonymous namespaces, to GCC's clones and to the cold parts of functions, and checks that it does on `codegen/self_check.cpp`.
It also says whether the instructions are identical, or the same in another order and with other SIMD registers.
The plain Eigen rigid transformation is a struct with the rotation matrix and the translation vector, in the order of *wuzp*'s members.
`ci/benchmarks.sh` runs the script after building the benchmarks, and leaves the listings in `build/<preset>/codegen`, e.g. for `diff build/gcc-benchmarks/codegen/compose.{eigen,wuzp}.txt`.
It supports x86-64 only, and skips the comparison on other architectures.
The functions are only built with GCC and Clang (the target `wuzp_codegen`), since `codegen/self_check.cpp` uses their extensions; with MSVC, only the benchmarks are built.

Number of instructions in the `latest` dev container (GCC 15.2, Clang 21.1, Eigen 5.0.1, `-O3`, x86-64):

| Operation | GCC, plain | GCC, *wuzp* | Clang, plain | Clang, *wuzp* |
| --- | --- | --- | --- | --- |
| `sum`: vector + vector | 10 | 10 (same) | 9 | 9 (same) |
| `productWithScalar`: mass * acceleration | 11 | 11 | 10 | 10 (same) |
| `norm` | 14 | 16 | 10 | 12 |
| `transformVector`: rigid transformation of a vector | 27 | 27 | 28 | 27 |
| `transformPoint`: rigid transformation of a point | 30 | 43 | 30 | 30 |
| `compose`: composition of two rigid transformations | 96 | 124 | 92 | 96 |

"Same" marks the same instructions in another order and with other registers; no operation compiles to identical code.
The differences have these causes:

* **Scheduling, register allocation and instruction selection.** Where the numbers are equal or close, the compilers do the same work with other instructions, e.g. GCC multiplies with a value in a register on one side, and with one in memory on the other, and Clang combines the elements of `transformVector` into SIMD loads differently. The data is laid out the same: a proper rigid transformation of *wuzp* is as large as its rotation and its translation, 96 bytes, like the plain struct, so both sides load the same offsets.
* **Quantities are returned in memory.** Values have a user-provided copy constructor, since the Blitz++ backend must copy arrays that Blitz++ would share (see `ValuesWrapper`), so no quantity or tensor is trivially copyable, also with Eigen (#79). The x86-64 calling convention therefore returns them through memory, even a single `double`: `norm` stores its result and returns its address, 2 instructions more than returning plain Eigen's `double` in a register. Once inlined, as in the benchmarks, this costs nothing.
* **GCC copies the point.** In `transformPoint`, GCC copies the point to the stack before transforming it, and the stack array then needs the stack protector's check (Ubuntu's compilers enable `-fstack-protector-strong`): 13 instructions more. Clang does not copy it.
* **GCC does not inline the composition.** In `compose`, GCC calls *wuzp*'s internal composition of rigid transformations instead of inlining it: the caller takes 15 instructions, with the stack protector's check, and the composition 109 instead of 96, since it keeps intermediate results on the stack. Clang inlines it, and also keeps two intermediate results on the stack.

## Compile time

The same computations take longer to compile with *wuzp*: it computes the types of all results, and it includes more headers (`wuzp.cpp` includes all of *wuzp* through `<wuzp/wuzp.hpp>`, and the Eigen backend includes `<Eigen/Geometry>` and `<Eigen/LU>` besides `<Eigen/Core>`).
Compiling `wuzp.cpp` vs. `eigen.cpp` (best of three runs, one translation unit each):

| Compiler | Plain Eigen | *wuzp* with Eigen |
| --- | --- | --- |
| GCC, `-O3` | 0.64 s | 0.78 s (+22%) |
| GCC, `-O0 -g` | 0.77 s | 1.12 s (+45%) |
| Clang, `-O3` | 0.96 s | 1.17 s (+22%) |
| Clang, `-O0 -g` | 0.99 s | 1.25 s (+26%) |

The overhead grows with the number of different types a translation unit uses, since each combination of units and coordinate systems is instantiated separately.
The backends do not check that they implement the interface of the core (`ValueMatrixBackend`, see "The backend interface" in [DESIGN.md](../DESIGN.md#the-backend-interface)) in their headers: with Eigen, this check takes about 0.7 s, which every translation unit would pay; the tests check it instead.
