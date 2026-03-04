project(ImuTest)

find_package(matrixapplication 0.3 REQUIRED)

add_executable(imutestapp main.cpp ImuTest.cpp)
target_link_libraries(imutestapp matrixapplication::matrixapplication)