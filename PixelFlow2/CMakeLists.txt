cmake_minimum_required(VERSION 3.20)
project(PixelFlow2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(pixelflow2 main.cpp pixelflow2.cpp)
target_link_libraries(pixelflow2 PRIVATE PkgConfig::CUBE)

install(TARGETS pixelflow2 DESTINATION bin)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/pixelflow2)