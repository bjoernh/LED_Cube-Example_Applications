#ifndef MATRIXSERVER_CUBETEST_H
#define MATRIXSERVER_CUBETEST_H

#include <cube/cube.h>

class ImuTest : public cube::CubeApp {
public:
    ImuTest();
    bool loop() override;
private:
    cube::Imu imu_;
};

#endif //MATRIXSERVER_CUBETEST_H
