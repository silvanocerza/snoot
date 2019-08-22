#include "../src/log_item.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace chrono;

TEST_CASE("LogItem") {
  SECTION("Creation from valid log string") {
    auto logString =
        "127.0.0.1 - alien "
        "[22/ago/2019:12:17:25 +0200] "
        "\"GET /report HTTP/1.0\" "
        "200 25";

    auto log = LogItem::from(logString);

    REQUIRE(log.host == "127.0.0.1");
    REQUIRE(log.rfc931 == "-");
    REQUIRE(log.user == "alien");
    auto ms = time_point_cast<milliseconds>(log.dateTime);
    REQUIRE(ms.time_since_epoch().count() == 1566469045000);
    REQUIRE(log.method == Method::GET);
    REQUIRE(log.resource == "/report");
    REQUIRE(log.protocol == "HTTP/1.0");
    REQUIRE(log.status == 200);
    REQUIRE(log.size == 25);
  }

  SECTION("Creation from log string with deep resource") {
    auto logString =
        "127.0.0.1 - alien "
        "[22/ago/2019:12:17:25 +0200] "
        "\"GET /api/user HTTP/1.0\" "
        "200 25";

    auto log = LogItem::from(logString);

    REQUIRE(log.host == "127.0.0.1");
    REQUIRE(log.rfc931 == "-");
    REQUIRE(log.user == "alien");
    auto ms = time_point_cast<milliseconds>(log.dateTime);
    REQUIRE(ms.time_since_epoch().count() == 1566469045000);
    REQUIRE(log.method == Method::GET);
    REQUIRE(log.resource == "/api");
    REQUIRE(log.protocol == "HTTP/1.0");
    REQUIRE(log.status == 200);
    REQUIRE(log.size == 25);
  }
}
