project(PixelFlow3)

find_package(matrixapplication REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)

find_library(IMLIB2_FOUND "Imlib2" PATHS "/opt/homebrew/Cellar/imlib2/1.12.2/")
 IF(IMLIB2_FOUND)
 	MESSAGE("\tIMLIB2 Found")
  LINK_DIRECTORIES(/opt/homebrew/Cellar/imlib2/1.12.2/lib)
  INCLUDE_DIRECTORIES(/opt/homebrew/Cellar/imlib2/1.12.2/inlude)
 ELSE(IMLIB2_FOUND)
 	MESSAGE(FATAL_ERROR "\tIMLIB2 Not Found")
 ENDIF(IMLIB2_FOUND)


find_package( absl REQUIRED )

set(MAINSRC
    main.cpp pixelflow.cpp pixelflow.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(PixelFlow3 ${MAINSRC})
target_link_libraries(PixelFlow3 ${MAINLIBS}) #-l flag

install(TARGETS PixelFlow3 DESTINATION /home/pi/APPS)