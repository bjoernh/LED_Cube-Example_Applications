#include "rainbow.h"

int main(int argc, char *argv[]) {
    Rainbow App1;
    App1.start();

    while (true) {
        AppState s = App1.getAppState();
        if (s != AppState::starting && s != AppState::running) break;
        sleep(1);
    }
    return 0;
}
