#include "Blackout3D.h"

Blackout3D::Blackout3D() : CubeApplication(20, "cube10.local"){

}

bool Blackout3D::loop() {
    clear();
    render();
    return true;
}