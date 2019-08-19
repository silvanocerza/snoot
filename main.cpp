#include <filesystem>
#include <iostream>

#include "logger.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, const char* argv[]) {
  // TODO: Takes last argument now, but it must be changed.
  // Also handle empty file name.
  Logger logger(argv[argc - 1]);
  logger.start();
  return 0;
}
