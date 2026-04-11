project(cubeTestApp)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)
include_directories(${Boost_INCLUDE_DIRS})

if(APPLE)
    find_library(IMLIB2_FOUND "Imlib2" PATHS "/opt/homebrew/lib" "/usr/local/lib")
    IF(IMLIB2_FOUND)
        MESSAGE("\tIMLIB2 Found")
        LINK_DIRECTORIES(/opt/homebrew/lib)
        INCLUDE_DIRECTORIES(/opt/homebrew/include)
    ELSE(IMLIB2_FOUND)
        MESSAGE(FATAL_ERROR "\tIMLIB2 Not Found")
    ENDIF(IMLIB2_FOUND)
endif()



find_package(matrixapplication 0.4 REQUIRED)


add_executable(cubetestapp main.cpp CubeTest.cpp)
target_link_libraries(cubetestapp matrixapplication::matrixapplication)
