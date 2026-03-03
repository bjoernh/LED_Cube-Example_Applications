#include "breakoutgame.h"

int main(int argc, char *argv[]) {
  const char* host = "127.0.0.1";
  if (argc > 1) {
      host = argv[1];
  }
  
  BreakoutGame App1(host);
  App1.start();

  while(1) sleep(1);
  return 0;
}
