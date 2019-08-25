#include <regex>
#include <sstream>
#include <unordered_map>

#include <date.h>

#include "log_item.h"

static constexpr auto regexString =
    R"~(^(.+?) (.+?) (.+?) \[(.+?)\] "(GET|HEAD|POST|PUT|DELETE|TRACE|OPTIONS|CONNECT|PATCH) (\/.+?) (.+?)" (\d+?) (\d+?)$)~";

LogItem::LogItem() : _isValid(false) {}

LogItem::LogItem(string host, string rfc931, string user,
                 chrono::system_clock::time_point dateTime, Method method,
                 string resource, string protocol, int status, int size)
    : _isValid(true),
      _host(host),
      _rfc931(rfc931),
      _user(user),
      _dateTime(dateTime),
      _method(method),
      _resource(resource),
      _protocol(protocol),
      _status(status),
      _size(size) {}

// Returns a new LogItem filled with data parsed from `s`.
// If string is not parsed correctly returns an empty LogItem.
LogItem LogItem::from(const string& s) {
  static regex r(regexString, regex::ECMAScript | regex::icase);

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
      {"DELETE", Method::DELETE},   {"TRACE", Method::TRACE},
      {"OPTIONS", Method::OPTIONS}, {"CONNECT", Method::CONNECT},
      {"PATCH", Method::PATCH}};

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
  dateStream >> date::parse("%d/%b/%Y:%T %z", dateTime);
  // Returns invalid LogItem if date can't be parsed
  if (dateStream.fail()) {
    return {};
  }

  auto methodString = match.str(5);
  transform(methodString.begin(), methodString.end(), methodString.begin(),
            [](unsigned char c) { return toupper(c); });
  auto method = methods.at(methodString);

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
