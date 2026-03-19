project(PixelFlow2)

find_package(matrixapplication 0.3 REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)

set(MAINSRC
    main.cpp pixelflow2.cpp pixelflow2.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(PixelFlow2 ${MAINSRC})
target_link_libraries(PixelFlow2 ${MAINLIBS})

install(TARGETS PixelFlow2 DESTINATION /home/pi/APPS)