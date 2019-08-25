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

  unsigned long alertThreshold() const noexcept { return _alertThreshold; }
  chrono::seconds alertDuration() const noexcept { return _alertDuration; }
  unsigned long averageHitsToAlert() const noexcept {
    return _averageHitsToAlert;
  }

 private:
  atomic<bool> _isRunning;

  // Number of hits per second to exceed to trigger alert
  unsigned long _alertThreshold;
  // Alert window to take into consideration to trigger alerts
  chrono::seconds _alertDuration;
  // Number of hits to exceed in the alert window to trigger alert
  unsigned long _averageHitsToAlert;

  list<Alert> _alerts;
  using alertIt = list<Alert>::iterator;
  alertIt _lastActiveAlert;
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
