#include "Blackout3D.h"

Blackout3D::Blackout3D(const char* host) : CubeApplication(20, host){

}

bool Blackout3D::loop() {
    clear();
    render();
    return true;
}