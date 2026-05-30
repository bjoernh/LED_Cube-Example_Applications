cmake_minimum_required(VERSION 3.20)
project(Picture CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)
pkg_check_modules(IMLIB2 REQUIRED IMPORTED_TARGET imlib2)

add_executable(picture main.cpp picture.cpp Image.cpp)
target_link_libraries(picture PRIVATE PkgConfig::CUBE PkgConfig::IMLIB2)

install(TARGETS picture DESTINATION libexec/cube/apps)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/picture)