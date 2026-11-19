# Benchmarks that compare wuzp with the same computations on the plain value matrix libraries (see README.md).
find_package(benchmark REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/WuzpWarnings.cmake)

# Builds the target with wuzp and the value matrix backend, which value_matrix.hpp selects.
function(wuzp_use_benchmark_backend target backend)
    target_compile_features(${target} PRIVATE cxx_std_20)
    wuzp_set_warnings(${target})
    string(TOUPPER ${backend} _backend)
    target_compile_definitions(${target} PRIVATE WUZP_BENCHMARK_${_backend}=1)
    target_link_libraries(${target} PRIVATE wuzp::wuzp wuzp::${backend})
endfunction()

# Adds wuzp_benchmarks_<backend>: the benchmarks of wuzp with this backend, and of the plain library.
function(wuzp_add_benchmarks backend)
    if(NOT TARGET wuzp::${backend})
        message(STATUS "Skipping the ${backend} benchmarks, the backend is not enabled.")
        return()
    endif()
    add_executable(wuzp_benchmarks_${backend} wuzp.cpp ${backend}.cpp)
    wuzp_use_benchmark_backend(wuzp_benchmarks_${backend} ${backend})
    target_link_libraries(wuzp_benchmarks_${backend} PRIVATE benchmark::benchmark_main)
endfunction()

wuzp_add_benchmarks(eigen)
wuzp_add_benchmarks(blitz)

# The operations of the benchmarks as functions of their own, on plain Eigen and with wuzp and Eigen, whose generated code
# compare_codegen.py compares (see "Generated code" in README.md). Only with GCC and Clang: self_check.cpp uses their extensions
# __builtin_expect and [[gnu::noinline]].
if(TARGET wuzp::eigen AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_library(wuzp_codegen OBJECT codegen/eigen.cpp codegen/wuzp.cpp codegen/self_check.cpp)
    wuzp_use_benchmark_backend(wuzp_codegen eigen)
endif()
