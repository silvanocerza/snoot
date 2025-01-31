#include <filesystem>
#include <iostream>

#include <args.hxx>

#include "display.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, const char* argv[]) {
  args::ArgumentParser parser("Snoot, HTTP access log monitor");
  args::HelpFlag help(parser, "help", "Display this menu", {'h', "help"});
  args::ValueFlag<fs::path> fileArg(
      parser, "file", "The HTTP access log file, defaults to /tmp/access.log",
      {'f', "file"});
  args::ValueFlag<unsigned long> hitsArg(
      parser, "hits threshold",
      "The amount of hits per second necessary to trigger an alert, defaults "
      "to 10",
      {'t', "threshold"});
  args::ValueFlag<unsigned long> alertDurationArg(
      parser, "alert duration",
      "The number of seconds to look back when searching for the number of "
      "logs necessary to trigger an alert, defaults to 120",
      {'d', "alert-duration"});
  args::ValueFlag<unsigned long> refreshRateArg(
      parser, "refresh rate",
      "Number of seconds after which the displayed information is refreshed, "
      "defaults to 10",
      {'r', "refresh-rate"});
  args::ValueFlag<unsigned int> alertHistoryArg(
      parser, "alert history",
      "Number of alerts to show in list of triggered alerts",
      {'s', "alert-history"});

  try {
    parser.ParseCLI(argc, argv);
  } catch (const args::Help&) {
    cout << parser;
    return 0;
  } catch (const args::ParseError& e) {
    cerr << e.what() << endl;
    cerr << parser << endl;
    return 1;
  }

  fs::path file("/tmp/access.log");
  if (fileArg) {
    file = fileArg.Get();
  }

  unsigned long hitsThreshold = 10;
  if (hitsArg) {
    hitsThreshold = hitsArg.Get();
  }

  auto alertDuration = 120s;
  if (alertDurationArg) {
    alertDuration = chrono::seconds(alertDurationArg.Get());
  }

  auto refreshRate = 10s;
  if (refreshRateArg) {
    refreshRate = chrono::seconds(refreshRateArg.Get());
  }

  unsigned int alertHistory = 5;
  if (alertHistoryArg) {
    alertHistory = alertHistoryArg.Get();
  }

  Display d(file, hitsThreshold, alertDuration, alertHistory, refreshRate);
  d.run();
  return 0;
}
