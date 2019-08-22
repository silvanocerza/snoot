#include <filesystem>
#include <iostream>

#include <args.hxx>

#include "display.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, const char* argv[]) {
  args::ArgumentParser parser("Snoot, HTTP access log monitor");
  args::HelpFlag help(parser, "help", "Display this menu", {'h', "help"});
  args::ValueFlag<fs::path> fileArg(parser, "file", "The HTTP access log file",
                                    {'f', "file"});

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

  Display d(file);
  d.run();
  return 0;
}
