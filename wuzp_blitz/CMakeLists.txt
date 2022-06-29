cmake_minimum_required(VERSION 3.12)
if (DEFINED PACKAGE_VERSION)
    project(wuzp_blitz
        VERSION ${PACKAGE_VERSION}
    )
else()
    project(wuzp_blitz)
endif()

set(CMAKE_CXX_STANDARD 20)

include(CMakeFindDependencyMacro)
find_dependency(blitz REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME} INTERFACE blitz)

# The installation of this package is only supported when the version number is specified.
if (DEFINED PACKAGE_VERSION)
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        COMPATIBILITY SameMinorVersion
    )
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
    )
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime
    )
    install(
        EXPORT
            ${PROJECT_NAME}Targets
        DESTINATION
            lib/cmake/${PROJECT_NAME}
    )
    install(
        FILES
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION
            lib/cmake/${PROJECT_NAME}
    )
    install(
        DIRECTORY
            ${PROJECT_SOURCE_DIR}/include/
        DESTINATION
            include
    )
endif()
