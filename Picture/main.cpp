#include "picture.h"

#include <cube/cube.h>

int main(int argc, char *argv[]) {
    Picture app(argc, argv);
    return cube::run(app);
}
