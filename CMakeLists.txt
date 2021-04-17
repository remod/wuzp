cmake_minimum_required(VERSION 3.10.2)
project(ctm)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_CLANG_TIDY clang-tidy;-checks=*)

#find_program(IWYU_PATH NAMES include-what-you-use iwyu)
#if(NOT IWYU_PATH)
#    message(FATAL_ERROR "Could not find the program include-what-you-use")
#endif()
#set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})

find_package(Boost 1.56 REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(main Eigen3::Eigen)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(tests tests/test.cpp)
target_include_directories(tests PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(tests Eigen3::Eigen GTest::GTest GTest::Main)
gtest_discover_tests(tests)

#find_program(CLANG_FORMAT "clang-format")
#if (CLANG_FORMAT)
#    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
#            ${PROJECT_SOURCE_DIR}/*.[ch]pp
#            )
#
#    add_custom_target(
#            format
#            COMMAND clang-format
#            -i
#            -style=file
#            ${ALL_CXX_SOURCE_FILES}
#    )
#endif()

#find_program(CLANG_TIDY "clang-tidy")
#if(CLANG_TIDY)
#    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
#            ${PROJECT_SOURCE_DIR}/src/*.[ch]pp
#            ${PROJECT_SOURCE_DIR}/src/*.[ch]
#            ${PROJECT_SOURCE_DIR}/include/*.[h]pp
#            ${PROJECT_SOURCE_DIR}/include/*.[h]
#            )
#
#    add_custom_target(
#            tidy
#            COMMAND clang-tidy
#            ${ALL_CXX_SOURCE_FILES}
#            -format-style=file
#            --
#            -std=c++17
#            -I{PROJECT_ROOT_DIR}/include
#    )
#endif()

#set_property(TARGET main PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
