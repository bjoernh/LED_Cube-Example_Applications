#ifndef MATRIXSERVER_CUBETEST_H
#define MATRIXSERVER_CUBETEST_H

#include <CubeApplication.h>
#include <Mpu6050.h>

class Blackout3D : public CubeApplication{
public:
    Blackout3D(const char* host = "127.0.0.1");
    bool loop();
};

#endif //MATRIXSERVER_CUBETEST_H
