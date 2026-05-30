# Ported to libcube (Cube 2.0). Build standalone:
#   PKG_CONFIG_PATH=<libcube-prefix>/lib/pkgconfig cmake -S . -B build
#   cmake --build build
cmake_minimum_required(VERSION 3.20)
project(Snake CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(snake main.cpp snake.cpp)
target_link_libraries(snake PRIVATE PkgConfig::CUBE)

install(TARGETS snake DESTINATION libexec/cube/apps)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/snake)
