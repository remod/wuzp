cmake_minimum_required(VERSION 3.12)
project(wuzp)

set(UNITS_IMPLEMENTATION_WUZP 0)
set(UNITS_IMPLEMENTATION_BOOST 1)
set(VALUE_MATRIX_IMPLEMENTATION_WUZP 0)
set(VALUE_MATRIX_IMPLEMENTATION_EIGEN 1)

option(FROM_SOURCE "True: Code is expected to lie in this directory. False: Code is expected to be installed." ON)
option(ENABLE_TESTS "Enables the definition of a 'tests' target to run the unit tests." OFF)
option(ENABLE_FORMAT "Enables the definition of a 'format' target to format the code." OFF)
option(ENABLE_TIDY "Enables the definition of a 'tidy' target to search for clang warnings." OFF)

# Use `set` instead of `option` for non-boolean values.
set(UNITS_IMPLEMENTATION ${UNITS_IMPLEMENTATION_WUZP} CACHE STRING "Define what kind of units implementation to use: ${UNITS_IMPLEMENTATION_WUZP} = WUZP, ${UNITS_IMPLEMENTATION_BOOST} = Boost.")
set(VALUE_MATRIX_IMPLEMENTATION ${VALUE_MATRIX_IMPLEMENTATION_EIGEN} CACHE STRING "Define what kind of matrix implementation to use: ${VALUE_MATRIX_IMPLEMENTATION_WUZP} = WUZP, ${VALUE_MATRIX_IMPLEMENTATION_EIGEN} = Eigen.")

message(STATUS "------------------------------------------")
message(STATUS "Building with the following configuration:")
message(STATUS "------------------------------------------")
message(STATUS "FROM_SOURCE:           ${FROM_SOURCE}")
message(STATUS "ENABLE_TESTS:          ${ENABLE_TESTS}")
message(STATUS "ENABLE_FORMAT:         ${ENABLE_FORMAT}")
message(STATUS "ENABLE_TIDY:           ${ENABLE_TIDY}")
message(STATUS "UNITS_IMPLEMENTATION:  ${UNITS_IMPLEMENTATION}")
message(STATUS "VALUE_MATRIX_IMPLEMENTATION: ${VALUE_MATRIX_IMPLEMENTATION}")
message(STATUS "------------------------------------------")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EIGEN_INITIALIZE_MATRICES_BY_ZERO ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_compile_definitions(UNITS_IMPLEMENTATION=${UNITS_IMPLEMENTATION})
add_compile_definitions(UNITS_IMPLEMENTATION_WUZP=${UNITS_IMPLEMENTATION_WUZP})
add_compile_definitions(UNITS_IMPLEMENTATION_BOOST=${UNITS_IMPLEMENTATION_BOOST})
add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION=${VALUE_MATRIX_IMPLEMENTATION})
add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION_WUZP=${VALUE_MATRIX_IMPLEMENTATION_WUZP})
add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION_EIGEN=${VALUE_MATRIX_IMPLEMENTATION_EIGEN})

add_subdirectory(wuzp)
if (UNITS_IMPLEMENTATION EQUAL UNITS_IMPLEMENTATION_BOOST)
    add_subdirectory(wuzp_boost)
endif()
if (VALUE_MATRIX_IMPLEMENTATION EQUAL VALUE_MATRIX_IMPLEMENTATION_EIGEN)
    add_subdirectory(wuzp_eigen)
endif()

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include)

add_executable(experimental src/experimental.cpp)
target_include_directories(experimental PRIVATE include)

if (ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    # The tests are ordered by complexity, basic tests come first.
    add_executable(tests
        test/CompilationError.cpp
        test/helpers.cpp
        test/Rational.cpp
        test/Integer.cpp
        test/Unit.cpp
        test/TypeMatrix.cpp
        test/CoordinateSystem.cpp
        test/UnitMatrix.cpp
        test/Quantity.cpp
        test/ValueMatrix.cpp
        test/QuantityMatrix.cpp
        test/CoordinateMatrix.cpp
        test/Tensor.cpp
        test/Translation.cpp
        test/Rotation.cpp
        test/Jacobian.cpp
        test/ProperRigidTransformation.cpp
    )
    # The source directory is required for the compilation tests.
    target_compile_options(tests PRIVATE --coverage)
    target_compile_definitions(tests PRIVATE CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)
    if (UNITS_IMPLEMENTATION EQUAL UNITS_IMPLEMENTATION_BOOST)
        target_link_libraries(tests PRIVATE wuzp_boost)
    endif()
    if (VALUE_MATRIX_IMPLEMENTATION EQUAL VALUE_MATRIX_IMPLEMENTATION_EIGEN)
        target_link_libraries(tests PRIVATE wuzp_eigen)
    endif()
    target_link_libraries(tests PRIVATE --coverage)
    gtest_discover_tests(tests)
endif()

if (ENABLE_FORMAT)
    find_program(CLANG_FORMAT "clang-format")
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/include/*.hpp
        ${PROJECT_SOURCE_DIR}/src/*.[ch]pp
        ${PROJECT_SOURCE_DIR}/test/*.[ch]pp
    )
    add_custom_target(format
        COMMAND clang-format -i --verbose -style=file ${ALL_CXX_SOURCE_FILES}
    )
endif()

if (ENABLE_TIDY)
    find_program(CLANG_TIDY "clang-tidy")
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/include/*.hpp
        ${PROJECT_SOURCE_DIR}/src/*.[ch]pp
        ${PROJECT_SOURCE_DIR}/test/*.[ch]pp
    )
    add_custom_target(tidy
        COMMAND clang-tidy --format-style=file -p build ${ALL_CXX_SOURCE_FILES}
    )
endif()
