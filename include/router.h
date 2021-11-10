/**
 * @brief Simple string matching router for URL requests
 */

#pragma once

#include "httpd_delegate.h"

#include <string>
#include <string.h>
#include <utility>
#include <vector>


class Router : public HTTPDDelegate {
public:
  void register_route(const std::string& route_path, HTTPDDelegate* route_delegate);
  void handle_request(const HTTPRequest& req);

private:
  std::vector<std::pair<std::string, HTTPDDelegate*>> _route_delegates;
};
