#pragma once

#include <chrono>
#include <string>

using namespace std;

// Parsable HTTP methods
enum class Method {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  OPTIONS,
  CONNECT,
  PATCH,
};

// Represents a single line from the log file parsed by Monitor.
// Its constructors are private so a new instance can be retrieved
// by parsing a log line with its static method from(const string&).
// If a line can't be parsed correctly an invalid instance is returned.
class LogItem {
 public:
  ~LogItem() = default;

  LogItem(const LogItem& other) = default;
  LogItem(LogItem&& other) = default;
  LogItem& operator=(const LogItem& other) = default;
  LogItem& operator=(LogItem&& other) = default;

  bool isValid() const noexcept { return _isValid; }
  string host() const noexcept { return _host; }
  string rfc931() const noexcept { return _rfc931; }
  string user() const noexcept { return _user; }
  chrono::system_clock::time_point dateTime() const noexcept {
    return _dateTime;
  }
  Method method() const noexcept { return _method; }
  string resource() const noexcept { return _resource; }
  string protocol() const noexcept { return _protocol; }
  int status() const noexcept { return _status; }
  int size() const noexcept { return _size; }

  static LogItem from(const string& s);

 private:
  bool _isValid;
  // Remote host name
  string _host;
  // Remote user log name
  string _rfc931;
  // Username the user authenticated with
  string _user;
  // Date and time of the request
  chrono::system_clock::time_point _dateTime;
  // HTTP method
  Method _method;
  // Resource requested
  string _resource;
  // Protocol used for the request
  string _protocol;
  // HTTP status code returned by the request
  int _status;
  // Content length of the document transferred in bytes
  int _size;

  LogItem();
  LogItem(string host, string rfc931, string user,
          chrono::system_clock::time_point dateTime, Method method,
          string resource, string protocol, int status, int size);
};
