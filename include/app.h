/**
 * @brief The main app responsible for handling requests from phone clients
 */

#pragma once

#include "app_delegate.h"
#include "httpd_delegate.h"
#include "router.h"

// Routes
#include "route_output_cgi.h"
#include "route_info_txt.h"
#include "route_status_txt.h"

#include <stdint.h>


class App : public HTTPDDelegate, public AppDelegate {
public:
  App();

  void set_delegate(AppDelegate* delegate) { _delegate = delegate; }

  // HTTPDDelegate hook
  void handle_request(const HTTPRequest& req);

  // AppDelegate hooks
  void handle_command_update(
    uint32_t color1,
    uint32_t color2,
    uint32_t color3,
    uint32_t color4,
    uint8_t intensity,
    uint8_t speed,
    ColorMode mode
  );

  friend class RouteStatusTxt;

private:
  AppDelegate* _delegate = NULL;
  Router _router;
  OutputCGIRoute _output_cgi_route;
  RouteInfoTxt _route_info_txt;
  RouteStatusTxt _route_status_txt;

  ColorMode _mode = ColorMode::off;

  uint32_t _color1 = 0;
  uint32_t _color2 = 0;
  uint32_t _color3 = 0;
  uint32_t _color4 = 0;

  uint8_t _intensity = 0;
  uint8_t _speed = 0;
};
