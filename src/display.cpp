#include <chrono>
#include <iostream>
#include <thread>

#include <date.h>

#include "display.h"

using namespace chrono;

Display::Display(const fs::path& logFile) : _monitor(new Monitor(logFile)) {}

[[noreturn]] void Display::run() {
  _monitor->start();

  while (true) {
    Display::clear();

    auto data = _monitor->logData();

    cout << "Requests count in last 10 seconds: " << data.size() << '\n';

    // Shows current date and time.
    auto now = time_point_cast<seconds>(system_clock::now());
    cout << date::format("%F %T", now) << '\n';

    this_thread::sleep_for(1s);
  }
}

void Display::clear() noexcept {
  // It's obviously not the most elegant solution to clear the terminal but it
  // does the job.
  cout << string(500, '\n') << endl;
}
