#include <iostream>

#include "logger.h"

Logger::Logger(const fs::path logFile)
    : _reader(logFile, [](string s) { cout << s << endl; }) {}

Logger::~Logger() {}

void Logger::start() noexcept {
  _reader.start();
  // Not the most elegant solution but it works for now
  while (true)
    ;
}
