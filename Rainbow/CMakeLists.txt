project(Rainbow)

find_package(matrixapplication REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)
set(MAINSRC
    main.cpp rainbow.cpp rainbow.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(Rainbow ${MAINSRC})
target_link_libraries(Rainbow ${MAINLIBS}) #-l flag

install(TARGETS Rainbow DESTINATION /home/pi/APPS)