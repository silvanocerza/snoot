#pragma once

#include <filesystem>
#include <fstream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include "log_item.h"

using namespace std;
namespace fs = std::filesystem;

class Monitor {
 public:
  Monitor(const fs::path& file);
  virtual ~Monitor();

  Monitor(Monitor&&) = delete;
  Monitor(const Monitor&) = delete;
  Monitor& operator=(Monitor&&) = delete;
  Monitor& operator=(const Monitor&) = delete;

  void start();

  list<LogItem> logData() const noexcept;

 private:
  unique_ptr<thread> _runThread;
  ifstream _file;
  list<LogItem> _logData;
  mutable mutex _logDataMutex;

  [[noreturn]] void run() noexcept;
  void update(const string& line) noexcept;
};
