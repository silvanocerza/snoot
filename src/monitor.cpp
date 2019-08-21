#include <algorithm>
#include <exception>
#include <iostream>

#include "monitor.h"

Monitor::Monitor(const fs::path& file) {
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

void Monitor::update(const string& line) noexcept {
  lock_guard{_logDataMutex};

  auto now = chrono::system_clock::now();
  // Returns iterator to first item "younger" than 10 seconds
  auto it =
      find_if(_logData.cbegin(), _logData.cend(), [now](auto item) -> bool {
        auto elapsed = now - item.dateTime;
        return elapsed < 10s;
      });

  if (it != _logData.cend()) {
    // We want to delete only items oldest than 10 seconds,
    // so we decrement by one, otherwise we would also delete
    // the first of the "young" logs
    it--;
    _logData.erase(_logData.cbegin(), it);
  }

  _logData.emplace_back(LogItem::from(line));
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
