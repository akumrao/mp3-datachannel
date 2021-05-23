

#pragma once

#include <memory>

#ifndef SPDLOG_DISABLED
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/fmt/ostr.h>
#endif

namespace rtcdcpp {

#ifndef SPDLOG_DISABLED

typedef spdlog::logger Logger;

#else

class Logger {
 public:

  Logger() = default;

  Logger(const Logger &) = delete;
  void operator=(const Logger &) = delete;
  Logger(Logger &&) = delete;
  void operator=(Logger &&) = delete;

  template<typename... Args>
  void trace(const char *fmt, const Args &... args) {}
  template<typename... Args>
  void debug(const char *fmt, const Args &... args) {}
  template<typename... Args>
  void info(const char *fmt, const Args &... args) {}
  template<typename... Args>
  void warn(const char *fmt, const Args &... args) {}
  template<typename... Args>
  void error(const char *fmt, const Args &... args) {}
  template<typename... Args>
  void critical(const char *fmt, const Args &... args) {}

  template<typename T>
  void trace(const T &) {}
  template<typename T>
  void debug(const T &) {}
  template<typename T>
  void info(const T &) {}
  template<typename T>
  void warn(const T &) {}
  template<typename T>
  void error(const T &) {}
  template<typename T>
  void critical(const T &) {}
};

#define SPDLOG_TRACE(logger, ...)
#define SPDLOG_DEBUG(logger, ...)

#endif

std::shared_ptr<Logger> GetLogger(const std::string &logger_name);

}
