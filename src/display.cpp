#include <iomanip>
#include <iostream>
#include <set>
#include <thread>
#include <unordered_map>
#include <utility>

#include <date.h>

#include "display.h"

using namespace chrono;

Display::Display(const fs::path& logFile, unsigned long hitsThreshold,
                 const seconds& alertDuration, unsigned int alertHistory,
                 const seconds& refreshRate)
    : _monitor(
          new Monitor(logFile, hitsThreshold, alertDuration, alertHistory)),
      _refreshRate(refreshRate) {}

[[noreturn]] void Display::run() {
  _monitor->start();
  _startTime = time_point_cast<seconds>(system_clock::now());

  while (true) {
    Display::clear();

    printHitsTable();
    printAlerts();
    printGeneralInfo();

    this_thread::sleep_for(_refreshRate);
  }
}

void Display::clear() noexcept {
  // It's obviously not the most elegant solution to clear the terminal but it
  // does the job.
  cout << string(10, '\n') << endl;
}

// Prints all alerts
void Display::printAlerts() const noexcept {
  for (const auto& a : _monitor->alerts()) {
    cout << "High traffic generated an alert - hits = ";
    cout << a.hits;
    cout << ", triggered at ";
    cout << date::format("%F %T", time_point_cast<seconds>(a.triggerTime));
    cout << '\n';

    if (a.hasRecovered()) {
      cout << "Previous alert recovered at ";
      cout << date::format("%F %T", time_point_cast<seconds>(a.recoverTime));
      cout << "\n";
    }
  }
}

// Prints each resources and the number of hits it received
void Display::printHitsTable() const noexcept {
  // First count resources hits
  unordered_map<string, int> resourcesHits;
  auto logs = _monitor->logs();
  auto totalHits = logs.size();
  for (const auto& item : logs) {
    if (resourcesHits.count(item.resource()) == 0) {
      resourcesHits.emplace(item.resource(), 1);
    } else {
      resourcesHits[item.resource()]++;
    }
  }

  // Sort resources by hits
  using PairValueComparator = bool (*)(pair<string, int>, pair<string, int>);
  auto hitsComparator = [](pair<string, int> a, pair<string, int> b) -> bool {
    return a.second > b.second;
  };
  set<pair<string, int>, PairValueComparator> sortedHits(
      resourcesHits.cbegin(), resourcesHits.cend(), hitsComparator);

  // Table header
  cout << "================================\n";
  cout << "Most visited during last period \n";
  cout << "--------------------------------\n";
  cout << "Resources                | Hits \n";
  cout << "--------------------------------\n";

  // Print each resource and its hits
  for (const auto& hit : sortedHits) {
    cout << setw(25) << left << hit.first;
    cout << '|';
    cout << setw(5) << right << hit.second << '\n';
  }

  // If we don't have many resources hit draw some rows anyway
  // so that it's more readable
  if (sortedHits.size() <= HITS_TABLE_ROWS) {
    cout << string(HITS_TABLE_ROWS - sortedHits.size(), '\n');
  }

  cout << "--------------------------------\n";
  cout << setw(25) << left << "Total";
  cout << '|';
  cout << setw(5) << right << totalHits << "\n";
  cout << "--------------------------------\n";
}

// Prints elapsed time, current time, total hits and total traffic from start
void Display::printGeneralInfo() const noexcept {
  cout << "================================\n";
  cout << "          General info          \n";
  cout << "--------------------------------\n";
  auto now = time_point_cast<seconds>(system_clock::now());
  auto elapsed = duration_cast<seconds>(now - _startTime);
  cout << setw(25) << left << "Elapsed time: " << date::format("%T", elapsed)
       << '\n';

  cout << setw(25) << left << "Total hits:" << _monitor->totalHits() << '\n';
  // Traffic in KB
  auto traffic = _monitor->totalTraffic() / 1000.f;
  cout << setw(25) << left << "Total traffic:" << traffic << "KB\n\n";

  cout << setw(25) << left << "Alert threshold: " << _monitor->alertThreshold()
       << " per second\n";
  cout << setw(25) << left
       << "Alert duration: " << _monitor->alertDuration().count()
       << " seconds\n";
}
