cmake_minimum_required(VERSION 3.20)
project(Genetic CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(genetic main.cpp genetic.cpp)
target_link_libraries(genetic PRIVATE PkgConfig::CUBE)

install(TARGETS genetic DESTINATION bin)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/genetic)