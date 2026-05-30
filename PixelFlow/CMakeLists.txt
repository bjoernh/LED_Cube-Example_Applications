cmake_minimum_required(VERSION 3.20)
project(PixelFlow CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(pixelflow main.cpp pixelflow.cpp)
target_link_libraries(pixelflow PRIVATE PkgConfig::CUBE)

install(TARGETS pixelflow DESTINATION libexec/cube/apps)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/pixelflow)