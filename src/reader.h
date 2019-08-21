#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <thread>

using namespace std;
namespace fs = std::filesystem;

class Reader {
  using cbFun = function<void(string)>;

 public:
  Reader(const fs::path& file, const cbFun callback);
  virtual ~Reader();

  Reader(Reader&&) = delete;
  Reader(const Reader&) = delete;
  Reader& operator=(Reader&&) = delete;
  Reader& operator=(const Reader&) = delete;

  void start();

 private:
  unique_ptr<thread> _thread;
  ifstream _file;
  cbFun _callback;

  [[noreturn]] void read() noexcept;
};
