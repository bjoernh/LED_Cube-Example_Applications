project(Snake)

find_package(matrixapplication REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS thread log system)
set(MAINSRC
  snake.cpp
  main.cpp
)

set(MAINLIBS
        matrixapplication::matrixapplication
)

add_executable(Snake ${MAINSRC})
target_link_libraries(Snake ${MAINLIBS})

install(TARGETS Snake DESTINATION /home/pi/APPS)