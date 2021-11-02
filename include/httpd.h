/**
 * @brief Simple http server
 */

#pragma once

#include "httpd_delegate.h"
#include "socket_delegate.h"
#include "socket.h"

#include <stdint.h>
#include <vector>

class HTTPD : public SocketDelegate {
public:
  HTTPD(uint32_t port) : _port(port) {};

  void start();

  void set_delegate(HTTPDDelegate* delegate) { _delegate = delegate; }


  // Socket Hooks
  void handle_rx(Socket* conn, std::string& data);
  void handle_closed(Socket* conn);
  void handle_accept(Socket* conn);

private:
  uint32_t _port = 0;
  Socket _listen_socket;
  std::vector<Socket*> _active_connections;

  HTTPDDelegate* _delegate = NULL;
};
