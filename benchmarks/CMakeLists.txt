# Benchmarks that compare wuzp with the same computations on the plain value matrix libraries (see README.md).
find_package(benchmark REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/WuzpWarnings.cmake)

# Adds wuzp_benchmarks_<backend>: the benchmarks of wuzp with this backend, and of the plain library.
function(wuzp_add_benchmarks backend)
    if(NOT TARGET wuzp::${backend})
        message(STATUS "Skipping the ${backend} benchmarks, the backend is not enabled.")
        return()
    endif()
    add_executable(wuzp_benchmarks_${backend} wuzp.cpp ${backend}.cpp)
    target_compile_features(wuzp_benchmarks_${backend} PRIVATE cxx_std_20)
    wuzp_set_warnings(wuzp_benchmarks_${backend})
    string(TOUPPER ${backend} _backend)
    target_compile_definitions(wuzp_benchmarks_${backend} PRIVATE WUZP_BENCHMARK_${_backend}=1)
    target_link_libraries(wuzp_benchmarks_${backend} PRIVATE wuzp::wuzp wuzp::${backend} benchmark::benchmark_main)
endfunction()

wuzp_add_benchmarks(eigen)
wuzp_add_benchmarks(blitz)
