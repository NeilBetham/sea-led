/**
 * @brief Simple logging to UART
 */

#pragma once

#include "uart.h"

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <string>

enum class LogLevel : uint8_t {
  unknown = 0,
  debug,
  info,
  warn,
  error
};

void logging_init(UART* output_uart);
void logging_set_log_level(LogLevel level);

void log_message(LogLevel level, const std::string& message);

template <typename ... Ts>
std::string format_args(Ts ... args) {
  try {
    return fmt::format(args...);
  } catch(const std::exception& e) {
    return e.what();
  }
}


template <typename ... Ts>
void log_e(Ts ... args) {
  auto message = format_args(args...);
  auto msg_hdr = fmt::format("[ERROR] - {}\r\n", message);
  log_message(LogLevel::error, msg_hdr);
}

template <typename ... Ts>
void log_w(Ts ... args) {
  auto message = format_args(args...);
  auto msg_hdr = fmt::format("[WARN] - {}\r\n", message);
  log_message(LogLevel::error, msg_hdr);
}

template <typename ... Ts>
void log_i(Ts ... args) {
  auto message = format_args(args...);
  auto msg_hdr = fmt::format("[INFO] - {}\r\n", message);
  log_message(LogLevel::error, msg_hdr);
}

template <typename ... Ts>
void log_d(Ts ... args) {
  auto message = format_args(args...);
  auto msg_hdr = fmt::format("[DEBUG] - {}\r\n", message);
  log_message(LogLevel::error, msg_hdr);
}

