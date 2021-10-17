#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

constexpr uint32_t MAX_MESSAGE_LENGTH = 1024;
static UART* global_logger_uart = NULL;
static LogLevel global_log_level = LogLevel::unknown;

void logging_init(UART* output_uart) {
  global_logger_uart = output_uart;
}

void logging_set_log_level(LogLevel level) {
  global_log_level = level;
}

void log_message(LogLevel level, const char* message, ...) {
  if(level == LogLevel::unknown) { return; }

  // Check if we have an ouput UART to log to
  if(global_logger_uart == NULL) { return; }

  // Check if the log level should be printed
  if(level < global_log_level) { return; }

  // Get the var args to pass to snprintf
  va_list arg_pointer;
  va_start(arg_pointer, message);

  // Setup some scratch space for the formatting
  int32_t header_bytes = 0;
  uint8_t buffer[MAX_MESSAGE_LENGTH + 2] = {0};

  // Insert the level at the front of the string
  char* header = NULL;
  switch(level) {
    case LogLevel::debug:
      header_bytes = snprintf((char*)buffer, MAX_MESSAGE_LENGTH, "[DEBUG] - ");
      break;
    case LogLevel::info:
      header_bytes = snprintf((char*)buffer, MAX_MESSAGE_LENGTH, "[INFO] - ");
      break;
    case LogLevel::warn:
      header_bytes = snprintf((char*)buffer, MAX_MESSAGE_LENGTH, "[WARN] - ");
      break;
    case LogLevel::error:
      header_bytes = snprintf((char*)buffer, MAX_MESSAGE_LENGTH, "[ERROR] - ");
      break;
    default:
      return;
  }

  // Put the message content into the buffer
  int32_t message_bytes = snprintf((char*)buffer + header_bytes, MAX_MESSAGE_LENGTH - header_bytes, message, arg_pointer);

  // Set the last character to be a new line
  buffer[header_bytes + message_bytes++] = '\n';
  buffer[header_bytes + message_bytes++] = '\r';
  buffer[header_bytes + message_bytes++] = 0;

  // Write to the UART
  global_logger_uart->send(buffer, header_bytes + message_bytes);

  // Finish the var arg processing
  va_end(arg_pointer);
}

void log_e(const char* message, ...) {
  va_list arg_pointer;
  va_start(arg_pointer, message);

  log_message(LogLevel::error, message, arg_pointer);

  va_end(arg_pointer);
}

void log_w(const char* message, ...) {
  va_list arg_pointer;
  va_start(arg_pointer, message);

  log_message(LogLevel::warn, message, arg_pointer);

  va_end(arg_pointer);
}

void log_i(const char* message, ...) {
  va_list arg_pointer;
  va_start(arg_pointer, message);

  log_message(LogLevel::info, message, arg_pointer);

  va_end(arg_pointer);
}

void log_d(const char* message, ...) {
  va_list arg_pointer;
  va_start(arg_pointer, message);

  log_message(LogLevel::debug, message, arg_pointer);

  va_end(arg_pointer);
}

