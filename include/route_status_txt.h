/**
 * @brief Route handler for status.txt
 */

#pragma once

#include "httpd_delegate.h"

class App;

class RouteStatusTxt : public HTTPDDelegate {
public:
  RouteStatusTxt(App& app) : _app(app) {}

  void handle_request(const HTTPRequest& req);

private:
  App& _app;
};
