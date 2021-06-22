cmake_minimum_required(VERSION 3.12)
project(ctm)

option(ENABLE_TESTS "Enables the definition of 'tests', 'tests_boost', 'tests_eigen', and 'tests_boost_eigen' targets to run the unit tests." OFF)
option(ENABLE_FORMAT "Enables the definition of a 'format' target to format the code." OFF)
option(ENABLE_TIDY "Enables the definition of a 'tidy' target to search for clang warnings." OFF)
option(UNITS_IMPLEMENTATION "Define what kind of units implementation to use: 0 = CTM, 1 = Boost." 0)
option(MATRIX_IMPLEMENTATION "Define what kind of matrix implementation to use: 0 = CTM, 1 = Eigen." 0)

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

add_compile_definitions(UNITS_IMPLEMENTATION)
add_compile_definitions(UNITS_IMPLEMENTATION_CTM=0)
add_compile_definitions(UNITS_IMPLEMENTATION_BOOST=1)
add_compile_definitions(MATRIX_IMPLEMENTATION)
add_compile_definitions(MATRIX_IMPLEMENTATION_CTM=0)
add_compile_definitions(MATRIX_IMPLEMENTATION_EIGEN=1)

add_executable(main src/main.cpp)
if (USE_BOOST)
    find_package(Boost 1.56 REQUIRED)
    target_include_directories(main PRIVATE ${Boost_INCLUDE_DIRS})
endif()
if (USE_EIGEN)
    find_package(Eigen3 3.3 REQUIRED NO_MODULE)
    target_link_libraries(main Eigen3::Eigen)
endif()
target_include_directories(main PRIVATE include)

if (ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(tests
        test/CompilationError.cpp
        test/TypeMatrix.cpp
        test/Frame.cpp
    )
    target_include_directories(tests PRIVATE include GoogleTest)
    target_link_libraries(tests GTest::GTest GTest::Main)

    add_executable(tests_boost
        test/boost/Boost.cpp
        test/boost/Quantity.cpp
    )
    target_include_directories(tests_boost PRIVATE include GoogleTest)
    if (USE_BOOST)
        target_include_directories(tests_boost PRIVATE ${Boost_INCLUDE_DIRS})
    endif()
    target_link_libraries(tests_boost GTest::GTest GTest::Main)

    if (USE_EIGEN)
        add_executable(tests_eigen
            test/eigen/Eigen.cpp
        )
        target_include_directories(tests_eigen PRIVATE include GoogleTest)
        target_link_libraries(tests_eigen Eigen3::Eigen GTest::GTest GTest::Main)
    endif()

    if (USE_BOOST AND USE_EIGEN)
        add_executable(tests_boost_eigen
            test/boost_eigen/QuantityMatrix.cpp
            test/boost_eigen/FramedQuantityMatrix.cpp
            test/boost_eigen/ProperRigidTransformation.cpp
        )
        target_include_directories(tests_boost_eigen PRIVATE include ${Boost_INCLUDE_DIRS} GoogleTest)
        target_link_libraries(tests_boost_eigen Eigen3::Eigen GTest::GTest GTest::Main)
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
        COMMAND clang-format -i -style=file ${ALL_CXX_SOURCE_FILES}
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
