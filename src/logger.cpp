#include "logger.h"

Logger::Logger(const fs::path logFile) {
  switch (fs::status(logFile).type()) {
    case fs::file_type::none: {
      throw runtime_error("Something really bad has happened!");
    };
    case fs::file_type::not_found: {
      throw runtime_error("File not found.");
    };
    case fs::file_type::regular: {
      // TODO: THIS IS IT
      break;
    };
    case fs::file_type::block:
    case fs::file_type::directory:
    case fs::file_type::socket:
    case fs::file_type::character: {
      throw runtime_error(
          "Can't read from provided path, please use a regular file.");
    };
    case fs::file_type::symlink: {
      // TODO: Maybe we can handle this?
      break;
    };
    case fs::file_type::fifo: {
      // TODO: Maybe I can handle this too? I'll give it a try
      break;
    };
    case fs::file_type::unknown: {
      throw runtime_error("Unknown file type");
    };
  }
}

Logger::~Logger() {}

void Logger::start() const noexcept {}
