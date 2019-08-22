#pragma once

#include <filesystem>
#include <memory>

#include "monitor.h"

using namespace std;
namespace fs = std::filesystem;

class Display {
 public:
  Display(const fs::path& logFile);
  virtual ~Display() = default;

  Display(Display&&) = delete;
  Display(const Display&) = delete;
  Display& operator=(Display&&) = delete;
  Display& operator=(const Display&) = delete;

  [[noreturn]] void run();

 private:
  unique_ptr<Monitor> _monitor;

  static void clear() noexcept;
  void printHitsTable(list<LogItem> logs) const noexcept;

  static constexpr unsigned int HITS_TABLE_ROWS = 12;
};
