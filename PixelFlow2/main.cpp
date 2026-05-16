#include "pixelflow2.h"

int main(int argc, char *argv[]) {
    PixelFlow2 App1;
    App1.start();

    while (true) {
        AppState s = App1.getAppState();
        if (s != AppState::starting && s != AppState::running) break;
        sleep(1);
    }
    return 0;
}
