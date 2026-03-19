project(PixelFlow)

find_package(matrixapplication 0.3 REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)

set(MAINSRC
    main.cpp pixelflow.cpp pixelflow.h)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(PixelFlow ${MAINSRC})
target_link_libraries(PixelFlow ${MAINLIBS})

install(TARGETS PixelFlow DESTINATION /home/pi/APPS)