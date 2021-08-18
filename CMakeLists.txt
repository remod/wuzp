cmake_minimum_required(VERSION 3.12)
project(ctm)

set(UNITS_IMPLEMENTATION_CTM 0)
set(UNITS_IMPLEMENTATION_BOOST 1)
set(MATRIX_IMPLEMENTATION_CTM 0)
set(MATRIX_IMPLEMENTATION_EIGEN 1)

option(ENABLE_TESTS "Enables the definition of a 'tests' target to run the unit tests." OFF)
option(ENABLE_FORMAT "Enables the definition of a 'format' target to format the code." OFF)
option(ENABLE_TIDY "Enables the definition of a 'tidy' target to search for clang warnings." OFF)

# Use `set` instead of `option` for non-boolean values.
set(UNITS_IMPLEMENTATION ${UNITS_IMPLEMENTATION_CTM} CACHE STRING "Define what kind of units implementation to use: ${UNITS_IMPLEMENTATION_CTM} = CTM, ${UNITS_IMPLEMENTATION_BOOST} = Boost.")
set(MATRIX_IMPLEMENTATION ${MATRIX_IMPLEMENTATION_EIGEN} CACHE STRING "Define what kind of matrix implementation to use: ${MATRIX_IMPLEMENTATION_CTM} = CTM, ${MATRIX_IMPLEMENTATION_EIGEN} = Eigen.")

message(STATUS "------------------------------------------")
message(STATUS "Building with the following configuration:")
message(STATUS "------------------------------------------")
message(STATUS "ENABLE_TESTS:          ${ENABLE_TESTS}")
message(STATUS "ENABLE_FORMAT:         ${ENABLE_FORMAT}")
message(STATUS "ENABLE_TIDY:           ${ENABLE_TIDY}")
message(STATUS "UNITS_IMPLEMENTATION:  ${UNITS_IMPLEMENTATION}")
message(STATUS "MATRIX_IMPLEMENTATION: ${MATRIX_IMPLEMENTATION}")
message(STATUS "------------------------------------------")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_compile_definitions(UNITS_IMPLEMENTATION=${UNITS_IMPLEMENTATION})
add_compile_definitions(UNITS_IMPLEMENTATION_CTM=${UNITS_IMPLEMENTATION_CTM})
add_compile_definitions(UNITS_IMPLEMENTATION_BOOST=${UNITS_IMPLEMENTATION_BOOST})
add_compile_definitions(MATRIX_IMPLEMENTATION=${MATRIX_IMPLEMENTATION})
add_compile_definitions(MATRIX_IMPLEMENTATION_CTM=${MATRIX_IMPLEMENTATION_CTM})
add_compile_definitions(MATRIX_IMPLEMENTATION_EIGEN=${MATRIX_IMPLEMENTATION_EIGEN})

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include)
if (UNITS_IMPLEMENTATION EQUAL UNITS_IMPLEMENTATION_BOOST)
    find_package(Boost 1.56 REQUIRED)
    target_include_directories(main PRIVATE ${Boost_INCLUDE_DIRS})
endif()
if (MATRIX_IMPLEMENTATION EQUAL MATRIX_IMPLEMENTATION_EIGEN)
    find_package(Eigen3 3.3 REQUIRED NO_MODULE)
    target_link_libraries(main Eigen3::Eigen)
endif()

if (ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    # The tests are ordered by complexity, basic tests come first.
    add_executable(tests
        test/CompilationError.cpp
        test/TypeMatrix.cpp
        test/Frame.cpp
        test/Unit.cpp
        test/Quantity.cpp
        test/ValueMatrix.cpp
        test/QuantityMatrix.cpp
        test/FramedQuantityMatrix.cpp
        test/RotationMatrix.cpp
        test/ProperRigidTransformation.cpp
    )
    # The source directory is required for the compilation tests.
    target_compile_definitions(tests PRIVATE CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(tests PRIVATE include GoogleTest)
    if (UNITS_IMPLEMENTATION EQUAL UNITS_IMPLEMENTATION_BOOST)
        target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS})
    endif()
    target_link_libraries(tests GTest::GTest GTest::Main)
    if (MATRIX_IMPLEMENTATION EQUAL MATRIX_IMPLEMENTATION_EIGEN)
        target_link_libraries(tests Eigen3::Eigen)
    endif()
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
