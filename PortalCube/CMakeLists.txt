cmake_minimum_required(VERSION 3.20)
project(PortalCube CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(portalcube main.cpp PortalCube.cpp)
target_link_libraries(portalcube PRIVATE PkgConfig::CUBE)

install(TARGETS portalcube DESTINATION bin)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/portalcube)
