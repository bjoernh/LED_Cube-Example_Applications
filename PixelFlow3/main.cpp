#include "pixelflow.h"

void signal_handler(int signal_num) 
{ 
    std::cout << "The interrupt signal is (" << signal_num 
         << "). \n"; 
  
    // It terminates the  program 
    //exit(signal_num); 
} 

int main(int argc, char *argv[]) {
    std::signal(SIGUSR2, signal_handler);
    
    std::string serverUri = DEFAULTSERVERURI;
    if (argc > 1) {
        serverUri = argv[1];
    }
    
    PixelFlow App1(serverUri);
    App1.start();

    while(1) sleep(2);
    return 0;
}
