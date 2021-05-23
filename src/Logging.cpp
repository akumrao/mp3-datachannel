

#include "rtcdcpp/Logging.hpp"

namespace rtcdcpp {

#ifndef SPDLOG_DISABLED

#include <mutex>

std::shared_ptr<Logger> GetLogger(const std::string &logger_name) {
  auto logger = spdlog::get(logger_name);
  //spdlog::set_level(spdlog::level::trace); // Set global log level to debug

  if (logger) {
    return logger;
  }
  return spdlog::stdout_color_mt(logger_name);
}

#else

std::shared_ptr<Logger> GetLogger(const std::string &logger) {
  return std::make_shared<Logger>();
}

#endif

}
