# Ported to libcube (Cube 2.0). Build standalone:
#   cmake -S . -B build -DCMAKE_PREFIX_PATH=<libcube-staging-prefix>
#   cmake --build build
# (libcube is found via pkg-config; PKG_CONFIG_PATH must include its lib/pkgconfig.)
cmake_minimum_required(VERSION 3.20)
project(MatrixRain CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(matrixrain main.cpp matrixrain.cpp)
target_link_libraries(matrixrain PRIVATE PkgConfig::CUBE)

install(TARGETS matrixrain DESTINATION bin)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/matrixrain)
