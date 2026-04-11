#include "genetic.h"
#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
  std::string serverUri = DEFAULTSERVERURI;
  if (argc > 1) {
      serverUri = argv[1];
  }
  
  Genetic App1(serverUri);
  App1.start();

  while(1) std::this_thread::sleep_for(std::chrono::seconds(1));
  return 0;
}
