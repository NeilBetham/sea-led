/**
 * @brief Simple logging to UART
 */

#pragma once

#include "uart.h"

enum class LogLevel : uint8_t {
  unknown = 0,
  debug,
  info,
  warn,
  error
};

void logging_init(UART* output_uart);
void logging_set_log_level(LogLevel level);

void log_message(LogLevel level, const char* message, ...);

void log_e(const char* message...);
void log_w(const char* message...);
void log_i(const char* message...);
void log_d(const char* message...);
