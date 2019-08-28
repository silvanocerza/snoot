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

    Monitor monitor(logFile, 10, 15s, 5);
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
    Monitor monitor(logFile, 2, 2s, 5);
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

  SECTION("Verifies average hits to trigger alert is calculated correctly") {
    Monitor monitor1(logFile, 10, 120s, 5);
    monitor1.start();
    REQUIRE(monitor1.averageHitsToAlert() == 1200);

    Monitor monitor2(logFile, 2, 2s, 5);
    monitor2.start();
    REQUIRE(monitor2.averageHitsToAlert() == 4);

    Monitor monitor3(logFile, 4, 10s, 5);
    monitor3.start();
    REQUIRE(monitor3.averageHitsToAlert() == 40);
  }

  SECTION("Verifes total number of hits and total traffic is correct") {
    ofstream f(logFile, ios::ate | ios::out);

    Monitor monitor(logFile, 10, 2s, 5);
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

    // Waits for processing
    this_thread::sleep_for(0.5s);

    REQUIRE(monitor.logs().size() == 3);
    REQUIRE(monitor.totalHits() == 3);
    REQUIRE(monitor.totalTraffic() == 276);

    // Waits a bit to go over alert threshold so old logs are deleted
    this_thread::sleep_for(2s);

    now = time_point_cast<seconds>(system_clock::now());
    dateTime = date::format(locale(""), "%d/%b/%EY:%T %z", now);

    // Writes some more logs
    f << "127.0.0.1 - alien ";
    f << "[" << dateTime << "] ";
    f << "\"HEAD /docs/api/tutorial HTTP/1.0\" ";
    f << "202 69" << endl;

    f << "127.0.0.1 - alien ";
    f << "[" << dateTime << "] ";
    f << "\"CONNECT /docs/api/reference HTTP/1.0\" ";
    f << "500 420" << endl;

    // Waits for processing
    this_thread::sleep_for(0.5s);

    REQUIRE(monitor.logs().size() == 2);
    REQUIRE(monitor.totalHits() == 5);
    REQUIRE(monitor.totalTraffic() == 765);

    f.close();
    fs::remove(logFile);
  }

  SECTION("Verifies constructor error handling") {
    REQUIRE_THROWS_WITH(
        Monitor(logFile, 0, 10s, 5),
        "Hits per second to trigger an alert must not be zero.");

    REQUIRE_THROWS_WITH(Monitor(logFile, 10, 0s, 5),
                        "An alert duration can't be zero.");

    fs::path unexistingFile("unexistingFile");
    REQUIRE_THROWS_WITH(Monitor(unexistingFile, 10, 10s, 5),
                        "File 'unexistingFile' not found.");
  }

  SECTION("Verifies alerts are deleted when alert history is reached") {
    ofstream f(logFile, ios::ate | ios::out);

    // If there are more than 2 request per second in the last 2 seconds
    // trigger an alert
    Monitor monitor(logFile, 2, 2s, 2);
    monitor.start();

    // Verifies we have no alerts by default
    REQUIRE(monitor.alerts().size() == 0);

    // Trigger an alert
    auto now = time_point_cast<seconds>(system_clock::now());
    auto dateTime = date::format(locale(""), "%d/%b/%EY:%T %z", now);

    auto logLine = "127.0.0.1 - alien [" + dateTime +
                   "] \"HEAD /docs/api/tutorial HTTP/1.0\" "
                   "202 40";
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;

    // Waits for processing
    this_thread::sleep_for(0.5s);

    // Verifies new alert is triggered
    REQUIRE(monitor.alerts().size() == 1);

    // Waits for alert to recover
    this_thread::sleep_for(3s);

    // Trigger second alert
    now = time_point_cast<seconds>(system_clock::now());
    dateTime = date::format(locale(""), "%d/%b/%EY:%T %z", now);

    logLine = "127.0.0.1 - alien [" + dateTime +
              "] \"HEAD /docs/api/tutorial HTTP/1.0\" "
              "202 40";
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;

    // Waits for processing
    this_thread::sleep_for(0.5s);

    // Verifies new alert is triggered
    REQUIRE(monitor.alerts().size() == 2);

    // Waits for alert to recover
    this_thread::sleep_for(3s);

    // Trigger third alert
    now = time_point_cast<seconds>(system_clock::now());
    dateTime = date::format(locale(""), "%d/%b/%EY:%T %z", now);

    logLine = "127.0.0.1 - alien [" + dateTime +
              "] \"HEAD /docs/api/tutorial HTTP/1.0\" "
              "202 40";
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;
    f << logLine << endl;

    // Waits for processing
    this_thread::sleep_for(0.5s);

    // Verifies new alert is triggered but number of alerts
    // doesn't go over alert history
    REQUIRE(monitor.alerts().size() == 2);

    f.close();
    fs::remove(logFile);
  }
}
