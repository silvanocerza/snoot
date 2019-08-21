#include <iomanip>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "processor.h"

enum class Method {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
};

Processor::Processor() : _r(regexString) {}

void Processor::process(const string& s) {
  smatch match;
  regex_match(s, match, _r);

  // Somehow nothing matched, just skip
  if (match.empty()) {
    return;
  }

  if (match.size() != 9) {
    return;
  }

  static unordered_map<string, Method> methods{
      {"GET", Method::GET},         {"HEAD", Method::HEAD},
      {"POST", Method::POST},       {"PUT", Method::PUT},
      {"DELETE", Method::DELETE},   {"CONNECT", Method::CONNECT},
      {"OPTIONS", Method::OPTIONS}, {"TRACE", Method::TRACE},
  };

  auto host = match.str(1);
  auto rfc931 = match.str(2);
  auto user = match.str(3);

  //  auto dateTime = Processor::parseDateTime(match.str(4));

  //  auto method = methods.at(match.str(5));

  auto resource = match.str(6);
  auto protocol = match.str(7);
  //  auto status = atoi(match.str(8).c_str());

  //  auto size = atoi(match.str(9).c_str());
}

chrono::system_clock::time_point Processor::parseDateTime(
    const string& dateTime) noexcept {
  // chrono::parse would have been great here but it's C++20
  // but I want to stay at C++17

  tm time;
  // The date format could be made configurable
  stringstream(dateTime) >> get_time(&time, "%d/%b/%Y:%H:%M:%S %z");

  return chrono::system_clock::from_time_t(mktime(&time));
}
