cmake_minimum_required(VERSION 3.20)
project(CubeTestApp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(cubetestapp main.cpp CubeTest.cpp)
target_link_libraries(cubetestapp PRIVATE PkgConfig::CUBE)

install(TARGETS cubetestapp DESTINATION bin)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/cubetestapp)
