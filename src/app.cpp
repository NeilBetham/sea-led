#include "app.h"

#include "logging.h"
#include "socket.h"

#include <string>


namespace {


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

void parse_cmd(const char* cmd, uint32_t cmd_len, uint32_t* color1, uint32_t* color2, uint32_t* color3, uint32_t* color4, uint8_t* intensity, uint8_t* speed, ColorMode* mode) {
  bool found_cmd = false;
  uint32_t cmd_start = 0;

  // First we need to skip past the "CMD.1=" text
  for(uint32_t index = 0; index < cmd_len; index++) {
    if(found_cmd == false) {
      if(cmd[index] == '=') {
        found_cmd = true;
      }
      continue;
    }

    cmd_start = index;
    break;
  }

  const char* loc_cmd = &cmd[cmd_start];

  // We should know where everything is now
  // Mode
  *mode = (ColorMode)(loc_cmd[2] - '0');

  // Colors
  *color1 = hex_to_int(&loc_cmd[7], 8);
  *color2 = hex_to_int(&loc_cmd[19], 8);
  *color3 = hex_to_int(&loc_cmd[31], 8);
  *color4 = hex_to_int(&loc_cmd[43], 8);

  // Intensity
  *intensity = hex_to_int(&loc_cmd[54], 2);

  // Speed
  *speed = hex_to_int(&loc_cmd[60], 2);
}



} // namespace


void App::handle_request(const HTTPRequest& req) {
  Path path;
  parse_path(path, req.path, req.path_len);

  for(uint32_t index = 0; index < path.params_len; index++) {
    log_d("Query Param: {}={}", std::string(path.params[index].key, path.params[index].key_len), std::string(path.params[index].value, path.params[index].value_len));
    if(strncmp(path.params[index].key, "text", 4) == 0) {
      uint32_t color1 = 0;
      uint32_t color2 = 0;
      uint32_t color3 = 0;
      uint32_t color4 = 0;
      uint8_t intensity = 0;
      uint8_t speed = 0;
      ColorMode mode;

      parse_cmd(path.params[index].value, path.params[index].value_len, &color1, &color2, &color3, &color4, &intensity, &speed, &mode);

      log_i("Parsed CMD: Mode: {}, C1: {:X}, C2: {:X}, C3: {:X}, C4: {:X}, Intensity: {:X}, Speed: {:X}", (uint8_t)mode, color1, color2, color3, color4, intensity, speed);

      const char msg[] = "HTTP/1.1 200 OK\r\nContent-Length:0\r\n\r\n";
      req.connection->write((uint8_t*)msg, sizeof(msg) / sizeof(char));
    }
  }
}
