#include <algorithm>
#include <cstring>
#include <exception>
#include <iostream>

#include "monitor.h"

Monitor::Monitor(const fs::path& file, unsigned long hitsThreshold,
                 const chrono::seconds& alertDuration)
    : _totalHits(0),
      _totalTraffic(0),
      _alertThreshold(hitsThreshold),
      _alertDuration(alertDuration),
      _lastActiveAlert(_alerts.end()) {
  if (_alertThreshold == 0) {
    throw runtime_error(
        "Hits per second to trigger an alert must not be zero.");
  }
  if (_alertDuration == 0s) {
    throw runtime_error("An alert duration can't be zero.");
  }

  switch (fs::status(file).type()) {
    case fs::file_type::regular: {
      // We got a usable file, just keep going
      break;
    };
    case fs::file_type::none: {
      // It could be anything ¯\_(ツ)_/¯
      throw runtime_error("Something really bad has happened!");
    };
    case fs::file_type::not_found: {
      throw runtime_error("File not found.");
    };
    case fs::file_type::fifo:
    case fs::file_type::symlink:
    case fs::file_type::block:
    case fs::file_type::directory:
    case fs::file_type::socket:
    case fs::file_type::character: {
      throw runtime_error(
          "Can't read from provided path, please use a regular file.");
    };
    case fs::file_type::unknown: {
      throw runtime_error("Unknown file type");
    };
  }

  // Seek last line on open, we don't really care about previous logs
  _file.open(file, ios::ate | ios::in);

  if (not _file.is_open()) {
    throw system_error(errno, system_category(),
                       "Error opening file " + file.string());
  }

  _averageHitsToAlert =
      _alertThreshold * static_cast<unsigned long>(_alertDuration.count());
}

Monitor::~Monitor() {
  stop();
  _file.close();
}

void Monitor::start() {
  _isRunning = true;
  _runThread.reset(new thread(&Monitor::run, this));
}

void Monitor::stop() {
  _isRunning = false;
  // Wait for thread to stop
  while (not _runThread->joinable())
    ;
  _runThread->join();
}

list<LogItem> Monitor::logs() const noexcept {
  lock_guard{_logsMutex};
  return _logs;
}

list<Alert> Monitor::alerts() const noexcept {
  lock_guard{_alertsMutex};
  return _alerts;
}

// Removes logs older than a certain threshold
void Monitor::eraseOldLogs(const chrono::seconds& threshold) noexcept {
  auto now = chrono::system_clock::now();
  auto timeComparator = [threshold, now](auto item) -> bool {
    auto elapsed = now - item.dateTime();
    return elapsed > threshold;
  };

  lock_guard{_logsMutex};
  _logs.remove_if(timeComparator);
}

// Parses passed line and creates new LogItem if line is parsed correctly
void Monitor::updateLogs(const string& line) noexcept {
  lock_guard{_logsMutex};

  // Creates new log
  auto log = LogItem::from(line);
  if (log.isValid()) {
    _logs.emplace_back(log);
    _totalHits++;
    _totalTraffic += static_cast<unsigned long>(log.size());
  }
}

// Triggers or recovers Alerts if necessary
void Monitor::updateAlert() noexcept {
  lock_guard{_alertsMutex};

  auto hits = _logs.size();

  lock_guard{_alertsMutex};
  if (hits > _averageHitsToAlert) {
    if (_lastActiveAlert == _alerts.end()) {
      // New alert
      auto now = chrono::system_clock::now();
      _alerts.emplace_back(hits, now);
      auto it = _alerts.end();
      it--;
      _lastActiveAlert = it;
    }
  } else {
    if (_lastActiveAlert != _alerts.end()) {
      // Recover
      _lastActiveAlert->recoverTime = chrono::system_clock::now();
      _lastActiveAlert = _alerts.end();
    }
  }
}

// Runs concurrently in a separate thread to monitor continuosly the specified
// log file.
void Monitor::run() {
  string line;
  while (_isRunning) {
    eraseOldLogs(_alertDuration);

    if (getline(_file, line)) {
      updateLogs(line);
    }

    updateAlert();

    if (not _file.eof() && not _file.good()) {
      string errorString(strerror(errno));
      throw runtime_error("Error while monitoring log file: " + errorString);
    }

    // Clear eof state so that we can keep reading
    _file.clear();
  }
}
