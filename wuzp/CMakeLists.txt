cmake_minimum_required(VERSION 3.12)
project(wuzp)

set(CMAKE_CXX_STANDARD 20)

add_library(wuzp INTERFACE)
target_include_directories(wuzp INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/wuzpConfigVersion.cmake"
    VERSION 0.1
    COMPATIBILITY AnyNewerVersion
)


install(TARGETS wuzp
    EXPORT wuzpTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/wuzpConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/wuzpConfig.cmake"
    INSTALL_DESTINATION lib/cmake/wuzp
)

install(EXPORT wuzpTargets DESTINATION lib/cmake/wuzp)
install(FILES "${PROJECT_BINARY_DIR}/wuzpConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/wuzpConfig.cmake"
    DESTINATION lib/cmake/wuzp)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
