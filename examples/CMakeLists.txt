# The examples can be built in two ways:
# - As part of the wuzp project (WUZP_BUILD_EXAMPLES=ON).
# - As a standalone project against an installed wuzp package, which is how users consume wuzp.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.22...3.31)
    project(wuzp_examples LANGUAGES CXX)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    find_package(wuzp CONFIG REQUIRED COMPONENTS eigen)
elseif(NOT TARGET wuzp::eigen)
    message(STATUS "Skipping the examples, they require the Eigen backend.")
    return()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/WuzpWarnings.cmake)

file(GLOB WUZP_EXAMPLES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(_example IN LISTS WUZP_EXAMPLES)
    get_filename_component(_name ${_example} NAME_WE)
    add_executable(example_${_name} ${_example})
    target_compile_features(example_${_name} PRIVATE cxx_std_20)
    wuzp_set_warnings(example_${_name})
    target_link_libraries(example_${_name} PRIVATE wuzp::wuzp wuzp::eigen)
endforeach()
