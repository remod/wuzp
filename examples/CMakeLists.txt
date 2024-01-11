cmake_minimum_required(VERSION 3.12)
project(examples)

set(REPO_DIR ${PROJECT_SOURCE_DIR}/..)

option(FROM_SOURCE "True: Build packages from source. False: Search for installed packages." ON)

message(STATUS "------------------------------------------")
message(STATUS "Building with the following configuration:")
message(STATUS "------------------------------------------")
message(STATUS "FROM_SOURCE:                 ${FROM_SOURCE}")
message(STATUS "CMAKE_C_COMPILER:            ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER:          ${CMAKE_CXX_COMPILER}")
message(STATUS "------------------------------------------")

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic)

if(${FROM_SOURCE})
    add_subdirectory(${REPO_DIR}/wuzp wuzp_build)
    add_subdirectory(${REPO_DIR}/wuzp_eigen wuzp_eigen_build)
else()
    find_package(wuzp REQUIRED)
    find_package(wuzp_eigen REQUIRED)
endif()

# Add executables for all cpp files in this directory.
file(GLOB EXAMPLES *.cpp)
foreach(EXAMPLE ${EXAMPLES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE wuzp)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE wuzp_eigen)
endforeach()
