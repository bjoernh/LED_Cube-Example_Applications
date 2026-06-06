cmake_minimum_required(VERSION 3.20)
project(ExampleApplications 
    VERSION 2.0.2
    DESCRIPTION "Example Applications for Cube 2.0"
    LANGUAGES CXX
)	


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Genetic)
#add_subdirectory(CubeTestApp)
add_subdirectory(Breakout3D)
add_subdirectory(PixelFlow3)
add_subdirectory(MatrixRain)
add_subdirectory(Snake)
add_subdirectory(Picture)
add_subdirectory(ImuTest)
add_subdirectory(PixelFlow)
add_subdirectory(PixelFlow2)
add_subdirectory(Rainbow)
#add_subdirectory(PortalCube)

