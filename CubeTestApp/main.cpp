#include "CubeTest.h"

void signal_handler(int signal_num) 
{ 
    std::cout << "The interrupt signal is (" << signal_num 
         << "). \n"; 
  
    // It terminates the  program 
    exit(signal_num); 
} 

int main(int argc, char *argv[]) {
    const char* host = "127.0.0.1";
    if (argc > 1) {
        host = argv[1];
    }
    
    CubeTest App1(host);
    App1.start();
    
    std::signal(SIGUSR1, signal_handler);
   

    while(1) sleep(1);
    return 0;
}
