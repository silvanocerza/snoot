#include <algorithm>
#include <exception>
#include <iostream>

#include "monitor.h"

Monitor::Monitor(const fs::path& file, unsigned long hitsThreshold,
                 const chrono::seconds& alertDuration)
    : _alertThreshold(hitsThreshold),
      _alertDuration(alertDuration),
      _latestActiveAlert(_alerts.end()) {
  switch (fs::status(file).type()) {
    case fs::file_type::fifo:
    case fs::file_type::symlink:
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
}

Monitor::~Monitor() { _file.close(); }

void Monitor::start() { _runThread.reset(new thread(&Monitor::run, this)); }

list<LogItem> Monitor::logData() const noexcept {
  lock_guard{_logDataMutex};
  return _logData;
}

list<Alert> Monitor::alerts() const noexcept {
  lock_guard{_alertsMutex};
  return _alerts;
}

void Monitor::update(const string& line) noexcept {
  lock_guard{_logDataMutex};

  // Removes logs older than a certain amount of seconds
  auto now = chrono::system_clock::now();
  auto timeComparator = [this, now](auto item) -> bool {
    auto elapsed = now - item.dateTime;
    return elapsed > _alertDuration;
  };
  auto it = remove_if(_logData.begin(), _logData.end(), timeComparator);
  _logData.erase(it, _logData.end());

  // Creates new log
  _logData.emplace_back(LogItem::from(line));

  auto hits = _logData.size();

  auto averageHits =
      _alertThreshold * static_cast<unsigned long>(_alertDuration.count());

  lock_guard{_alertsMutex};
  while (_alerts.size() > 5) {
    _alerts.pop_front();
  }

  if (hits > averageHits) {
    if (_latestActiveAlert == _alerts.end()) {
      // New alert
      auto now = chrono::system_clock::now();
      _alerts.emplace_back(hits, now);
      auto it = _alerts.end();
      it--;
      _latestActiveAlert = it;
    }
  } else {
    if (_latestActiveAlert != _alerts.end()) {
      // Recover
      _latestActiveAlert->recoverTime = chrono::system_clock::now();
      _latestActiveAlert = _alerts.end();
    }
  }
}

[[noreturn]] void Monitor::run() noexcept {
  string line;
  // TODO: We might want to find a way to stop this somehow
  while (true) {
    while (getline(_file, line)) {
      if (line.empty()) {
        continue;
      }
      update(line);
    }

    if (not _file.eof()) {
      // TODO: Handle this?
    }

    // Clear eof state so that we can keep reading
    _file.clear();
  }
}
