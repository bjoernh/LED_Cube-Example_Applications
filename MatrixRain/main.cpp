#include "matrixrain.h"
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
    std::string host = "127.0.0.1";
    float fade = 0.9f;

    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        try {
            fade = std::stof(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid fade argument '" << argv[2] << "', using default 0.9.\n";
        }
    }

    PixelFlow App1(host, fade);
    App1.start();
    
    while(1) sleep(2);
    
    return 0;
}
