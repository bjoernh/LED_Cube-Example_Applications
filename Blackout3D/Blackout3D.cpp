#include "Blackout3D.h"

Blackout3D::Blackout3D(std::string serverUri) : CubeApplication(20, serverUri){

}

bool Blackout3D::loop() {
    clear();
    render();
    return true;
}