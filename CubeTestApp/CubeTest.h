#ifndef MATRIXSERVER_CUBETEST_H
#define MATRIXSERVER_CUBETEST_H

#include <CubeApplication.h>

class CubeTest : public CubeApplication{
public:
    CubeTest(std::string serverUri = DEFAULTSERVERURI);
    bool loop();
};

#endif //MATRIXSERVER_CUBETEST_H
