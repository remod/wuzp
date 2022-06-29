cmake_minimum_required(VERSION 3.12)
project(tests)

set(REPO_DIR ${PROJECT_SOURCE_DIR}/..)

set(UNITS_IMPLEMENTATION_WUZP 0)
set(UNITS_IMPLEMENTATION_BOOST 1)
set(VALUES_IMPLEMENTATION_WUZP 0)
set(VALUES_IMPLEMENTATION_EIGEN 1)
set(VALUES_IMPLEMENTATION_BLITZ 2)

option(FROM_SOURCE "True: Build packages from source. False: Search for installed packages." ON)
option(ENABLE_TESTS "Enables the definition of a 'tests' target to run the unit tests." OFF)
option(ENABLE_CODE_COVERAGE "Enables the analysis of code coverage. Requires the tests to be enabled and the compiler to be clang respectively clang++." OFF)
option(ENABLE_FORMAT "Enables the definition of a 'format' target to format the code." OFF)
option(ENABLE_TIDY "Enables the definition of a 'tidy' target to search for clang tidy warnings." OFF)

# Use `set` instead of `option` for non-boolean values.
set(UNITS_IMPLEMENTATION ${UNITS_IMPLEMENTATION_WUZP} CACHE STRING "Define which kind of units implementation to use: ${UNITS_IMPLEMENTATION_WUZP} = WUZP, ${UNITS_IMPLEMENTATION_BOOST} = Boost.")
set(VALUES_IMPLEMENTATION ${VALUES_IMPLEMENTATION_EIGEN} CACHE STRING "Define which kind of values implementation to use: ${VALUES_IMPLEMENTATION_WUZP} = WUZP, ${VALUES_IMPLEMENTATION_EIGEN} = Eigen, ${VALUES_IMPLEMENTATION_BLITZ} = Blitz.")

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
message(STATUS "VALUES_IMPLEMENTATION:       ${VALUES_IMPLEMENTATION}")
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
add_compile_definitions(VALUES_IMPLEMENTATION=${VALUES_IMPLEMENTATION})
add_compile_definitions(VALUES_IMPLEMENTATION_WUZP=${VALUES_IMPLEMENTATION_WUZP})
add_compile_definitions(VALUES_IMPLEMENTATION_EIGEN=${VALUES_IMPLEMENTATION_EIGEN})
add_compile_definitions(VALUES_IMPLEMENTATION_BLITZ=${VALUES_IMPLEMENTATION_BLITZ})

if(${FROM_SOURCE})
    add_subdirectory(${REPO_DIR}/wuzp wuzp_build)
else()
    find_package(wuzp REQUIRED)
endif()
if(${UNITS_IMPLEMENTATION} EQUAL ${UNITS_IMPLEMENTATION_BOOST})
    if(${FROM_SOURCE})
        add_subdirectory(${REPO_DIR}/wuzp_boost wuzp_boost_build)
    else()
        find_package(wuzp_boost REQUIRED)
    endif()
endif()
if(${VALUES_IMPLEMENTATION} EQUAL ${VALUES_IMPLEMENTATION_EIGEN})
    if(${FROM_SOURCE})
        add_subdirectory(${REPO_DIR}/wuzp_eigen wuzp_eigen_build)
    else()
        find_package(wuzp_eigen REQUIRED)
    endif()
elseif(${VALUES_IMPLEMENTATION} EQUAL ${VALUES_IMPLEMENTATION_BLITZ})
    if(${FROM_SOURCE})
        add_subdirectory(${REPO_DIR}/wuzp_blitz wuzp_blitz_build)
    else()
        find_package(wuzp_blitz REQUIRED)
    endif()
endif()

if(${ENABLE_TESTS})
    enable_testing()
    if(${ENABLE_CODE_COVERAGE})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -O0 -fno-inline -fno-elide-constructors")
    endif()
    find_package(GTest REQUIRED)
    # The tests are ordered by complexity, basic tests come first.
    add_executable(tests
        src/test_compilation_error.cpp
        src/CompilationError.cpp
        src/helpers.cpp
        src/traits.cpp
        src/Rational.cpp
        src/Integer.cpp
        src/Unit.cpp
        src/TypeMatrix.cpp
        src/Units.cpp
        src/Quantity.cpp
        src/ValuesWrapper.cpp
        src/Quantities.cpp
        src/Tensor.cpp
        src/ActiveTranslation.cpp
        src/PassiveTranslation.cpp
        src/ActiveRotation.cpp
        src/PassiveRotation.cpp
        src/Jacobian.cpp
        src/ActiveProperRigidTransformation.cpp
        src/PassiveProperRigidTransformation.cpp
        src/LorentzTransformation.cpp
        src/RigidBodyKinematics.cpp
        src/RotatingDumbbell.cpp
    )
    target_compile_definitions(tests PRIVATE REPO_DIR=${REPO_DIR})
    target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)
    target_link_libraries(tests PRIVATE wuzp)
    if(${UNITS_IMPLEMENTATION} EQUAL ${UNITS_IMPLEMENTATION_BOOST})
        # TODO(remod): Also use "namespaced" format.
        target_link_libraries(tests PRIVATE wuzp_boost)
    endif()
    if(${VALUES_IMPLEMENTATION} EQUAL ${VALUES_IMPLEMENTATION_EIGEN})
        target_link_libraries(tests PRIVATE wuzp_eigen)
    elseif(${VALUES_IMPLEMENTATION} EQUAL ${VALUES_IMPLEMENTATION_BLITZ})
        target_link_libraries(tests PRIVATE wuzp_blitz)
    endif()
    gtest_discover_tests(tests)
    if(${ENABLE_CODE_COVERAGE})
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

if(${ENABLE_FORMAT} OR ${ENABLE_TIDY})
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        # A /*/ matches one or more levels of directories.
        ${REPO_DIR}/wuzp/include/*/*.hpp
        ${REPO_DIR}/wuzp_boost/include/*/*.hpp
        ${REPO_DIR}/wuzp_blitz/include/*/*.hpp
        ${REPO_DIR}/wuzp_eigen/include/*/*.hpp
        ${REPO_DIR}/tests/src/*.[ch]pp
    )
endif()

if(${ENABLE_FORMAT})
    find_program(CLANG_FORMAT "clang-format")
    add_custom_target(format
        COMMAND clang-format -i --verbose -style=file ${ALL_CXX_SOURCE_FILES}
    )
endif()

if(${ENABLE_TIDY})
    find_program(CLANG_TIDY "clang-tidy")
    add_custom_target(tidy
        COMMAND clang-tidy --format-style=file -p build ${ALL_CXX_SOURCE_FILES}
    )
endif()
