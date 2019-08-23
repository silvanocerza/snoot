#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include "log_item.h"

using namespace std;
namespace fs = std::filesystem;

struct Alert {
  Alert(unsigned long hits, chrono::system_clock::time_point triggerTime)
      : hits(hits), triggerTime(triggerTime) {}

  bool hasRecovered() const {
    return recoverTime != chrono::system_clock::time_point();
  }

  unsigned long hits;
  chrono::system_clock::time_point triggerTime;
  chrono::system_clock::time_point recoverTime;
};

class Monitor {
 public:
  Monitor(const fs::path& file, unsigned long hitsThreshold,
          const chrono::seconds& alertDuration);
  virtual ~Monitor();

  Monitor(Monitor&&) = delete;
  Monitor(const Monitor&) = delete;
  Monitor& operator=(Monitor&&) = delete;
  Monitor& operator=(const Monitor&) = delete;

  void start();
  void stop();

  list<LogItem> logs() const noexcept;
  list<Alert> alerts() const noexcept;

 private:
  atomic<bool> _isRunning;

  unsigned long _alertThreshold;
  chrono::seconds _alertDuration;

  list<Alert> _alerts;
  using alertIt = list<Alert>::iterator;
  alertIt _latestActiveAlert;
  mutable mutex _alertsMutex;

  unique_ptr<thread> _runThread;
  ifstream _file;
  list<LogItem> _logs;
  mutable mutex _logsMutex;

  void run() noexcept;

  void eraseOldLogs(const chrono::seconds& threshold) noexcept;
  void updateLogs(const string& line) noexcept;
  void updateAlert() noexcept;
};
