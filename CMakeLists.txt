cmake_minimum_required(VERSION 3.22...3.31)

project(wuzp
    VERSION 0.1.0
    DESCRIPTION "Wrong Unit? Zero Points! Compile-time unit and coordinate system safety for linear algebra."
    HOMEPAGE_URL "https://github.com/remod/wuzp"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# wuzp does not use C++20 modules. Scanning for them would require clang-scan-deps with Clang and CMake >= 3.28.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
include(GNUInstallDirs)

# ----------------------------------------------------------------------------------------------------------------------
# Options
# ----------------------------------------------------------------------------------------------------------------------

option(WUZP_BUILD_TESTS "Build the unit tests." ${PROJECT_IS_TOP_LEVEL})
option(WUZP_BUILD_EXAMPLES "Build the examples (requires the Eigen backend)." ${PROJECT_IS_TOP_LEVEL})
option(WUZP_INSTALL "Generate the install target." ${PROJECT_IS_TOP_LEVEL})

# Backends: AUTO enables a backend if its dependency is found, ON requires it, OFF disables it.
set(WUZP_WITH_EIGEN AUTO CACHE STRING "Eigen value matrix backend (AUTO, ON, OFF).")
set(WUZP_WITH_BLITZ AUTO CACHE STRING "Blitz++ value matrix backend (AUTO, ON, OFF).")
set(WUZP_WITH_BOOST AUTO CACHE STRING "Boost.Units units backend (AUTO, ON, OFF).")
foreach(_backend EIGEN BLITZ BOOST)
    set_property(CACHE WUZP_WITH_${_backend} PROPERTY STRINGS AUTO ON OFF)
endforeach()

# Oldest supported dependency versions, see the versioning policy in CONTRIBUTING.md.
set(WUZP_EIGEN_MIN_VERSION 3.4)
set(WUZP_BOOST_MIN_VERSION 1.74)

# ----------------------------------------------------------------------------------------------------------------------
# Targets
# ----------------------------------------------------------------------------------------------------------------------

# Core library, including wuzp's own units implementation.
add_library(wuzp INTERFACE)
add_library(wuzp::wuzp ALIAS wuzp)
target_include_directories(wuzp INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/wuzp/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(wuzp INTERFACE cxx_std_20)

# Backends are optional components, e.g. wuzp::eigen.
set(WUZP_COMPONENTS)

# Adds the backend target wuzp::<name>, whose headers live in wuzp_<name>/include.
function(wuzp_add_backend name dependency)
    add_library(wuzp_${name} INTERFACE)
    add_library(wuzp::${name} ALIAS wuzp_${name})
    set_target_properties(wuzp_${name} PROPERTIES EXPORT_NAME ${name})
    target_include_directories(wuzp_${name} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/wuzp_${name}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(wuzp_${name} INTERFACE wuzp ${dependency})
    set(WUZP_COMPONENTS ${WUZP_COMPONENTS} ${name} PARENT_SCOPE)
endfunction()

# Sets <out_var> to TRUE if the backend should be searched for and <mode_var> to the find_package mode.
function(wuzp_backend_mode backend out_var mode_var)
    string(TOUPPER "${WUZP_WITH_${backend}}" _value)
    if(_value STREQUAL "AUTO")
        set(${out_var} TRUE PARENT_SCOPE)
        set(${mode_var} QUIET PARENT_SCOPE)
    elseif(_value STREQUAL "ON")
        set(${out_var} TRUE PARENT_SCOPE)
        set(${mode_var} REQUIRED PARENT_SCOPE)
    elseif(_value STREQUAL "OFF")
        set(${out_var} FALSE PARENT_SCOPE)
    else()
        message(FATAL_ERROR "WUZP_WITH_${backend} must be AUTO, ON or OFF, but is '${WUZP_WITH_${backend}}'.")
    endif()
endfunction()

wuzp_backend_mode(EIGEN _enabled _mode)
if(_enabled)
    # The version is checked after the search, because no version request accepts both Eigen 3.4 and Eigen 5: Eigen 3's
    # package version file rejects ranges beyond major version 3, and Eigen 5's rejects other major versions.
    find_package(Eigen3 ${_mode} NO_MODULE)
    if(Eigen3_FOUND AND Eigen3_VERSION VERSION_LESS WUZP_EIGEN_MIN_VERSION)
        if(_mode STREQUAL "REQUIRED")
            message(FATAL_ERROR "wuzp requires Eigen ${WUZP_EIGEN_MIN_VERSION} or newer, but found ${Eigen3_VERSION}.")
        endif()
    elseif(TARGET Eigen3::Eigen)
        wuzp_add_backend(eigen Eigen3::Eigen)
    endif()
endif()

wuzp_backend_mode(BLITZ _enabled _mode)
if(_enabled)
    find_package(Blitz ${_mode})
    if(TARGET Blitz::Blitz)
        wuzp_add_backend(blitz Blitz::Blitz)
    endif()
endif()

wuzp_backend_mode(BOOST _enabled _mode)
if(_enabled)
    find_package(Boost ${WUZP_BOOST_MIN_VERSION} ${_mode} CONFIG)
    if(TARGET Boost::headers)
        wuzp_add_backend(boost Boost::headers)
    endif()
endif()

message(STATUS "wuzp ${PROJECT_VERSION}, enabled backends: ${WUZP_COMPONENTS}")

# ----------------------------------------------------------------------------------------------------------------------
# Tests, examples, developer tooling
# ----------------------------------------------------------------------------------------------------------------------

if(WUZP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(WUZP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(PROJECT_IS_TOP_LEVEL)
    file(GLOB_RECURSE WUZP_ALL_CXX_FILES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/examples/*.cpp
        ${PROJECT_SOURCE_DIR}/wuzp/include/*.hpp
        ${PROJECT_SOURCE_DIR}/wuzp_blitz/include/*.hpp
        ${PROJECT_SOURCE_DIR}/wuzp_boost/include/*.hpp
        ${PROJECT_SOURCE_DIR}/wuzp_eigen/include/*.hpp
        ${PROJECT_SOURCE_DIR}/tests/src/*.cpp
        ${PROJECT_SOURCE_DIR}/tests/src/*.hpp
        ${PROJECT_SOURCE_DIR}/tests/compile_tests/cases/*.cpp
    )
    find_program(WUZP_CLANG_FORMAT clang-format)
    if(WUZP_CLANG_FORMAT)
        add_custom_target(wuzp_format
            COMMAND ${WUZP_CLANG_FORMAT} -i --style=file ${WUZP_ALL_CXX_FILES}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Formatting the code."
        )
        add_custom_target(wuzp_check_format
            COMMAND ${WUZP_CLANG_FORMAT} --dry-run --Werror --style=file ${WUZP_ALL_CXX_FILES}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Checking the code format."
        )
    endif()
endif()

# ----------------------------------------------------------------------------------------------------------------------
# Installation
# ----------------------------------------------------------------------------------------------------------------------

if(WUZP_INSTALL)
    include(CMakePackageConfigHelpers)
    # The library is header-only, so the package is architecture independent.
    set(WUZP_INSTALL_CMAKEDIR ${CMAKE_INSTALL_DATADIR}/cmake/wuzp)

    install(TARGETS wuzp EXPORT wuzpTargets)
    install(DIRECTORY wuzp/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT wuzpTargets NAMESPACE wuzp:: DESTINATION ${WUZP_INSTALL_CMAKEDIR})

    foreach(_component IN LISTS WUZP_COMPONENTS)
        install(TARGETS wuzp_${_component} EXPORT wuzp_${_component}Targets)
        install(DIRECTORY wuzp_${_component}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
        install(EXPORT wuzp_${_component}Targets NAMESPACE wuzp:: DESTINATION ${WUZP_INSTALL_CMAKEDIR})
    endforeach()

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/wuzpConfig.cmake.in
        ${PROJECT_BINARY_DIR}/wuzpConfig.cmake
        INSTALL_DESTINATION ${WUZP_INSTALL_CMAKEDIR}
    )
    # Semantic versioning: versions with the same major version are compatible. Before 1.0, minor versions may break.
    if(PROJECT_VERSION_MAJOR EQUAL 0)
        set(_compatibility SameMinorVersion)
    else()
        set(_compatibility SameMajorVersion)
    endif()
    write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/wuzpConfigVersion.cmake
        COMPATIBILITY ${_compatibility}
        ARCH_INDEPENDENT
    )
    install(
        FILES
            ${PROJECT_BINARY_DIR}/wuzpConfig.cmake
            ${PROJECT_BINARY_DIR}/wuzpConfigVersion.cmake
            ${PROJECT_SOURCE_DIR}/cmake/FindBlitz.cmake
        DESTINATION ${WUZP_INSTALL_CMAKEDIR}
    )
endif()
