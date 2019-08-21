#pragma once

#include <filesystem>
#include <memory>

#include "processor.h"
#include "reader.h"

using namespace std;
namespace fs = std::filesystem;

class Logger {
 public:
  Logger(const fs::path logFile);
  virtual ~Logger();

  Logger(Logger&&) = delete;
  Logger(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;

  void start() noexcept;

 private:
  unique_ptr<Processor> _processor;
  unique_ptr<Reader> _reader;
};
