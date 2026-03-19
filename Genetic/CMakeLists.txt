project(Genetic)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)

find_package(matrixapplication 0.3 REQUIRED)

add_executable(Genetic main.cpp genetic.cpp)
target_link_libraries(Genetic matrixapplication::matrixapplication)

install(TARGETS Genetic DESTINATION /home/pi/APPS)