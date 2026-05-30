#include "ImuTest.h"

#include <cube/cube.h>

ImuTest::ImuTest() : cube::CubeApp("imutest", 30) {}

bool ImuTest::loop() {
    fade(0.85F);
    setPixel3D(imu_.cubeAccIntersect(), cube::Color::green());
    return true;
}
