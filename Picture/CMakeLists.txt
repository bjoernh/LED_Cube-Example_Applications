project(Picture)

find_package(matrixapplication REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)

set(MAINSRC
    main.cpp picture.cpp picture.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(Picture ${MAINSRC})

target_link_libraries(Picture ${MAINLIBS} ) #-l flag

install(TARGETS Picture DESTINATION /home/pi/APPS)