#include "route_status_txt.h"

#include "app.h"
#include "http_utils.h"

#define FMT_HEADER_ONLY
#include <fmt/core.h>


void RouteStatusTxt::handle_request(const HTTPRequest& req) {
  static const std::string status_line = "STATUS=M:{},C1:{:08x},C2:{:08x},C3:{:08x},C4:{:08x},I:{:02x},SP:{:02x},ST:00,ER:00,CS:";
  std::string resp = fmt::format(status_line, (uint8_t)_app._mode, _app._color1, _app._color2, _app._color3, _app._color4, _app._intensity, _app._speed);
  uint8_t cs = checksum(resp);
  resp = fmt::format("{}{:02x}", resp, cs);
  render_txt(req.connection, resp);
}
