#include <chrono>
#include <iostream>
#include <set>
#include <thread>
#include <unordered_map>
#include <utility>

#include <date.h>

#include "display.h"

using namespace chrono;

Display::Display(const fs::path& logFile) : _monitor(new Monitor(logFile)) {}

[[noreturn]] void Display::run() {
  _monitor->start();

  while (true) {
    Display::clear();

    auto logs = _monitor->logData();

    cout << "Hits in 10 seconds: " << logs.size();

    cout << "\n\n";

    printHitsTable(logs);

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

void Display::printHitsTable(list<LogItem> logs) const noexcept {
  // First count resources hits
  unordered_map<string, int> resourcesHits;
  for (const auto& item : logs) {
    if (resourcesHits.count(item.resource) == 0) {
      resourcesHits.emplace(item.resource, 1);
    } else {
      resourcesHits[item.resource]++;
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
  cout << setw(25) << left << "Resources";
  cout << '|';
  cout << setw(5) << right << "Hits";
  cout << '\n';

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
}
