/**
 * @brief Handles the output.cgi path
 */

#pragma once

#include "app_delegate.h"
#include "httpd_delegate.h"


class OutputCGIRoute : public HTTPDDelegate {
public:
  OutputCGIRoute(AppDelegate& delegate) : _delegate(delegate) {}

  void handle_request(const HTTPRequest& req);

private:
  AppDelegate& _delegate;
};
