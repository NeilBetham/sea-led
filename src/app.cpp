#include "app.h"

#include "http_utils.h"
#include "logging.h"
#include "socket.h"

#include <string>

App::App() : _output_cgi_route(*this), _route_status_txt(*this) {
  _router.register_route("/output.cgi", &_output_cgi_route);
  _router.register_route("/info.txt", &_route_info_txt);
  _router.register_route("/status.txt", &_route_status_txt);
}

void App::handle_request(const HTTPRequest& req) {
  _router.handle_request(req);
  req.connection->flush();
  req.connection->shutdown();
}

void App::handle_command_update(
    uint32_t color1,
    uint32_t color2,
    uint32_t color3,
    uint32_t color4,
    uint8_t intensity,
    uint8_t speed,
    ColorMode mode
) {
  _color1 = color1;
  _color2 = color2;
  _color3 = color3;
  _color4 = color4;
  _intensity = intensity;
  _speed = speed;
  _mode = mode;

  if(_delegate != NULL) {
    _delegate->handle_command_update(_color1, _color2, _color3, _color4, _intensity, _speed, _mode);
  }
}


