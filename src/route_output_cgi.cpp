#include "route_output_cgi.h"

#include "http_utils.h"
#include "logging.h"
#include "socket.h"

#include <string>

namespace {

void parse_cmd(
    const char* cmd,
    uint32_t cmd_len,
    uint32_t* color1,
    uint32_t* color2,
    uint32_t* color3,
    uint32_t* color4,
    uint8_t* intensity,
    uint8_t* speed,
    ColorMode* mode
) {
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


void OutputCGIRoute::handle_request(const HTTPRequest& req) {
  Path path;
  parse_path(path, req.path, req.path_len);

  ColorMode mode = ColorMode::off;
  uint32_t color1 = 0;
  uint32_t color2 = 0;
  uint32_t color3 = 0;
  uint32_t color4 = 0;
  uint8_t intensity = 0;
  uint8_t speed = 0;

  for(uint32_t index = 0; index < path.params_len; index++) {
    log_d("Query Param: {}={}", std::string(path.params[index].key, path.params[index].key_len), std::string(path.params[index].value, path.params[index].value_len));
    if(strncmp(path.params[index].key, "text", 4) == 0) {
      parse_cmd(path.params[index].value, path.params[index].value_len, &color1, &color2, &color3, &color4, &intensity, &speed, &mode);

      log_i("Parsed CMD: Mode: {}, C1: {:X}, C2: {:X}, C3: {:X}, C4: {:X}, Intensity: {:X}, Speed: {:X}", (uint8_t)mode, color1, color2, color3, color4, intensity, speed);

      _delegate.handle_command_update(color1, color2, color3, color4, intensity, speed, mode);

      render_txt(req.connection, "");
    }
  }
}


