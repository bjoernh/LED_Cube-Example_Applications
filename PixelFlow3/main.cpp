#include "pixelflow.h"
#include <chrono>
#include <thread>
#include <csignal>

void signal_handler(int signal_num)
{
    std::cout << "The interrupt signal is (" << signal_num
         << "). \n";

    // It terminates the  program
    //exit(signal_num);
}

int main(int argc, char *argv[]) {
#ifndef _WIN32
    std::signal(SIGUSR2, signal_handler);
#endif

    std::string serverUri = DEFAULTSERVERURI;
    if (argc > 1) {
        serverUri = argv[1];
    }

    PixelFlow App1(serverUri);
    App1.start();

    while(1) std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
