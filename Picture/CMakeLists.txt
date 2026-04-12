project(Picture)

find_package(matrixapplication 0.4 REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)

set(MAINSRC
    main.cpp picture.cpp picture.h Image.cpp Image.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(Picture ${MAINSRC})

target_link_libraries(Picture ${MAINLIBS} Imlib2) #-l flag

install(TARGETS Picture DESTINATION /home/pi/APPS)