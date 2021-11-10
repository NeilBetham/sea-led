#include "http_utils.h"

#include "logging.h"

#define FMT_HEADER_ONLY
#include <fmt/core.h>

uint32_t hex_to_int(const char* str, uint32_t len) {
  uint32_t value = 0;

  for(uint32_t index = 0; index < len; index++) {
    uint8_t nyb = str[index];
    if(nyb >= '0' && nyb <= '9') { nyb = nyb -'0'; }
    else if(nyb >= 'a' && nyb <='f') { nyb = nyb - 'a' + 10; }
    else if(nyb >= 'A' && nyb <='F') { nyb = nyb - 'A' + 10; }

    value = (value << 4) | (nyb & 0xF);
  }

  return value;
}

uint8_t checksum(const std::string& buffer) {
  uint8_t sum = 0;
  for(uint32_t index = 0; index < buffer.size(); index++) {
    sum -= buffer[index];
  }
  return sum;
}

void parse_path(Path& path, const char* path_str, uint32_t path_len) {
  bool question_mark_found = false;
  bool consuming_key = false;
  bool consuming_value = false;
  uint32_t current_string_start = 0;
  uint32_t current_query_param = 0;

  for(uint32_t index = 0; index < path_len; index++) {
    // First find the question mark indicating the start of params
    if(question_mark_found != true) {
      if(path_str[index] == '?') {
        question_mark_found = true;

        path.params[current_query_param].key = &(path_str[index + 1]);
        current_string_start = index + 1;
        consuming_key = true;
        continue;
      }
    }

    // Start consuming keys and values
    if(consuming_key) {
      if(path_str[index] == '=') {
        path.params[current_query_param].key_len = index - current_string_start;
        path.params[current_query_param].value = &(path_str[index + 1]);
        consuming_key = false;

        current_string_start = index + 1;
        consuming_value = true;
        continue;
      }
    }

    if(consuming_value) {
      if(path_str[index] == '&') {
        path.params[current_query_param].value_len = index - current_string_start;
        if((current_query_param + 1) >= 20) { break; }
        current_query_param++;
        path.params[current_query_param].key = &(path_str[index + 1]);
        consuming_value = false;

        current_string_start = index + 1;
        consuming_key = true;
        continue;
      }

      if((index + 1) >= path_len) {
        path.params[current_query_param].value_len = index - current_string_start + 1;
        current_query_param++;
      }
    }
  }

  path.params_len = current_query_param;
}

void render_txt(Socket* conn, const std::string& str) {
  std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {}\r\n\r\n{}\r\n";
  std::string msg = fmt::format(header, str.size() + 2, str);
  conn->write((uint8_t*)msg.data(), msg.size());
}

