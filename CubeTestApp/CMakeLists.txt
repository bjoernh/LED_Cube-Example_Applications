project(cubeTestApp)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)
include_directories(${Boost_INCLUDE_DIRS})

find_library(IMLIB2_FOUND "Imlib2" PATHS "/opt/homebrew/Cellar/imlib2/1.12.2/")
 IF(IMLIB2_FOUND)
 	MESSAGE("\tIMLIB2 Found")
  LINK_DIRECTORIES(/opt/homebrew/Cellar/imlib2/1.12.2/lib)
  INCLUDE_DIRECTORIES(/opt/homebrew/Cellar/imlib2/1.12.2/inlude)
 ELSE(IMLIB2_FOUND)
 	MESSAGE(FATAL_ERROR "\tIMLIB2 Not Found")
 ENDIF(IMLIB2_FOUND)


find_package( absl REQUIRED )
find_package(matrixapplication REQUIRED)


add_executable(cubetestapp main.cpp CubeTest.cpp)
target_link_libraries(cubetestapp matrixapplication::matrixapplication absl::log_internal_message)
