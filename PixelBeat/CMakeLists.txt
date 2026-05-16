project(PixelBeat)

find_package(matrixapplication 0.4 REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)

set(MAINSRC
    main.cpp pixelbeat.cpp pixelbeat.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(PixelBeat ${MAINSRC})
target_link_libraries(PixelBeat ${MAINLIBS})

install(TARGETS PixelBeat DESTINATION /home/pi/APPS)
