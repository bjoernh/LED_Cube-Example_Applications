#include "pixelflow.h"

int main(int argc, char *argv[]) {
    PixelFlow App1;
    App1.start();

    // Exit on pause/kill/end so resources (audio device, joysticks) are
    // released when MainMenu spawns a new instance of this app.
    while (true) {
        AppState s = App1.getAppState();
        if (s != AppState::starting && s != AppState::running) break;
        sleep(1);
    }
    return 0;
}
