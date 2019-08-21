#include <iostream>

#include "logger.h"

Logger::Logger(const fs::path logFile)
    : _processor(new Processor),
      _reader(new Reader(
          logFile, [this](const string& s) { _processor->process(s); })) {}

Logger::~Logger() {}

void Logger::start() noexcept {
  _reader->start();
  // Not the most elegant solution but it works for now
  while (true)
    ;
}
