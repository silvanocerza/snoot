#include <iostream>

#include "logger.h"

Logger::Logger(const fs::path logFile) : _monitor(new Monitor(logFile)) {}

Logger::~Logger() {}

void Logger::start() noexcept {
  _monitor->start();
  // Not the most elegant solution but it works for now
  while (true)
    ;
}
