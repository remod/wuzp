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
# The archetypes (tests/src/archetype_units, tests/src/archetype_values) are minimal backends that provide exactly what the
# concepts of the backend interface require: the tests pass with them if the core uses nothing beyond the interface.
set(WUZP_TEST_UNITS wuzp CACHE STRING "Units backend to test against (wuzp, boost, mp_units, archetype).")
set(WUZP_TEST_VALUES eigen CACHE STRING "Value matrix backend to test against (eigen, blitz, archetype).")
set_property(CACHE WUZP_TEST_UNITS PROPERTY STRINGS wuzp boost mp_units archetype)
set_property(CACHE WUZP_TEST_VALUES PROPERTY STRINGS eigen blitz archetype)
option(WUZP_ENABLE_COVERAGE "Instrument the tests for source-based code coverage (Clang only)." OFF)
option(WUZP_ENABLE_SANITIZERS "Build the tests with AddressSanitizer and UndefinedBehaviorSanitizer (GCC and Clang)." OFF)

# The test sources select the backends using these preprocessor definitions.
set(WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION_WUZP=0 UNITS_IMPLEMENTATION_BOOST=1 UNITS_IMPLEMENTATION_MP_UNITS=2
    UNITS_IMPLEMENTATION_ARCHETYPE=3 VALUES_IMPLEMENTATION_EIGEN=1 VALUES_IMPLEMENTATION_BLITZ=2 VALUES_IMPLEMENTATION_ARCHETYPE=3)
set(WUZP_TEST_COMPONENTS)
if(WUZP_TEST_UNITS STREQUAL "wuzp")
    list(APPEND WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION=0)
elseif(WUZP_TEST_UNITS STREQUAL "boost")
    list(APPEND WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION=1)
    list(APPEND WUZP_TEST_COMPONENTS boost)
elseif(WUZP_TEST_UNITS STREQUAL "mp_units")
    list(APPEND WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION=2)
    list(APPEND WUZP_TEST_COMPONENTS mp_units)
elseif(WUZP_TEST_UNITS STREQUAL "archetype")
    list(APPEND WUZP_TEST_DEFINITIONS UNITS_IMPLEMENTATION=3)
else()
    message(FATAL_ERROR "Unknown WUZP_TEST_UNITS '${WUZP_TEST_UNITS}'.")
endif()
if(WUZP_TEST_VALUES STREQUAL "eigen")
    list(APPEND WUZP_TEST_DEFINITIONS VALUES_IMPLEMENTATION=1)
elseif(WUZP_TEST_VALUES STREQUAL "blitz")
    list(APPEND WUZP_TEST_DEFINITIONS VALUES_IMPLEMENTATION=2)
elseif(WUZP_TEST_VALUES STREQUAL "archetype")
    list(APPEND WUZP_TEST_DEFINITIONS VALUES_IMPLEMENTATION=3)
else()
    message(FATAL_ERROR "Unknown WUZP_TEST_VALUES '${WUZP_TEST_VALUES}'.")
endif()
# Both value matrix backends: the tests run against WUZP_TEST_VALUES, and ValueMatrixBackends.cpp uses both in one translation
# unit, which checks that they can be used side by side.
list(APPEND WUZP_TEST_COMPONENTS eigen blitz)

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

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/WuzpWarnings.cmake)
find_package(GTest REQUIRED)
include(GoogleTest)

# The tests are ordered by complexity, basic tests come first.
add_executable(wuzp_tests
    src/helpers.cpp
    src/Rational.cpp
    src/Integer.cpp
    src/Unit.cpp
    src/TypeMatrix.cpp
    src/Units.cpp
    src/ValuesWrapper.cpp
    src/Quantity.cpp
    src/Quantities.cpp
    src/Tensor.cpp
    src/ValueMatrixBackends.cpp
    src/Transformation.cpp
    src/ActiveLinearTransformation.cpp
    src/PassiveLinearTransformation.cpp
    src/ActiveTranslation.cpp
    src/PassiveTranslation.cpp
    src/ActiveAndPassiveTranslation.cpp
    src/ActiveRotation.cpp
    src/PassiveRotation.cpp
    src/ActiveAndPassiveRotation.cpp
    src/Jacobian.cpp
    src/ActiveProperRigidTransformation.cpp
    src/PassiveProperRigidTransformation.cpp
    src/ActiveAndPassiveProperRigidTransformation.cpp
    src/LorentzTransformation.cpp
    src/RigidBodyKinematics.cpp
    src/RotatingDumbbell.cpp
)
target_compile_features(wuzp_tests PRIVATE cxx_std_20)
wuzp_set_warnings(wuzp_tests)
target_compile_definitions(wuzp_tests PRIVATE ${WUZP_TEST_DEFINITIONS})
target_include_directories(wuzp_tests PRIVATE src)
target_link_libraries(wuzp_tests PRIVATE GTest::gtest_main ${WUZP_TEST_LINK_LIBRARIES})

if(WUZP_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "WUZP_ENABLE_COVERAGE requires Clang.")
    endif()
    set(_coverage_flags -fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors)
    target_compile_options(wuzp_tests PRIVATE ${_coverage_flags})
    target_link_options(wuzp_tests PRIVATE ${_coverage_flags})
endif()

if(WUZP_ENABLE_SANITIZERS)
    set(_sanitizer_flags -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
    target_compile_options(wuzp_tests PRIVATE ${_sanitizer_flags})
    target_link_options(wuzp_tests PRIVATE ${_sanitizer_flags})
endif()

gtest_discover_tests(wuzp_tests DISCOVERY_TIMEOUT 60)

# ----------------------------------------------------------------------------------------------------------------------
# Header check
# ----------------------------------------------------------------------------------------------------------------------

# Every public header compiles on its own, without warnings even if users enable stricter ones. The check covers the core
# and the backends of this configuration. It is a build target, so it runs with every build.
set(WUZP_CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../wuzp/include)
set(WUZP_HEADER_DIRS ${WUZP_CORE_INCLUDE_DIR})
foreach(_component IN LISTS WUZP_TEST_COMPONENTS)
    list(APPEND WUZP_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../wuzp_${_component}/include)
endforeach()
set(WUZP_HEADER_CHECK_SOURCES "")
foreach(_dir IN LISTS WUZP_HEADER_DIRS)
    file(GLOB_RECURSE _headers RELATIVE ${_dir} CONFIGURE_DEPENDS ${_dir}/*.hpp)
    foreach(_header IN LISTS _headers)
        string(MAKE_C_IDENTIFIER "${_header}" _name)
        set(_source ${CMAKE_CURRENT_BINARY_DIR}/header_check/${_name}.cpp)
        file(CONFIGURE OUTPUT ${_source} CONTENT "#include \"@_header@\"\n" @ONLY)
        list(APPEND WUZP_HEADER_CHECK_SOURCES ${_source})
    endforeach()
endforeach()
add_library(wuzp_header_check OBJECT ${WUZP_HEADER_CHECK_SOURCES})
target_compile_features(wuzp_header_check PRIVATE cxx_std_20)
target_compile_options(wuzp_header_check PRIVATE ${WUZP_STRICT_WARNING_OPTIONS})
target_link_libraries(wuzp_header_check PRIVATE ${WUZP_TEST_LINK_LIBRARIES})

# <wuzp/wuzp.hpp> includes every header of the core and the transformations.
file(READ ${WUZP_CORE_INCLUDE_DIR}/wuzp/wuzp.hpp _umbrella_header)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${WUZP_CORE_INCLUDE_DIR}/wuzp/wuzp.hpp)
file(GLOB _headers RELATIVE ${WUZP_CORE_INCLUDE_DIR} CONFIGURE_DEPENDS ${WUZP_CORE_INCLUDE_DIR}/wuzp/core/*.hpp
    ${WUZP_CORE_INCLUDE_DIR}/wuzp/trafo/*.hpp)
foreach(_header IN LISTS _headers)
    string(FIND "${_umbrella_header}" "#include \"${_header}\"" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "wuzp/wuzp.hpp does not include ${_header}.")
    endif()
endforeach()

# ----------------------------------------------------------------------------------------------------------------------
# Compile tests
# ----------------------------------------------------------------------------------------------------------------------

# Compile tests verify that code compiles, or that it fails to compile with a specific error (see
# compile_tests/CompileTest.cmake). Each case in compile_tests/cases is compiled at test time as part of a small
# project, which uses the same compiler, generator, toolchain and wuzp package as the unit tests.
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
set(WUZP_COMPILE_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/compile_tests)
configure_file(compile_tests/CMakeLists.txt.in ${WUZP_COMPILE_TEST_DIR}/CMakeLists.txt @ONLY)

# The configuration of the compile tests, read by compile_tests/run_compile_test.cmake.
string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" WUZP_COMPILE_TEST_COMPILER)
set(WUZP_COMPILE_TEST_UNITS ${WUZP_TEST_UNITS})
set(WUZP_COMPILE_TEST_VALUES ${WUZP_TEST_VALUES})
file(CONFIGURE OUTPUT ${WUZP_COMPILE_TEST_DIR}/config.cmake CONTENT [[
set(WUZP_COMPILE_TEST_CMAKELISTS "@WUZP_COMPILE_TEST_DIR@/CMakeLists.txt")
set(WUZP_COMPILE_TEST_GENERATOR "@CMAKE_GENERATOR@")
set(WUZP_COMPILE_TEST_MAKE_PROGRAM "@CMAKE_MAKE_PROGRAM@")
set(WUZP_COMPILE_TEST_CXX_COMPILER "@CMAKE_CXX_COMPILER@")
set(WUZP_COMPILE_TEST_TOOLCHAIN_FILE "@CMAKE_TOOLCHAIN_FILE@")
set(WUZP_COMPILE_TEST_COMPILER "@WUZP_COMPILE_TEST_COMPILER@")
set(WUZP_COMPILE_TEST_UNITS "@WUZP_COMPILE_TEST_UNITS@")
set(WUZP_COMPILE_TEST_VALUES "@WUZP_COMPILE_TEST_VALUES@")
]] @ONLY)

include(compile_tests/CompileTest.cmake)
file(GLOB WUZP_COMPILE_TEST_CASES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compile_tests/cases/*.cpp)
foreach(_case IN LISTS WUZP_COMPILE_TEST_CASES)
    get_filename_component(_name ${_case} NAME_WLE)
    add_test(NAME compile.${_name}
        COMMAND ${CMAKE_COMMAND} -D CONFIG=${WUZP_COMPILE_TEST_DIR}/config.cmake -D CASE=${_case}
            -D BUILD_DIR=${WUZP_COMPILE_TEST_DIR}/${_name} -P ${CMAKE_CURRENT_SOURCE_DIR}/compile_tests/run_compile_test.cmake
    )
    set_tests_properties(compile.${_name} PROPERTIES LABELS compile)
    # Cases whose requirements do not hold for this configuration are reported as disabled.
    wuzp_compile_test_parse(${_case} _case)
    if(NOT _case_ENABLED)
        set_tests_properties(compile.${_name} PROPERTIES DISABLED TRUE)
    endif()
endforeach()

# Every static_assert message in the public headers must be verified by a compile test.
add_test(NAME compile.diagnostics_coverage
    COMMAND ${CMAKE_COMMAND} -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/.. -P ${CMAKE_CURRENT_SOURCE_DIR}/compile_tests/check_diagnostics.cmake
)
set_tests_properties(compile.diagnostics_coverage PROPERTIES LABELS compile)
