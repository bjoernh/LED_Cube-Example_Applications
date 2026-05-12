project(PortalCube)

find_package(matrixapplication 0.4 REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log)
set(MAINSRC
  PortalCube.cpp
  main.cpp
)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(PortalCube ${MAINSRC})
target_link_libraries(PortalCube ${MAINLIBS})

install(TARGETS PortalCube DESTINATION /home/pi/APPS)
