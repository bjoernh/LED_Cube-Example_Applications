#include "PortalCube.h"
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
    std::string serverUri = DEFAULTSERVERURI;
    if (argc > 1)
        serverUri = argv[1];

    PortalCube app(serverUri);
    app.start();

    while (1) sleep(2);
    return 0;
}
