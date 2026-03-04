project(ImuTest)

find_package(matrixapplication 0.3 REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)

add_executable(imutestapp main.cpp ImuTest.cpp)
target_link_libraries(imutestapp matrixapplication::matrixapplication)