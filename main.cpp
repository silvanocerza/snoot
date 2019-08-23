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
      parser, "file", "The HTTP access log file, default to /tmp/access.log",
      {'f', "file"});
  args::ValueFlag<unsigned long> hitsArgs(
      parser, "hits threshold",
      "The amount of hits per second necessary to trigger an alert",
      {'t', "threshold"});

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

  unsigned long hitsThreshold = 5;
  if (hitsArgs) {
    hitsThreshold = hitsArgs.Get();
  }

  auto alertDuration = 10s;

  Display d(file, hitsThreshold, alertDuration);
  d.run();
  return 0;
}
