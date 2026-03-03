#ifndef MATRIXSERVER_CUBETEST_H
#define MATRIXSERVER_CUBETEST_H

#include <CubeApplication.h>

class CubeTest : public CubeApplication{
public:
    CubeTest(const char* host = "127.0.0.1");
    bool loop();
};

#endif //MATRIXSERVER_CUBETEST_H
