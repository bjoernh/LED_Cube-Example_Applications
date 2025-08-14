#include "CubeTest.h"

void signal_handler(int signal_num) 
{ 
    std::cout << "The interrupt signal is (" << signal_num 
         << "). \n"; 
  
    // It terminates the  program 
    exit(signal_num); 
} 

int main(int argc, char *argv[]) {
    if (argc == 2) { 
        std::string ip = argv[1];
        std::string port = "2017";
        std::cout << "arg1: " << ip << std::endl;
        //CubeTest App1(30, ip, port);
        CubeTest App1;
        App1.start();
    }
    else {
        //CubeTest App1(30, "127.0.0.1", "2017");
        //App1.start();
    }
    std::signal(SIGUSR1, signal_handler);
   

    while(1) sleep(1);
    return 0;
}
