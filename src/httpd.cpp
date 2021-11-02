#include "httpd.h"

#include "logging.h"

#include "picohttpparser.h"

namespace {


int32_t parse_http(HTTPRequest& req, const std::string& buffer) {
  return phr_parse_request(
    buffer.data(),
    buffer.size(),
    &req.method,
    &req.method_len,
    &req.path,
    &req.path_len,
    &req.http_minor_rev,
    req.headers,
    &req.headers_len,
    0
  );
}


}


void HTTPD::start() {
  _listen_socket.set_delegate(this);
  _listen_socket.listen(_port);
}

void HTTPD::handle_rx(Socket* conn, std::string& data) {
  HTTPRequest request;

  // Parse all the header info
  int32_t ret = parse_http(request, data);
  if(ret < 0) { log_e("Error parsing HTTP request: {}", ret); }
  log_i("HTTP {} for {}", std::string(request.method, request.method_len), std::string(request.path, request.path_len));

  // Whats left is the body
  request.body = data.data() + ret;
  request.body_len = data.size() - ret;

  request.connection = conn;

  if(_delegate != NULL) {
    _delegate->handle_request(request);
  }
}

void HTTPD::handle_closed(Socket* conn) {
  if(_active_connections.size() == 1) {
    delete _active_connections[0];
    _active_connections.clear();
  } else {
    for(auto sock_it = _active_connections.begin(); sock_it != _active_connections.end(); sock_it++) {
      if(*sock_it == conn) {
        delete *sock_it;
        _active_connections.erase(sock_it);
      }
    }
  }
  log_i("Active Echo Conns: {}", _active_connections.size());
}

void HTTPD::handle_accept(Socket* conn) {
  _active_connections.push_back(conn);
  conn->set_delegate(this);
  log_i("Active Echo Conns: {}", _active_connections.size());
}

