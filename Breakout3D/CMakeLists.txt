cmake_minimum_required(VERSION 3.20)
project(Breakout3D CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(breakout3d main.cpp breakoutgame.cpp)
target_link_libraries(breakout3d PRIVATE PkgConfig::CUBE)

install(TARGETS breakout3d DESTINATION bin)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/breakout3d)