cmake_minimum_required(VERSION 3.10.2)
project(ctm)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.56 REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include ${Boost_INCLUDE_DIRS})
target_link_libraries(main Eigen3::Eigen)

find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
    message(STATUS asdf)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/*.[ch]pp
            )

    add_custom_target(
            format
            COMMAND clang-format
            -i
            -style=file
            ${ALL_CXX_SOURCE_FILES}
    )
endif()
