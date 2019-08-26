#pragma once

#include <chrono>
#include <filesystem>
#include <memory>

#include "monitor.h"

using namespace std;
namespace fs = std::filesystem;

class Display {
 public:
  Display(const fs::path& logFile, unsigned long hitsThreshold,
          const chrono::seconds& alertDuration,
          const chrono::seconds& refreshRate);
  virtual ~Display() = default;

  Display(Display&&) = delete;
  Display(const Display&) = delete;
  Display& operator=(Display&&) = delete;
  Display& operator=(const Display&) = delete;

  [[noreturn]] void run();

 private:
  unique_ptr<Monitor> _monitor;
  chrono::system_clock::time_point _startTime;
  chrono::seconds _refreshRate;

  static void clear() noexcept;
  void printAlerts() const noexcept;
  void printHitsTable() const noexcept;
  void printGeneralInfo() const noexcept;

  static constexpr unsigned int HITS_TABLE_ROWS = 12;
};
