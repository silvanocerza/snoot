#include <exception>
#include <iostream>

#include "reader.h"

Reader::Reader(const fs::path& file, const cbFun callback)
    : _callback(callback) {
  switch (fs::status(file).type()) {
    case fs::file_type::fifo:
    case fs::file_type::symlink:
    case fs::file_type::regular: {
      // We got a usable file, just keep going
      break;
    };
    case fs::file_type::none: {
      // ¯\_(ツ)_/¯
      throw runtime_error("Something really bad has happened!");
    };
    case fs::file_type::not_found: {
      throw runtime_error("File not found.");
    };
    case fs::file_type::block:
    case fs::file_type::directory:
    case fs::file_type::socket:
    case fs::file_type::character: {
      throw runtime_error(
          "Can't read from provided path, please use a regular file.");
    };
    case fs::file_type::unknown: {
      throw runtime_error("Unknown file type");
    };
  }

  _file.open(file, ios::ate | ios::in);

  if (not _file.is_open()) {
    throw system_error(errno, system_category(),
                       "Error opening file " + file.string());
  }
}

Reader::~Reader() { _file.close(); }

void Reader::start() { _thread.reset(new thread(&Reader::read, this)); }

[[noreturn]] void Reader::read() noexcept {
  string line;
  // Read and read and read
  while (true) {
    while (getline(_file, line)) {
      cerr << "reader " << this_thread::get_id() << endl;
      _callback(line);
    }

    if (not _file.eof()) {
      // TODO: Handle this?
    }
    // Clear eof state so that we can keep reading
    _file.clear();
  }
}
