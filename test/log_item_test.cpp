#include <catch.hpp>

#include "../src/log_item.h"

#include "utils.h"

using namespace chrono;

TEST_CASE("LogItem") {
  SECTION("Verifies creation from valid log string") {
    auto dateTime = toSystemLocale("22/Aug/2019:12:17:25 +0200");

    auto logString = "127.0.0.1 - alien [" + dateTime +
                     "] \"GET /report HTTP/1.0\" "
                     "200 25";

    auto log = LogItem::from(logString);

    REQUIRE(log.isValid() == true);
    REQUIRE(log.host() == "127.0.0.1");
    REQUIRE(log.rfc931() == "-");
    REQUIRE(log.user() == "alien");
    auto ms = time_point_cast<milliseconds>(log.dateTime());
    REQUIRE(ms.time_since_epoch().count() == 1566469045000);
    REQUIRE(log.method() == Method::GET);
    REQUIRE(log.resource() == "/report");
    REQUIRE(log.protocol() == "HTTP/1.0");
    REQUIRE(log.status() == 200);
    REQUIRE(log.size() == 25);
  }

  SECTION("Verifies creation from log string with deep resource") {
    auto dateTime = toSystemLocale("22/Aug/2019:12:17:25 +0200");

    auto logString = "127.0.0.1 - alien [" + dateTime +
                     "] \"GET /api/user HTTP/1.0\" "
                     "200 25";

    auto log = LogItem::from(logString);

    REQUIRE(log.isValid() == true);
    REQUIRE(log.host() == "127.0.0.1");
    REQUIRE(log.rfc931() == "-");
    REQUIRE(log.user() == "alien");
    auto ms = time_point_cast<milliseconds>(log.dateTime());
    REQUIRE(ms.time_since_epoch().count() == 1566469045000);
    REQUIRE(log.method() == Method::GET);
    REQUIRE(log.resource() == "/api");
    REQUIRE(log.protocol() == "HTTP/1.0");
    REQUIRE(log.status() == 200);
    REQUIRE(log.size() == 25);
  }

  SECTION(
      "Verifies log string with unexisting method returns invalid LogItem") {
    auto dateTime = toSystemLocale("22/Aug/2019:12:17:25 +0200");

    auto logString = "127.0.0.1 - alien [" + dateTime +
                     "] \"WHAAAAAAAAT /api/user HTTP/1.0\" "
                     "200 25";

    auto log = LogItem::from(logString);
    REQUIRE(log.isValid() == false);
  }

  SECTION(
      "Verifies log string with uncorrectly formatted date returns invalid "
      "LogItem") {
    auto logString =
        "127.0.0.1 - alien "
        "[000/123/2012:25:62:63 +12345] "
        "\"GET /api/user HTTP/1.0\" "
        "200 25";

    auto log = LogItem::from(logString);
    REQUIRE(log.isValid() == false);
  }

  SECTION("Verifies lower case method matches correctly") {
    auto dateTime = toSystemLocale("22/Aug/2019:12:17:25 +0200");

    auto logString = "127.0.0.1 - alien [" + dateTime +
                     "] \"patch /api/user HTTP/1.0\" "
                     "200 25";

    auto log = LogItem::from(logString);
    REQUIRE(log.isValid() == true);
    REQUIRE(log.host() == "127.0.0.1");
    REQUIRE(log.rfc931() == "-");
    REQUIRE(log.user() == "alien");
    auto ms = time_point_cast<milliseconds>(log.dateTime());
    REQUIRE(ms.time_since_epoch().count() == 1566469045000);
    REQUIRE(log.method() == Method::PATCH);
    REQUIRE(log.resource() == "/api");
    REQUIRE(log.protocol() == "HTTP/1.0");
    REQUIRE(log.status() == 200);
    REQUIRE(log.size() == 25);
  }

  SECTION("Verifies resource not starting with / doesn't match") {
    auto dateTime = toSystemLocale("22/Aug/2019:12:17:25 +0200");

    auto logString = "127.0.0.1 - alien [" + dateTime +
                     "] \"GET bad/resource HTTP/1.0\" "
                     "200 25";
    auto log = LogItem::from(logString);
    REQUIRE(log.isValid() == false);
  }
}
