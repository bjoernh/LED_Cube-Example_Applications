#include "matrixrain.h"
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
    std::string serverUri = DEFAULTSERVERURI;
    float fade = 0.9f;

    if (argc > 1) {
        serverUri = argv[1];
    }
    if (argc > 2) {
        try {
            fade = std::stof(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid fade argument '" << argv[2] << "', using default 0.9.\n";
        }
    }

    MatrixRain App1(serverUri, fade);
    App1.start();
    
    while(1) sleep(2);
    
    return 0;
}
