cmake_minimum_required(VERSION 3.12)
project(tests)

set(REPO_DIR ${PROJECT_SOURCE_DIR}/..)

set(UNITS_IMPLEMENTATION_WUZP 0)
set(UNITS_IMPLEMENTATION_BOOST 1)
set(VALUE_MATRIX_IMPLEMENTATION_WUZP 0)
set(VALUE_MATRIX_IMPLEMENTATION_EIGEN 1)

option(FROM_SOURCE "True: Build packages from source. False: Search for installed packages." ON)
option(ENABLE_TESTS "Enables the definition of a 'tests' target to run the unit tests." OFF)
option(ENABLE_CODE_COVERAGE "Enables the analysis of code coverage. Requires the tests to be enabled and the compiler to be clang respectively clang++." OFF)
option(ENABLE_FORMAT "Enables the definition of a 'format' target to format the code." OFF)
option(ENABLE_TIDY "Enables the definition of a 'tidy' target to search for clang tidy warnings." OFF)

# Use `set` instead of `option` for non-boolean values.
set(UNITS_IMPLEMENTATION ${UNITS_IMPLEMENTATION_WUZP} CACHE STRING "Define what kind of units implementation to use: ${UNITS_IMPLEMENTATION_WUZP} = WUZP, ${UNITS_IMPLEMENTATION_BOOST} = Boost.")
set(VALUE_MATRIX_IMPLEMENTATION ${VALUE_MATRIX_IMPLEMENTATION_EIGEN} CACHE STRING "Define what kind of matrix implementation to use: ${VALUE_MATRIX_IMPLEMENTATION_WUZP} = WUZP, ${VALUE_MATRIX_IMPLEMENTATION_EIGEN} = Eigen.")

message(STATUS "------------------------------------------")
message(STATUS "Building with the following configuration:")
message(STATUS "------------------------------------------")
message(STATUS "FROM_SOURCE:                 ${FROM_SOURCE}")
message(STATUS "CMAKE_C_COMPILER:            ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER:          ${CMAKE_CXX_COMPILER}")
message(STATUS "ENABLE_TESTS:                ${ENABLE_TESTS}")
message(STATUS "ENABLE_CODE_COVERAGE:        ${ENABLE_CODE_COVERAGE}")
message(STATUS "ENABLE_FORMAT:               ${ENABLE_FORMAT}")
message(STATUS "ENABLE_TIDY:                 ${ENABLE_TIDY}")
message(STATUS "UNITS_IMPLEMENTATION:        ${UNITS_IMPLEMENTATION}")
message(STATUS "VALUE_MATRIX_IMPLEMENTATION: ${VALUE_MATRIX_IMPLEMENTATION}")
message(STATUS "------------------------------------------")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_compile_definitions(FROM_SOURCE=${FROM_SOURCE})
add_compile_definitions(CMAKE_C_COMPILER=${CMAKE_C_COMPILER})
add_compile_definitions(CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
add_compile_definitions(UNITS_IMPLEMENTATION=${UNITS_IMPLEMENTATION})
add_compile_definitions(UNITS_IMPLEMENTATION_WUZP=${UNITS_IMPLEMENTATION_WUZP})
add_compile_definitions(UNITS_IMPLEMENTATION_BOOST=${UNITS_IMPLEMENTATION_BOOST})
add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION=${VALUE_MATRIX_IMPLEMENTATION})
add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION_WUZP=${VALUE_MATRIX_IMPLEMENTATION_WUZP})
add_compile_definitions(VALUE_MATRIX_IMPLEMENTATION_EIGEN=${VALUE_MATRIX_IMPLEMENTATION_EIGEN})

if (${FROM_SOURCE})
    add_subdirectory(${REPO_DIR}/wuzp wuzp_build)
else()
    find_package(wuzp REQUIRED)
endif()
if (${UNITS_IMPLEMENTATION} EQUAL ${UNITS_IMPLEMENTATION_BOOST})
    if (${FROM_SOURCE})
        add_subdirectory(${REPO_DIR}/wuzp_boost wuzp_boost_build)
    else()
        find_package(wuzp_boost REQUIRED)
    endif()
endif()
if (${VALUE_MATRIX_IMPLEMENTATION} EQUAL ${VALUE_MATRIX_IMPLEMENTATION_EIGEN})
    if (${FROM_SOURCE})
        add_subdirectory(${REPO_DIR}/wuzp_eigen wuzp_eigen_build)
    else()
        find_package(wuzp_eigen REQUIRED)
    endif()
endif()

if (${ENABLE_TESTS})
    enable_testing()
    if (${ENABLE_CODE_COVERAGE})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors")
    endif()
    find_package(GTest REQUIRED)
    # The tests are ordered by complexity, basic tests come first.
    add_executable(tests
        src/CompilationError.cpp
        src/helpers.cpp
        src/traits.cpp
        src/Rational.cpp
        src/Integer.cpp
        src/Unit.cpp
        src/TypeMatrix.cpp
        src/CoordinateSystem.cpp
        src/UnitMatrix.cpp
        src/Quantity.cpp
        src/Coordinate.cpp
        src/ValueMatrix.cpp
        src/QuantityMatrix.cpp
        src/CoordinateMatrix.cpp
        src/Tensor.cpp
        src/Translation.cpp
        src/Rotation.cpp
        src/Jacobian.cpp
        src/ProperRigidTransformation.cpp
        src/RotatingDumbbell.cpp
    )
    target_compile_definitions(tests PRIVATE REPO_DIR=${REPO_DIR})
    target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)
    target_link_libraries(tests PRIVATE wuzp)
    if (${UNITS_IMPLEMENTATION} EQUAL ${UNITS_IMPLEMENTATION_BOOST})
        # TODO(remod): Also use "namespaced" format.
        target_link_libraries(tests PRIVATE wuzp_boost)
    endif()
    if (${VALUE_MATRIX_IMPLEMENTATION} EQUAL ${VALUE_MATRIX_IMPLEMENTATION_EIGEN})
        target_link_libraries(tests PRIVATE wuzp_eigen)
    endif()
    gtest_discover_tests(tests)
    if (${ENABLE_CODE_COVERAGE})
        add_custom_target(code_coverage_preprocessing
                COMMAND LLVM_PROFILE_FILE=tests.profraw $<TARGET_FILE:tests>
                COMMAND llvm-profdata merge -sparse tests.profraw -o tests.profdata
                DEPENDS tests)
        add_custom_target(code_coverage_show
                COMMAND llvm-cov show $<TARGET_FILE:tests> -instr-profile=tests.profdata -show-line-counts-or-regions -show-instantiation-summary
                DEPENDS code_coverage_preprocessing)
        add_custom_target(code_coverage_console
                COMMAND llvm-cov report $<TARGET_FILE:tests> -instr-profile=tests.profdata
                DEPENDS code_coverage_preprocessing)
        add_custom_target(code_coverage_html
                COMMAND llvm-cov show $<TARGET_FILE:tests> -instr-profile=tests.profdata -show-line-counts-or-regions -show-instantiation-summary -output-dir=${PROJECT_SOURCE_DIR}/llvm-cov -format="html"
                DEPENDS code_coverage_preprocessing)
    endif()
endif()

if (${ENABLE_FORMAT} OR ${ENABLE_TIDY})
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${REPO_DIR}/wuzp/include/wuzp/*.hpp
        ${REPO_DIR}/wuzp/include/wuzp_units/*.hpp
        ${REPO_DIR}/wuzp_boost/include/wuzp_boost/*.hpp
        ${REPO_DIR}/wuzp_eigen/include/wuzp_eigen/*.hpp
        ${REPO_DIR}/tests/src/*.[ch]pp
    )
endif()

if (${ENABLE_FORMAT})
    find_program(CLANG_FORMAT "clang-format")
    add_custom_target(format
        COMMAND clang-format -i --verbose -style=file ${ALL_CXX_SOURCE_FILES}
    )
endif()

if (${ENABLE_TIDY})
    find_program(CLANG_TIDY "clang-tidy")
    add_custom_target(tidy
        COMMAND clang-tidy --format-style=file -p build ${ALL_CXX_SOURCE_FILES}
    )
endif()
