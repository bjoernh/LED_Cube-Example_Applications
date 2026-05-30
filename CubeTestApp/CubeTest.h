#ifndef MATRIXSERVER_CUBETEST_H
#define MATRIXSERVER_CUBETEST_H

#include <cube/cube.h>

class CubeTest : public cube::CubeApp {
public:
    CubeTest();
    bool loop() override;
};

#endif //MATRIXSERVER_CUBETEST_H
