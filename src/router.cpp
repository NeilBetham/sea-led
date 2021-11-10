#include "router.h"

void Router::register_route(const std::string& route_path, HTTPDDelegate* route_delegate) {
  _route_delegates.push_back(std::make_pair(route_path, route_delegate));
}

void Router::handle_request(const HTTPRequest& req) {
  // First thing we need to do is get the path sans query string
  uint32_t path_stop = 0;
  for(uint32_t index = 0; index < req.path_len; index++) {
    if(req.path[index] == '?') {
      break;
    }
    path_stop++;
  }

  // Next match the path to a route
  if(path_stop > req.path_len) {
    path_stop = req.path_len;
  }
  for(auto& route_pair : _route_delegates) {
    if(strncmp(req.path, route_pair.first.data(), path_stop) == 0) {
      route_pair.second->handle_request(req);
    }
  }
}
