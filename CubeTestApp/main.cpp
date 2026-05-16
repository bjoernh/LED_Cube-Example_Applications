#include "CubeTest.h"

void signal_handler(int signal_num) 
{ 
    std::cout << "The interrupt signal is (" << signal_num 
         << "). \n"; 
  
    // It terminates the  program 
    exit(signal_num); 
} 

int main(int argc, char *argv[]) {
    std::string serverUri = DEFAULTSERVERURI;
    if (argc > 1) {
        serverUri = argv[1];
    }
    
    CubeTest App1(serverUri);
    App1.start();

    std::signal(SIGUSR1, signal_handler);

    // Exit when the framework pauses, kills, or ends us. MainMenu spawns a
    // fresh process when the user picks the app again — staying alive would
    // orphan the old process and leak the audio device to the new instance.
    while (true) {
        AppState s = App1.getAppState();
        if (s != AppState::starting && s != AppState::running) break;
        sleep(1);
    }
    return 0;
}
