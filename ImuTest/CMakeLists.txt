cmake_minimum_required(VERSION 3.20)
project(ImuTest CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CUBE REQUIRED IMPORTED_TARGET cube)

add_executable(imutest main.cpp ImuTest.cpp)
target_link_libraries(imutest PRIVATE PkgConfig::CUBE)

install(TARGETS imutest DESTINATION libexec/cube/apps)
install(FILES schema.toml manifest.toml DESTINATION share/cube/apps/imutest)