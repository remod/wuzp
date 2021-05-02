cmake_minimum_required(VERSION 3.10.2)
project(ctm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Boost 1.56 REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(main Eigen3::Eigen)

if (ENABLE_TEST)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(tests
        test/TypeMatrix.cpp
        test/Frame.cpp
        test/Boost.cpp
        test/Eigen.cpp
        test/BoostEigenQuantityMatrix.cpp
        test/BoostEigenFramedQuantityMatrix.cpp
        test/BoostEigenProperRigidTransformation.cpp
    )
    target_include_directories(tests PRIVATE include ${Boost_INCLUDE_DIRS} GoogleTest)
    target_link_libraries(tests Eigen3::Eigen GTest::GTest GTest::Main)
    gtest_discover_tests(tests)
endif()

if (ENABLE_FORMAT)
    find_program(CLANG_FORMAT "clang-format")
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/*.[ch]pp
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
