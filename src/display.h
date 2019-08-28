#pragma once

#include <chrono>
#include <filesystem>
#include <memory>

#include "monitor.h"

using namespace std;
namespace fs = std::filesystem;

// Display has only one task: display the information from
// its internal Monitor. Nothing else.
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
  // Monitor where data is retrieved from
  unique_ptr<Monitor> _monitor;
  // When run() has been called
  chrono::system_clock::time_point _startTime;
  // Number of seconds to wait before refreshing information displayed
  chrono::seconds _refreshRate;

  static void clear() noexcept;
  void printAlerts() const noexcept;
  void printHitsTable() const noexcept;
  void printGeneralInfo() const noexcept;

  static constexpr unsigned int HITS_TABLE_ROWS = 5;
};
