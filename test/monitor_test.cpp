#include <date.h>
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

  SECTION("Verifies Monitor logs correct number of log lines") {
    ofstream f(logFile, ios::ate | ios::out);

    Monitor monitor(logFile, 10, 15s);
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

  SECTION("Verifies alert trigger when traffic exceeds average") {
    ofstream f(logFile, ios::ate | ios::out);

    // If there are more than 2 request per second in the last 2 seconds
    // trigger an alert
    Monitor monitor(logFile, 2, 2s);
    monitor.start();

    // Verifies we have no alerts by default
    REQUIRE(monitor.alerts().size() == 0);

    auto now = time_point_cast<seconds>(system_clock::now());
    auto dateTime = date::format(locale(""), "%d/%b/%EY:%T %z", now);

    auto logLine = "127.0.0.1 - alien [" + dateTime +
                   "] \"HEAD /docs/api/tutorial HTTP/1.0\" "
                   "202 40";

    // Write 5 logs, one over the average to trigger the alert
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;

    // Waits for processing
    this_thread::sleep_for(0.5s);

    // Verifies new alert is triggered
    REQUIRE(monitor.alerts().size() == 1);
    REQUIRE(monitor.alerts().front().hits == 5);
    REQUIRE(!monitor.alerts().front().hasRecovered());

    // Waits for alert to recover
    this_thread::sleep_for(3s);

    // Verifies alert has now recovered
    REQUIRE(monitor.alerts().size() == 1);
    REQUIRE(monitor.alerts().front().hits == 5);
    REQUIRE(monitor.alerts().front().hasRecovered());

    f.close();
    fs::remove(logFile);
  }
}
