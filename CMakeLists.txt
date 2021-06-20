cmake_minimum_required(VERSION 3.10.2)
project(ctm)

option(ENABLE_TESTS "Enables the definition of 'tests', 'tests_boost', 'tests_eigen', and 'tests_boost_eigen' targets to run the unit tests." OFF)
option(ENABLE_FORMAT "Enables the definition of a 'format' target to format the code." OFF)
option(ENABLE_TIDY "Enables the definition of a 'tidy' target to search for clang warnings." OFF)
option(USE_BOOST "Add support for Boost units." ON)
option(USE_EIGEN "Add support for Eigen matrices." ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_definitions(-DUSE_BOOST=${USE_BOOST})
add_definitions(-DUSE_EIGEN=${USE_EIGEN})

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

    if (USE_BOOST)
        add_executable(tests_boost
            test/boost/Boost.cpp
            test/boost/Quantity.cpp
        )
        target_include_directories(tests_boost PRIVATE include ${Boost_INCLUDE_DIRS} GoogleTest)
        target_link_libraries(tests_boost GTest::GTest GTest::Main)
    endif()

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
