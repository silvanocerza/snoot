#pragma once

#include <chrono>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

#include <date.h>

using namespace std;

namespace {
// This is used only to create LogItems, no one else should need it
static constexpr auto regexString =
    R"~(^(.+?) (.+?) (.+?) \[(.+?)\] "([A-Z]+) (.+?) (.+?)" (\d+?) (\d+?)$)~";
}  // namespace

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

struct LogItem {
  string host;
  string rfc931;
  string user;
  chrono::system_clock::time_point dateTime;
  Method method;
  string resource;
  string protocol;
  int status;
  int size;

  // Returns a new LogItem filled with data parsed from `s`.
  // If string is not parsed correctly returns an empty LogItem.
  static LogItem from(const string& s) {
    static regex r(regexString);

    smatch match;
    regex_match(s, match, r);

    if (match.empty() || match.size() < 10) {
      return {};
    }

    // This is used to convert from method string to our Method
    // enum, just to be a bit more space efficient since an enum
    // occupies way less space than a string.
    // Also if we ever need to compare our logs by method using
    // the enum will be way faster.
    static unordered_map<string, Method> methods{
        {"GET", Method::GET},         {"HEAD", Method::HEAD},
        {"POST", Method::POST},       {"PUT", Method::PUT},
        {"DELETE", Method::DELETE},   {"CONNECT", Method::CONNECT},
        {"OPTIONS", Method::OPTIONS}, {"TRACE", Method::TRACE},
    };

    auto host = match.str(1);
    auto rfc931 = match.str(2);
    auto user = match.str(3);

    // chrono::parse would have been great here but it's C++20
    // and I want to stay at C++17 to ease compilation
    chrono::system_clock::time_point dateTime;
    stringstream dateStream(match.str(4));
    // We use the system locale to parse the date
    dateStream.imbue(locale(""));
    // The date format could be made configurable
    // TODO: Handle parsing failure
    dateStream >> date::parse("%d/%b/%Y:%T %z", dateTime);

    // TODO: Handle bogus methods
    auto method = methods.at(match.str(5));

    auto resource = match.str(6);
    auto separatorPosition = resource.find_first_of('/', 1);
    if (separatorPosition != string::npos) {
      resource = resource.substr(0, separatorPosition);
    }

    auto protocol = match.str(7);
    auto status = atoi(match.str(8).c_str());
    auto size = atoi(match.str(9).c_str());

    return {host,     rfc931,   user,   dateTime, method,
            resource, protocol, status, size};
  }
};
