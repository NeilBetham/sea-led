/**
 * @brief HTTP Utilities
 */

#pragma once

#include "socket.h"

#include <stdint.h>

struct QueryParam {
  const char* key;
  uint32_t key_len;
  const char* value;
  uint32_t value_len;
};

struct Path {
  const char* path;
  uint32_t path_len;

  QueryParam params[20];
  uint32_t params_len = 0;
};

uint32_t hex_to_int(const char* str, uint32_t len);
uint8_t checksum(const std::string& buffer);
void parse_path(Path& path, const char* path_str, uint32_t path_len);
void render_txt(Socket* conn, const std::string& str);

