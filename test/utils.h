#pragma once

#include <chrono>
#include <string>

#include <date.h>

using namespace std;
using namespace chrono;

// Utility method to convert an a date string from a US locale
// to current system locale.
// The date must be formatted like "22/Aug/2019:12:17:25 +0200"
// Returns a date formatted in the same but using system language.
// This is needed because the program is locale aware and parses
// date strings using the system's so we need to always use it
// otherwise it would fail miserably. The main issue is the
// month since it's different for each language.
string toSystemLocale(const string& date) {
  system_clock::time_point dateTime;
  stringstream dateStream(date);
  dateStream.imbue(locale("en_US.UTF-8"));
  dateStream >> date::parse("%d/%b/%Y:%T %z", dateTime);
  return date::format(locale(""), "%d/%b/%EY:%T %z", dateTime);
}
