#include "pixelbeat.h"
#include <csignal>
#include <iostream>
#include <unistd.h>

void signal_handler(int signal_num) {
    std::cout << "The interrupt signal is (" << signal_num << "). \n";
}

int main(int argc, char *argv[]) {
    std::signal(SIGUSR2, signal_handler);

    std::string serverUri = DEFAULTSERVERURI;
    if (argc > 1) {
        serverUri = argv[1];
    }

    PixelBeat App1(serverUri);
    App1.start();

    while (true) {
        AppState s = App1.getAppState();
        if (s != AppState::starting && s != AppState::running) break;
        sleep(1);
    }
    return 0;
}
