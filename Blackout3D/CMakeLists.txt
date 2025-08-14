project(Blackout3D)

find_package(matrixapplication REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)

add_executable(Blackout3D main.cpp Blackout3D.cpp)
target_link_libraries(Blackout3D matrixapplication::matrixapplication)