#include "genetic.h"

int main(int argc, char *argv[]) {
  std::string serverUri = DEFAULTSERVERURI;
  if (argc > 1) {
      serverUri = argv[1];
  }
  
  Genetic App1(serverUri);
  App1.start();

  while(1) sleep(1);
  return 0;
}
