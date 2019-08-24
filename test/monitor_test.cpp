#include <catch.hpp>

#include "../src/monitor.h"

using namespace chrono;

TEST_CASE("Monitor") {
  fs::path logFile("testLogFile");

  if (fs::exists(logFile)) {
    fs::remove(logFile);
  }

  // Create file
  ofstream f(logFile);
  f.close();

  unsigned long hitsThreshold = 10;
  chrono::seconds alertDuration = 15s;

  SECTION("Verifies Monitor logs correct number of logs") {
    ofstream f(logFile, ios::ate | ios::out);

    Monitor monitor(logFile, hitsThreshold, alertDuration);
    monitor.start();

    auto now = time_point_cast<seconds>(system_clock::now());
    auto dateTime = date::format(locale(""), "%d/%b/%EY:%T %z", now);

    // Writes some lines to logFile
    f << "127.0.0.1 - alien ";
    f << "[" << dateTime << "] ";
    f << "\"HEAD /docs/api/tutorial HTTP/1.0\" ";
    f << "202 40" << endl;

    f << "127.0.0.1 - alien ";
    f << "[" << dateTime << "] ";
    f << "\"CONNECT /docs/api/reference HTTP/1.0\" ";
    f << "500 113" << endl;

    // Write some empty lines too
    f << endl;
    f << endl;

    f << "127.0.0.1 - alien ";
    f << "[" << dateTime << "] ";
    f << "\"PUT /api HTTP/1.0\" ";
    f << "400 123" << endl;

    // Waits a bit for the Monitor thread to process the lines
    this_thread::sleep_for(0.5s);

    REQUIRE(monitor.logs().size() == 3);

    f.close();
    fs::remove(logFile);
  }
}
