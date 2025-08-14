project(MatrixRain)

find_package(matrixapplication REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system program_options)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(MAINSRC
    main.cpp matrixrain.cpp matrixrain.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(MatrixRain ${MAINSRC})
target_link_libraries(MatrixRain ${MAINLIBS} ${Boost_LIBRARIES}) #-l flag

install(TARGETS MatrixRain DESTINATION /home/pi/APPS)