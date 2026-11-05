# The tests can be built in two ways:
# - As part of the wuzp project (WUZP_BUILD_TESTS=ON), testing the source tree.
# - As a standalone project, testing an installed wuzp package found via find_package.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.22...3.31)
    project(wuzp_tests LANGUAGES CXX)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    enable_testing()
    set(WUZP_TESTS_STANDALONE ON)
else()
    set(WUZP_TESTS_STANDALONE OFF)
endif()

# Each test build runs the whole test suite against one units backend and one value matrix backend.
set(WUZP_TEST_UNITS wuzp CACHE STRING "Units backend to test against (wuzp, boost).")
set(WUZP_TEST_VALUES eigen CACHE STRING "Value matrix backend to test against (eigen, blitz).")
set_property(CACHE WUZP_TEST_UNITS PROPERTY STRINGS wuzp boost)
set_property(CACHE WUZP_TEST_VALUES PROPERTY STRINGS eigen blitz)
option(WUZP_ENABLE_COVERAGE "Instrument the tests for source-based code coverage (Clang only)." OFF)

# The test sources select the backends using these preprocessor definitions.
set(WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION_WUZP=0 UNITS_IMPLEMENTATION_BOOST=1 VALUES_IMPLEMENTATION_EIGEN=1
    VALUES_IMPLEMENTATION_BLITZ=2)
set(WUZP_TEST_COMPONENTS)
if(WUZP_TEST_UNITS STREQUAL "wuzp")
    list(APPEND WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION=0)
elseif(WUZP_TEST_UNITS STREQUAL "boost")
    list(APPEND WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION=1)
    list(APPEND WUZP_TEST_COMPONENTS boost)
else()
    message(FATAL_ERROR "Unknown WUZP_TEST_UNITS '${WUZP_TEST_UNITS}'.")
endif()
if(WUZP_TEST_VALUES STREQUAL "eigen")
    list(APPEND WUZP_TEST_DEFINITIONS VALUES_IMPLEMENTATION=1)
elseif(WUZP_TEST_VALUES STREQUAL "blitz")
    list(APPEND WUZP_TEST_DEFINITIONS VALUES_IMPLEMENTATION=2)
else()
    message(FATAL_ERROR "Unknown WUZP_TEST_VALUES '${WUZP_TEST_VALUES}'.")
endif()
list(APPEND WUZP_TEST_COMPONENTS ${WUZP_TEST_VALUES})

set(WUZP_TEST_LINK_LIBRARIES wuzp::wuzp)
foreach(_component IN LISTS WUZP_TEST_COMPONENTS)
    list(APPEND WUZP_TEST_LINK_LIBRARIES wuzp::${_component})
endforeach()

if(WUZP_TESTS_STANDALONE)
    find_package(wuzp CONFIG REQUIRED COMPONENTS ${WUZP_TEST_COMPONENTS})
else()
    foreach(_component IN LISTS WUZP_TEST_COMPONENTS)
        if(NOT TARGET wuzp::${_component})
            string(TOUPPER ${_component} _upper)
            message(FATAL_ERROR "The tests need the '${_component}' backend. Configure with -DWUZP_WITH_${_upper}=ON.")
        endif()
    endforeach()
endif()

find_package(GTest REQUIRED)
include(GoogleTest)

# Some tests verify that invalid code does not compile. They compile small projects at test time, which are generated
# from this template so that they use the same compiler, generator and wuzp package as the tests themselves.
if(WUZP_TESTS_STANDALONE)
    set(WUZP_COMPILE_TEST_SETUP
        "list(PREPEND CMAKE_PREFIX_PATH \"${CMAKE_PREFIX_PATH}\")\nfind_package(wuzp CONFIG REQUIRED COMPONENTS ${WUZP_TEST_COMPONENTS})")
else()
    set(WUZP_COMPILE_TEST_SETUP "")
    foreach(_component IN LISTS WUZP_TEST_COMPONENTS)
        string(TOUPPER ${_component} _upper)
        string(APPEND WUZP_COMPILE_TEST_SETUP "set(WUZP_WITH_${_upper} ON)\n")
    endforeach()
    string(APPEND WUZP_COMPILE_TEST_SETUP
        "set(WUZP_BUILD_TESTS OFF)\nset(WUZP_BUILD_EXAMPLES OFF)\nadd_subdirectory(\"${PROJECT_SOURCE_DIR}\" wuzp_build)")
endif()
configure_file(compile_test/CMakeLists.txt.in ${CMAKE_CURRENT_BINARY_DIR}/compile_test/CMakeLists.txt @ONLY)
configure_file(compile_test/compile_test_config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/compile_test_config.hpp @ONLY)

# The tests are ordered by complexity, basic tests come first.
add_executable(wuzp_tests
    src/test_compilation_error.cpp
    src/CompilationError.cpp
    src/helpers.cpp
    src/traits.cpp
    src/Rational.cpp
    src/Integer.cpp
    src/Unit.cpp
    src/TypeMatrix.cpp
    src/Units.cpp
    src/ValuesWrapper.cpp
    src/Quantity.cpp
    src/Quantities.cpp
    src/Tensor.cpp
    src/ActiveTranslation.cpp
    src/PassiveTranslation.cpp
    src/ActiveAndPassiveTranslation.cpp
    src/ActiveRotation.cpp
    src/PassiveRotation.cpp
    src/ActiveAndPassiveRotation.cpp
    src/Jacobian.cpp
    src/ActiveProperRigidTransformation.cpp
    src/PassiveProperRigidTransformation.cpp
    src/LorentzTransformation.cpp
    src/RigidBodyKinematics.cpp
    src/RotatingDumbbell.cpp
)
target_compile_features(wuzp_tests PRIVATE cxx_std_20)
target_compile_options(wuzp_tests PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(wuzp_tests PRIVATE ${WUZP_TEST_DEFINITIONS})
if(WUZP_TEST_VALUES STREQUAL "blitz")
    target_compile_definitions(wuzp_tests PRIVATE BZ_USE_NUMTRAIT=OFF BZ_HAVE_TEMPLATE_QUALIFIED_RETURN_TYPE=OFF)
endif()
target_include_directories(wuzp_tests PRIVATE src ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(wuzp_tests PRIVATE GTest::gtest_main ${WUZP_TEST_LINK_LIBRARIES})

if(WUZP_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "WUZP_ENABLE_COVERAGE requires Clang.")
    endif()
    set(_coverage_flags -fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors)
    target_compile_options(wuzp_tests PRIVATE ${_coverage_flags})
    target_link_options(wuzp_tests PRIVATE ${_coverage_flags})
endif()

gtest_discover_tests(wuzp_tests DISCOVERY_TIMEOUT 60)
