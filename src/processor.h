#pragma once

#include <chrono>
#include <regex>
#include <string>

using namespace std;

class Processor {
 public:
  Processor();
  virtual ~Processor() = default;

  Processor(Processor&&) = delete;
  Processor(const Processor&) = delete;
  Processor& operator=(Processor&&) = delete;
  Processor& operator=(const Processor&) = delete;

  void process(const string& s);

 private:
  static constexpr auto regexString =
      "^(?<host>.+) (?<rfc931>.+) (?<user>.+) [(?<datetime>.+)] "
      "\"(?<method>[A-Z]+) (?<resource>.+) (?<protocol>.+)\" (?<status>\\d+)"
      " (?<size>\\d+)$";
  const regex _r;

  static chrono::system_clock::time_point parseDateTime(
      const string& dateTime) noexcept;
};
